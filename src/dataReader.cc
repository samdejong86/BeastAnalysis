
using namespace std;

#include "dataReader.h"

dataReader::dataReader(const dataReader &obj){
  this->X.ResizeTo(obj.X.GetNrows(), obj.X.GetNcols());
  this->X = obj.X;

  this->inputFile = obj.inputFile;
  this->DataBranchName = obj.DataBranchName;
  this->nSubRun = obj.nSubRun;
  this->sim = obj.sim;
  this->Ring = obj.Ring;
  this->nChannels = obj.nChannels;
  
  //this->ydata.resize(obj.ydata.size());
  //this->errors.resize(obj.errors.size());
  this->BS.resize(obj.BS.size());
  this->n2.resize(obj.n2.size());
  
  for(int i=0; i<(int)obj.ydata.size(); i++)
    this->ydata.push_back(obj.ydata[i]);
  
  for(int i=0; i<(int)obj.errors.size(); i++)
    this->errors.push_back(obj.errors[i]);
  
  for(int i=0; i<(int)obj.BS.size(); i++) this->BS[i] = obj.BS[i];
  for(int i=0; i<(int)obj.n2.size(); i++) this->n2[i] = obj.n2[i];
  
}

dataReader::dataReader(TString inFile, TString DataBranch, TString ring, int nch){
  Ring = ring;
  inputFile=inFile;
  sim=inputFile.Contains("mc");
  DataBranchName=DataBranch;
  nChannels=nch;
  
  readRunTimes();
    
}

TGraphErrors *dataReader::getGraph(int channel){

    TGraphErrors *gr_real = new TGraphErrors();
    
    double large=0;
    for(int i=0; i<ydata[channel].GetNrows(); i++){
      if(ydata[channel][i][0]>large) large=ydata[channel][i][0];
      
      gr_real->SetPoint(i, i+0.5,ydata[channel][i][0]);
      gr_real->SetPointError(i, 0, errors[channel][i][0]);
      
    }

    gr_real->SetMaximum(large);
    gr_real->SetMarkerStyle(20);

    return gr_real;

}

TH1F *dataReader::getEmptyHist(TString name, TString title, int fillColour){
    TH1F *h = new TH1F(name, title, 1, -0, 1);
    
    stringstream ss;
    for(int i=0; i<X.GetNrows(); i++){
    char label[100];
      sprintf(label, "%.0f", BS[i]/n2[i]);
      
      TString l = label;
      l = l +" #mum";
    
      for(int j=0; j<i; j++) l = l+" ";
      h->Fill(l, 1);

    }
    h->SetFillColor(fillColour);
    h->GetYaxis()->SetTitle(DataBranchName);
    
    return h;

}

void dataReader::readRunTimes(){
    
    subrunStart.clear();
    subrunEnd.clear();

    ifstream InFile; 
    InFile.open("data/"+Ring+"SubRunTimes.dat");

    while(!InFile.eof()){
      //for(int i=0; i<nSubRun; i++){
      UInt_t a, b;
      InFile>>a;
      if(a==5) break;
      InFile>>b;
      subrunStart.push_back(a);
      subrunEnd.push_back(b);
      
    }
    InFile.close();
    
    nSubRun=subrunStart.size();

}

