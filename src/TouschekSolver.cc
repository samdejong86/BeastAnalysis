#include <iostream>
#include <iomanip>


using namespace std;

#include "TouschekSolver.h"

TouschekSolver::TouschekSolver(const TouschekSolver &obj){
       
    this->ydata.ResizeTo(obj.ydata.GetNrows(), obj.ydata.GetNcols());
    this->ydata = obj.ydata;
    
    this->X.ResizeTo(obj.X.GetNrows(), obj.X.GetNcols());
    this->X = obj.X;

    this->errors.ResizeTo(obj.errors.GetNrows(), obj.errors.GetNcols());
    this->errors = obj.errors;

    this->appliedSoln.ResizeTo(obj.appliedSoln.GetNrows(), obj.appliedSoln.GetNcols());
    this->appliedSoln = obj.appliedSoln;

    this->soln.ResizeTo(obj.soln.GetNrows(), obj.soln.GetNcols());
    this->soln = obj.soln;

    this->Var.ResizeTo(obj.Var.GetNrows(), obj.Var.GetNcols());
    this->Var = obj.Var;


    
    this->chiSquare = obj.chiSquare;
    this->ndof = obj.ndof;
    this->sim = obj.sim;
    this->Ring = obj.Ring;

  }


TouschekSolver::TouschekSolver(TString ring, TMatrixD y, TMatrixD err, TMatrixD x, bool isSim){
    setVariables(ring, y,err,x,isSim);
}

void TouschekSolver::setVariables(TString ring, TMatrixD y, TMatrixD err, TMatrixD x, bool isSim){
    int nRowY = y.GetNrows();
    int nColY = y.GetNcols();

    ydata.ResizeTo(nRowY, nColY);
    X.ResizeTo(nRowY, 2);
    errors.ResizeTo(nRowY, nColY);

    ydata=y;
    X=x;
    errors=err;    
    sim=isSim;

    Ring=ring;

    soln.ResizeTo(2,1);
    appliedSoln.ResizeTo(nRowY,1);
    Var.ResizeTo(2,2);

    chiSquare=0;
    

  }

void TouschekSolver::draw(TGraphErrors *data, TH1F *Coulomb, TH1F *Touschek){
    
    for(int i=0; i<X.GetNrows(); i++){
      Touschek->SetBinContent(i+1, soln[1][0]*X[i][1]);
      Coulomb->SetBinContent(i+1,soln[0][0]*X[i][0]);
    }
    THStack *hs = new THStack("hs", "");
    hs->Add(Coulomb);
    hs->Add(Touschek);
    
        double maximum = std::max(Touschek->GetMaximum()+Coulomb->GetMaximum(),data->GetMaximum());
    
    TGraph *grDivider = new TGraph();
    grDivider->SetPoint(0, 5, maximum*2);
    grDivider->SetPoint(1, 5, -10);
    grDivider->SetPoint(2, 10, -10);
    grDivider->SetPoint(3, 10, maximum*2);
    
    hs->SetMaximum(maximum);
    hs->Draw();

   
    hs->GetYaxis()->SetTitle(Coulomb->GetYaxis()->GetTitle());
    hs->GetXaxis()->SetTitle("360mA                                    540mA                                    720mA            ");
    hs->GetXaxis()->SetRangeUser(0, X.GetNrows());
    hs->GetXaxis()->SetTitleOffset(1.2);
    hs->GetXaxis()->SetLabelSize(0.05);

    data->Draw("sameP");
    grDivider->Draw("sameL");

    if(!sim){
    TPaveText *text = new TPaveText(1.1, maximum*0.8, 2.9, maximum*0.9);
    text->AddText("Data");
    text->SetFillColor(kWhite);
    text->SetBorderSize(1);
    text->Draw();
  }else if(sim){
    TPaveText *text = new TPaveText(1.1, maximum*0.8, 3.9, maximum*0.9);
    text->AddText("Simulation");
    text->SetFillColor(kWhite);
    text->SetBorderSize(1);
    text->Draw();
  }
  
    
    
    int colour = 1005;
    if(Ring.Contains("HER")){
      colour=1002;
    }
    
    TPaveText *Beam = new TPaveText(1.1, maximum*0.6, 2.9, maximum*0.7);
    Beam->AddText(Ring)->SetTextColor(colour);
    Beam->SetFillColor(kWhite);
    Beam->SetBorderSize(1);
    Beam->Draw();
    

} 


