#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include <TMatrixD.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>

using namespace std;

vector<int> badCh;

#include "beamSim.h"  //a class
#include "dataReader.h"   //a class
#include "TouschekSolver.h" //a class
#include "utilities.h"
#include "style.h"
#include "dataSimRatio.h"
#include "SystematicHolder.h"

/*
 *  This program performs my (Sam's) simulation weighting scheme as described in
 *  my thesis. There are four stages to this analysis:
 *
 *   1) Initial weighting of simulation
 *   2) Fitting of Touschek and Beam gas components of data and simulation
 *   3) Re-weighting of simulation using data-simulation ratios
 *   4) Fitting re-weighted simulation
 *
 *  This analysis has been modified to use any (detector) branch in the BEAST data ntuples.
 */
  

int nC=1;
beamSim Params;

#include "BadChannel.h"
#include "getWeights.h"
#include "doSystematicStudy.h"  


void doIt(TString DataBranchName, TString forwardOrBackward = ""){

  #ifdef BELLE
  TString belle="1";
  cout<<"Using Belle II style\n";
  #else
  TString belle="0";
  cout<<"Not using Belle II style\n";
  #endif

  thisStyle();

  #ifdef PDF
  TString imageType="pdf";
  #else
  TString imageType="png";
  #endif

  TCanvas *c1 = new TCanvas();

  TString inputBranchName = DataBranchName;
   
  badCh = getBadChannels(DataBranchName);
  forwardBackward(DataBranchName, forwardOrBackward, badCh);


  TString CMD = "mkdir -p figs/"+inputBranchName+forwardOrBackward;
  gROOT->ProcessLine(".! mkdir -p figs");
  gROOT->ProcessLine(".! "+CMD);
  gROOT->ProcessLine(".! mkdir -p rootFiles");
 
  
  TString HERfile = "../../ntuples/v3/BEAST_run200789.root";
  TString LERfile = "../../ntuples/v3/BEAST_run300789.root";

  
  nC = getNumberOfChannels(DataBranchName, LERfile);
  

  Params = beamSim("../../ntuples/Sim_v3/mc_beast_run_2005.root", DataBranchName,  nC);
  Params.getSimBeamPars();

  Params.Simulate(LERfile, "rootFiles/mc_Initial_LER_"+DataBranchName+".root");
  Params.Simulate(HERfile, "rootFiles/mc_Initial_HER_"+DataBranchName+".root");

  

  //2) Fit the beam-gas and Touschek componets of data and simulation

  dataReader dataLER(LERfile, DataBranchName, "LER", nC);
  dataReader dataHER(HERfile, DataBranchName, "HER", nC);
  dataLER.readData();
  dataHER.readData();

  dataReader simLER("rootFiles/mc_Initial_LER_"+DataBranchName+".root", DataBranchName, "LER", nC);
  dataReader simHER("rootFiles/mc_Initial_HER_"+DataBranchName+".root", DataBranchName, "HER", nC);
  simLER.readData();
  simHER.readData();

  TH1F *histCouLER = dataLER.getEmptyHist("CouL", "CouL", 1003);
  TH1F *histCouHER = dataHER.getEmptyHist("CouH", "CouH", 1003);
  
  TH1F *histTouLER = dataLER.getEmptyHist("TouL", "TouL", 1004);
  TH1F *histTouHER = dataHER.getEmptyHist("TouH", "TouH", 1004);


  vector<TouschekSolver> solnLER;
  vector<TouschekSolver> solnHER;
  vector<TouschekSolver> solnLER_sim;
  vector<TouschekSolver> solnHER_sim;

  
  solnLER.resize(nC);
  solnHER.resize(nC);
  solnLER_sim.resize(nC);
  solnHER_sim.resize(nC);
  
  
  stringstream ss;

  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;   
    
    
    solnLER[i].setVariables("LER", dataLER.getYData(i), dataLER.getErrors(i), dataLER.getX(), false);
    solnLER[i].Solve(0);
    solnLER[i].calculateVariance();
    
    
    solnHER[i].setVariables("HER", dataHER.getYData(i), dataHER.getErrors(i), dataHER.getX(), false);
    solnHER[i].Solve(0);
    solnHER[i].calculateVariance();

   
    solnLER_sim[i].setVariables("LER", simLER.getYData(i), simLER.getErrors(i), simLER.getX(), true);
    solnLER_sim[i].Solve(0);
    solnLER_sim[i].calculateVariance();
    
    
    solnHER_sim[i].setVariables("HER", simHER.getYData(i), simHER.getErrors(i), simHER.getX(), true);
    solnHER_sim[i].Solve(0);
    solnHER_sim[i].calculateVariance();
    
    ss<<i;
    TString ch = ss.str();
    ss.str("");
    


    solnLER[i].draw(dataLER.getGraph(i),histCouLER, histTouLER);
    TString imageFileLER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_LER_Data_"+ch;
    c1->Print(imageFileLER+"."+imageType);

    solnHER[i].draw(dataHER.getGraph(i),histCouHER, histTouHER);
    TString imageFileHER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_HER_Data_"+ch;
    c1->Print(imageFileHER+"."+imageType);



    solnLER_sim[i].draw(simLER.getGraph(i),histCouLER, histTouLER);
    imageFileLER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_LER_Initial_Simulation_"+ch;
    c1->Print(imageFileLER+"."+imageType);

    solnHER_sim[i].draw(simHER.getGraph(i),histCouHER, histTouHER);
    imageFileHER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_HER_Initial_Simulation_"+ch;
    c1->Print(imageFileHER+"."+imageType);


  }
 

  double PScaleErrLER;
  double PScaleErrHER;

  double PressureScaleLER = getWeights(solnLER, solnLER_sim, PScaleErrLER);
  double PressureScaleHER = getWeights(solnHER, solnHER_sim, PScaleErrHER);
  
		       
 
  Params.setPressureScaleLER(PressureScaleLER);  
  Params.setPressureScaleHER(PressureScaleHER);
  
  Params.Simulate(LERfile, "rootFiles/mc_reWeight_LER_"+DataBranchName+".root");
  Params.Simulate(HERfile, "rootFiles/mc_reWeight_HER_"+DataBranchName+".root");

  dataReader reSimLER("rootFiles/mc_reWeight_LER_"+DataBranchName+".root", DataBranchName, "LER", nC);
  dataReader reSimHER("rootFiles/mc_reWeight_HER_"+DataBranchName+".root", DataBranchName, "HER", nC);
  reSimLER.readData();
  reSimHER.readData();
  
  
  vector<TouschekSolver> solnLER_reSim;
  vector<TouschekSolver> solnHER_reSim;
  solnLER_reSim.resize(nC);
  solnHER_reSim.resize(nC);

  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;   

   
    solnLER_reSim[i].setVariables("LER", reSimLER.getYData(i), reSimLER.getErrors(i), reSimLER.getX(), true);
    solnLER_reSim[i].Solve(solnLER_sim[i].getTousFitParameters());
    //solnLER_reSim[i].Solve(0);
    solnLER_reSim[i].calculateVariance();
    
    
    solnHER_reSim[i].setVariables("HER", reSimHER.getYData(i), reSimHER.getErrors(i), reSimHER.getX(), true);
    solnHER_reSim[i].Solve(solnHER_sim[i].getTousFitParameters());
    //solnHER_reSim[i].Solve(0);
    solnHER_reSim[i].calculateVariance();

     
    ss<<i;
    TString ch = ss.str();
    ss.str("");

    solnLER_reSim[i].draw(reSimLER.getGraph(i),histCouHER, histTouHER);
    TString imageFileLER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_LER_ReWeighted_Simulation_"+ch;
    c1->Print(imageFileLER+"."+imageType);

    solnHER_reSim[i].draw(reSimHER.getGraph(i),histCouHER, histTouHER);
    TString imageFileHER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_HER_ReWeighted_Simulation_"+ch;
    c1->Print(imageFileHER+"."+imageType); 


  }
 

  cout<<"\n\n-----------------------------------------LER-----------------------------------------\n";
  cout<<"data\n";
  cout<<right<<setw(8)<<"Channel";
  solnLER[0].PrintTitle(cout);
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;
    cout<<right<<setw(8)<<i<<solnLER[i]<<endl;
  }
  cout<<endl;
  cout<<"initial sim\n";
  cout<<right<<setw(8)<<"Channel";
  solnLER[0].PrintTitle(cout);
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;
    cout<<right<<setw(8)<<i<<solnLER_sim[i]<<endl;
  }
  cout<<endl;
  cout<<"reweighted sim\n";
  cout<<right<<setw(8)<<"Channel";
  solnLER[0].PrintTitle(cout);
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;
    cout<<right<<setw(8)<<i<<solnLER_reSim[i]<<endl;
  } 


  cout<<"\n\n-----------------------------------------HER-----------------------------------------\n";
  cout<<"data\n";
  cout<<right<<setw(8)<<"Channel";
  solnLER[0].PrintTitle(cout);
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;
    cout<<right<<setw(8)<<i<<solnHER.at(i)<<endl;
  }
  cout<<endl;
  cout<<"initial sim\n";
  cout<<right<<setw(8)<<"Channel";
  solnLER[0].PrintTitle(cout);
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;
    cout<<right<<setw(8)<<i<<solnHER_sim[i]<<endl;  
  }
  cout<<endl;
  cout<<"reweighted sim\n";
  cout<<right<<setw(8)<<"Channel";
  solnLER[0].PrintTitle(cout);
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;
    cout<<right<<setw(8)<<i<<solnHER_reSim[i]<<endl;
  }

  //4) fit the re-weighted simulation and get data/sim ratios for beam gas and Tousckek for HER and LER
  dataSimRatio ratios;


  ratios.addRatios(solnLER, solnLER_reSim, "LER");
  ratios.addRatios(solnHER, solnHER_reSim, "HER");


  cout<<"\n\n----Reweighting-Factors----\n";
  cout<<"PscaleZ^2 for HER: ";
  cout<<PressureScaleHER<<"+/-"<<PScaleErrHER<<endl;
  cout<<"Pscale    for LER: ";
  cout<<PressureScaleLER<<"+/-"<<PScaleErrLER<<endl;

  cout<<ratios;


 
  //------------------Systematics-----------------------------------------------------
  
  
  SystematicHolder Systematics;
  Systematics.setData(ratios);

  doSystematicStudy(solnHER, solnLER, Systematics, solnHER_sim, solnLER_sim, PScaleErrHER, PScaleErrLER);

  cout<<endl<<endl;
  cout<<Systematics;

   
  TString filename = "data/Systematics.xml";
  Systematics.xmlPrint(inputBranchName+forwardOrBackward, filename);

  #ifdef PLOTSYSTEMATICS

  TString command = "python SystematicPlotter.py "+belle+" 0 "+imageType;
  
  gROOT->ProcessLine(".! "+command);

  #endif

  		     
}

int main(int argc,char** argv) {

   
  doIt(argv[1], argv[2]);
  

}