void dataReader::readData(){
    readRunTimes();

    TFile *f = new TFile(inputFile);
    
    TTree *tout = (TTree*)f->Get("tout");
    
    UInt_t ts;
    vector<double> *rate = new vector<double>();
    vector<double> *Current = new vector<double>();
    vector<double> *Pressure = new vector<double>();
    vector<double> *BeamSize = new vector<double>();
    vector<int> *injFlag = new vector<int>();
    vector<double> *nBunch = new vector<double>();
    vector<double> *Zeff = new vector<double>();


    tout->SetBranchAddress("ts", &ts);
    tout->SetBranchAddress(DataBranchName, &rate);
    tout->SetBranchAddress("SKB_"+Ring+"_current", &Current);
    tout->SetBranchAddress("SKB_"+Ring+"_pressure_average", &Pressure);
    tout->SetBranchAddress("SKB_"+Ring+"_correctedBeamSize_xray_Y", &BeamSize);
    tout->SetBranchAddress("SKB_"+Ring+"_injectionFlag_safe", &injFlag);
    if(Ring=="LER"){
      tout->SetBranchAddress("SKB_LER_Zeff_D02", &Zeff);
      tout->SetBranchAddress("SKB_"+Ring+"_injectionNumberOfBunches", &nBunch);    
    } else {
      Zeff->push_back(1);
      nBunch->push_back(1576);
    }
    
    double **Rate = new double*[nSubRun];
    double **Mean = new double*[nSubRun];
    double **n = new double*[nSubRun];
    double **M2 = new double*[nSubRun];
    for(int i=0; i<nSubRun; i++){
      Rate[i] = new double[nChannels];
      Mean[i] = new double[nChannels];
      n[i] = new double[nChannels];
      M2[i] = new double[nChannels];
      for(int j=0; j<nChannels; j++){
	Rate[i][j] = 0;
	Mean[i][j] = 0;
	n[i][j] = 0;
	M2[i][j] = 0;
      }
    }
    
    
    vector<double> Cur;
    vector<double> Pre;  
    vector<double> Zef;
    vector<double> nBun;

    Cur.resize(nSubRun);
    BS.resize(nSubRun);    
    Pre.resize(nSubRun);  
    Zef.resize(nSubRun);   
    nBun.resize(nSubRun);    
    n2.resize(nSubRun);
    
    
    int nentries = tout->GetEntries();
    for(int i=0; i<nentries; i++){
      tout->GetEntry(i);
      
      if((int)rate->size()!=nChannels) continue;
      if(Pressure->size()!=1) continue;
      if(Current->size()!=1) continue;
      if(BeamSize->size()!=1) continue;
      if(injFlag->size()!=1) continue;
      
      if(Pressure->at(0)!=Pressure->at(0))continue;
      if(Current->at(0)!=Current->at(0))continue;
      if(BeamSize->at(0)!=BeamSize->at(0))continue;
      
      if(BeamSize->at(0)>200||BeamSize->at(0)<15) continue;
      
      if(injFlag->at(0)!=0) continue;
      
      for(int j=0; j<nSubRun; j++){
	if(ts<subrunEnd[j]&&ts>subrunStart[j]){
	  
	  Cur[j] += Current->at(0);
	  Pre[j] += Pressure->at(0);
	  BS[j] += BeamSize->at(0);
	  Zef[j] += Zeff->at(0);
	  nBun[j] += nBunch->at(0);

	  for(int k=0; k<nChannels; k++){
	    n[j][k]++;
	    double delta = rate->at(k) - Mean[j][k];
	    Mean[j][k] += delta/n[j][k];
	    double delta2=rate->at(k) - Mean[j][k];
	    M2[j][k] +=delta*delta2;
	  }
	  n2[j]++;
	  
	}
      }
    }
  
    f->Close();
    
    ydata.clear();    
    errors.clear();
    
    ydata.resize(nChannels);    
    errors.resize(nChannels);
    
    for(int i=0; i<nChannels; i++){
      ydata[i].ResizeTo(nSubRun, 1);
      errors[i] .ResizeTo(nSubRun,1);
    }
    
    
    X.ResizeTo(nSubRun,2);    
    
    
    for(int i=0; i<nSubRun; i++){
    
      X[i][0] = (Cur[i]/n2[i])*(Pre[i]/n2[i])*(Zef[i]/n2[i])*(Zef[i]/n2[i]);
      X[i][1] = (Cur[i]/n2[i])*(Cur[i]/n2[i])/(BS[i]/n2[i]*nBun[i]/n2[i]);
      
      
      for(int j=0; j<nChannels; j++){
	ydata[j][i][0] = Mean[i][j];
	errors[j][i][0]= sqrt(M2[i][j]/(n[i][j]-1))/sqrt(n[i][j]);
      }
      
    }
    for(int i=0; i<nSubRun; i++){
      delete[] Rate[i];
      delete[] Mean[i];
      delete[] n[i];
      delete[] M2[i];
      //delete[] error[i];
    }
    
    delete[] Rate;
    delete[] Mean;
    delete[] n;
    delete[] M2;
    //delete[] error;
    
    delete rate;
    delete Current;
    delete Pressure;
    delete BeamSize;
    delete injFlag;
    delete nBunch;
    delete Zeff;
  }