void TouschekSolver::Solve(double tousParam){
    
    
    TF2 *f2 = new TF2("f2", "[0]*x+[1]*y", 0,1e7,0,30);
    

    if(tousParam!=0&&sim) {
      f2->SetParameter(1, tousParam);
      f2->SetParLimits(1, 0.01*tousParam, 1.99*tousParam);
    }

    
    
    TGraph2DErrors *gr = new TGraph2DErrors();
    for(int j=0; j<X.GetNrows(); j++){
      gr->SetPoint(j, X[j][0], X[j][1], ydata[j][0]);
      gr->SetPointError(j, 0,0,errors[j][0]);
    }
    
    gr->Fit(f2, "QR");
    
    for(int i=0; i<2; i++) soln[i][0] = f2->GetParameter(i);   
    
    appliedSoln = MatrixMultiply(X, soln);

}


void TouschekSolver::calculateVariance(){

    TMatrixD Xt(X.GetNcols(), X.GetNrows());
    for(int i=0; i<X.GetNrows(); i++){
      for(int j=0; j<X.GetNcols(); j++){
	Xt[j][i] = X[i][j];
      }
    }
    
    //multiply X-transpose by X, then invert
    TMatrixD XtX = MatrixMultiply(Xt, X);
    TMatrixD XtXInv = XtX.Invert();
    
    //get the fitting uncertainties  
    TMatrixD difference = ydata-appliedSoln;
    
    double sum=0;
    double sumSq=0;
    int n=difference.GetNrows();
    for(int i=0; i<n; i++){
      sum +=difference[i][0];
      sumSq+=difference[i][0]*difference[i][0];      
    }
    
    double sigmaSq = (sumSq/n)-(sum/n)*(sum/n);
    //cout<<sigmaSq<<endl;
    
    //off diagonal terms are the covariances
    Var = XtXInv*sigmaSq;
   
    this->chiSq();
    this->ndf();

}

double TouschekSolver::chiSq(){
    double chsq=0;

    for(int i=0; i<ydata.GetNrows(); i++){
      
      double errorApplied = sqrt(X[i][1]*X[i][1]*Var[1][1] + X[i][0]*X[i][0]*Var[0][0]);

      double num = (ydata[i][0] - appliedSoln[i][0])*(ydata[i][0] - appliedSoln[i][0])/(errors[i][0]*errors[i][0]+errorApplied*errorApplied);
      chsq+=num;
    }
    chiSquare = chsq;
    return chsq;   
}
  
int TouschekSolver::ndf(){
    ndof=ydata.GetNrows()-1-X.GetNcols();
    return ydata.GetNrows()-1-X.GetNcols();
}

void TouschekSolver::PrintTitle(ostream & out) const{
  out<<right<<setw(13)<<"Beam Gas"<<right<<setw(16)<<"Beam Gas Error"<<right<<setw(16)<<"Touschek"<<right<<setw(16)<<"Tousckek Error"<<right<<setw(11)<<"ChiSq"<<right<<setw(5)<<"ndf"<<endl;
}

void TouschekSolver::printSoln(ostream & out) const{
  out<<right<<setw(13)<<soln[0][0]<<right<<setw(16)<<sqrt(Var[0][0])<<right<<setw(16)<<soln[1][0]<<right<<setw(16)<<sqrt(Var[1][1])<<right<<setw(11)<<chiSquare<<right<<setw(5)<<ndof;

//<<"\t"<<1-ROOT::Math::ROOT::Math::chisquared_cdf (chiSquare, ndof);
}
  

ostream& operator<<(ostream& os, const TouschekSolver& r) {
  r.printSoln(os);
  return os;
}
