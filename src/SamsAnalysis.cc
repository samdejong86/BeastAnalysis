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
 *
 *  This file contains the main method of the analysis. There are several objects used by the
 *  analysis:
 *
 *  beamSim - performs scaling of simulation
 *  dataReader - reads and holds data
 *  TouschekSolver - performs Touschek fit, contains results
 *  dataSimRatio - calculates and contains data/sim ratio
 *  SystematicHolder - contains systematic uncertainties.
 */

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
#include "xmlParser.h"

beamSim Params;
int nC=1;
XmlParser inputs;

#include "BadChannel.h"
#include "getWeights.h"
#include "doSystematicStudy.h"  
#include "Print.h"

void doIt(TString DataBranchName, TString forwardOrBackward = ""){

  //what style will be used?
  #ifdef BELLE
  TString belle="1";
  cout<<"Using Belle II style\n";
  #else
  TString belle="0";
  cout<<"Not using Belle II style\n";
  #endif

  //set style
  thisStyle();

  //pdf or png output?
  #ifdef PDF
  TString imageType="pdf";
  #else
  TString imageType="png";
  #endif

  TCanvas *c1 = new TCanvas();

  TString inputBranchName = DataBranchName;
   
  //get the bad channels
  badCh = getBadChannels(DataBranchName);
  //get forward or backward channels
  if(forwardOrBackward!="") forwardBackward(DataBranchName, forwardOrBackward, badCh);  

  //create some directories of they don't exist
  TString CMD = "mkdir -p figs/"+inputBranchName+forwardOrBackward;
  gROOT->ProcessLine(".! mkdir -p figs");
  gROOT->ProcessLine(".! "+CMD);
  gROOT->ProcessLine(".! mkdir -p rootFiles");
 
  inputs = XmlParser("data/inputs.xml");


  //location of data ntuples
  TString HERfile = inputs.getStringValue("HERfile");
  TString LERfile = inputs.getStringValue("LERfile");

  //determine the number of channels in this detector
  nC = getNumberOfChannels(DataBranchName, LERfile);
  
  //-------------1) Initial weighting of simulation-------------

  //get simulation parameters
  Params = beamSim(inputs.getStringValue("SimulationReference"), DataBranchName,  nC);
  Params.getSimBeamPars();

  //perform scaling of simulation
  Params.Simulate(LERfile, "rootFiles/mc_Initial_LER_"+DataBranchName+".root");
  Params.Simulate(HERfile, "rootFiles/mc_Initial_HER_"+DataBranchName+".root");

  //-------------2) Fit the beam-gas and Touschek componets of data and simulation-------------

  //get data
  dataReader dataLER(LERfile, DataBranchName, "LER", nC);
  dataReader dataHER(HERfile, DataBranchName, "HER", nC);
  dataLER.readData();
  dataHER.readData();

  //get simulated data
  dataReader simLER("rootFiles/mc_Initial_LER_"+DataBranchName+".root", DataBranchName, "LER", nC);
  dataReader simHER("rootFiles/mc_Initial_HER_"+DataBranchName+".root", DataBranchName, "HER", nC);
  simLER.readData();
  simHER.readData();

  //generate histograms for drawing Touschek fit result
  TH1F *histCouLER = dataLER.getEmptyHist("CouL", "CouL", 1003);
  TH1F *histCouHER = dataHER.getEmptyHist("CouH", "CouH", 1003);
  
  TH1F *histTouLER = dataLER.getEmptyHist("TouL", "TouL", 1004);
  TH1F *histTouHER = dataHER.getEmptyHist("TouH", "TouH", 1004);


  //create and resize vectors of TouschekSolvers
  vector<TouschekSolver> solnLER;
  vector<TouschekSolver> solnHER;
  vector<TouschekSolver> solnLER_sim;
  vector<TouschekSolver> solnHER_sim;
  solnLER.resize(nC);
  solnHER.resize(nC);
  solnLER_sim.resize(nC);
  solnHER_sim.resize(nC);
  
  
  stringstream ss;

  //loop over channels, performing solution for all
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;   //skip bad channels
    
    //solve
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

    //draw solution and save to file
    solnLER[i].draw(dataLER.getGraph(i),histCouLER, histTouLER);
    TString imageFileLER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_LER_Data-"+ch;
    c1->Print(imageFileLER+"."+imageType);

    solnHER[i].draw(dataHER.getGraph(i),histCouHER, histTouHER);
    TString imageFileHER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_HER_Data-"+ch;
    c1->Print(imageFileHER+"."+imageType);

    solnLER_sim[i].draw(simLER.getGraph(i),histCouLER, histTouLER);
    imageFileLER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_LER_Initial_Simulation-"+ch;
    c1->Print(imageFileLER+"."+imageType);

    solnHER_sim[i].draw(simHER.getGraph(i),histCouHER, histTouHER);
    imageFileHER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_HER_Initial_Simulation-"+ch;
    c1->Print(imageFileHER+"."+imageType);


  }
 
  //-------------3) Re-weighting of simulation using data-simulation ratios-------------

  // get pscale scale factors
  double PScaleErrLER;
  double PScaleErrHER;
  double PressureScaleLER = getWeights(solnLER, solnLER_sim, PScaleErrLER);
  double PressureScaleHER = getWeights(solnHER, solnHER_sim, PScaleErrHER);
  
		       
  //redo simulation, incorporating pscale factors
  Params.setPressureScaleLER(PressureScaleLER);  
  Params.setPressureScaleHER(PressureScaleHER);
  Params.Simulate(LERfile, "rootFiles/mc_reWeight_LER_"+DataBranchName+".root");
  Params.Simulate(HERfile, "rootFiles/mc_reWeight_HER_"+DataBranchName+".root");


  //-------------4) Fitting re-weighted simulation-------------

  //read reweighted simulation file
  dataReader reSimLER("rootFiles/mc_reWeight_LER_"+DataBranchName+".root", DataBranchName, "LER", nC);
  dataReader reSimHER("rootFiles/mc_reWeight_HER_"+DataBranchName+".root", DataBranchName, "HER", nC);
  reSimLER.readData();
  reSimHER.readData();
  
  
  vector<TouschekSolver> solnLER_reSim;
  vector<TouschekSolver> solnHER_reSim;
  solnLER_reSim.resize(nC);
  solnHER_reSim.resize(nC);

  //fit reweighted simulation, draw figures
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;   

   
    solnLER_reSim[i].setVariables("LER", reSimLER.getYData(i), reSimLER.getErrors(i), reSimLER.getX(), true);
    solnLER_reSim[i].Solve(solnLER_sim[i].getTousFitParameters());
    solnLER_reSim[i].calculateVariance();
    
    
    solnHER_reSim[i].setVariables("HER", reSimHER.getYData(i), reSimHER.getErrors(i), reSimHER.getX(), true);
    solnHER_reSim[i].Solve(solnHER_sim[i].getTousFitParameters());
    solnHER_reSim[i].calculateVariance();

     
    ss<<i;
    TString ch = ss.str();
    ss.str("");

    solnLER_reSim[i].draw(reSimLER.getGraph(i),histCouHER, histTouHER);
    TString imageFileLER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_LER_ReWeighted_Simulation-"+ch;
    c1->Print(imageFileLER+"."+imageType);

    solnHER_reSim[i].draw(reSimHER.getGraph(i),histCouHER, histTouHER);
    TString imageFileHER = "figs/"+inputBranchName+forwardOrBackward+"/"+inputBranchName+forwardOrBackward+"_HER_ReWeighted_Simulation-"+ch;
    c1->Print(imageFileHER+"."+imageType); 


  }
 
  

  //print results

  cout<<"\n\n-----------------------------------------HER-----------------------------------------\n";
  Print(solnHER, inputBranchName+forwardOrBackward, "HER", "Data");
  Print(solnHER_sim, inputBranchName+forwardOrBackward, "HER", "Initial Simulation");
  Print(solnHER_reSim, inputBranchName+forwardOrBackward, "HER", "ReWeighted Simulation");


  cout<<"\n\n-----------------------------------------LER-----------------------------------------\n";
  Print(solnLER, inputBranchName+forwardOrBackward, "LER", "Data");
  Print(solnLER_sim, inputBranchName+forwardOrBackward, "LER", "Initial Simulation");
  Print(solnLER_reSim, inputBranchName+forwardOrBackward, "LER", "ReWeighted Simulation");


  
  //4) fit the re-weighted simulation and get data/sim ratios for beam gas and Tousckek for HER and LER
  vector<dataSimRatio> ratios;
  ratios.resize(nC);


  //ratios.addRatios(solnLER, solnLER_reSim, "LER");
  //ratios.addRatios(solnHER, solnHER_reSim, "HER");
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;       
    ratios[i].addRatios(dataLER.getGraph(i), reSimLER.getGraph(i), "LER");
    ratios[i].addRatios(dataHER.getGraph(i), reSimHER.getGraph(i), "HER");
  }

  cout<<"\n\n----Reweighting-Factors----\n";
  cout<<"PscaleZ^2 for HER: ";
  cout<<PressureScaleHER<<"+/-"<<PScaleErrHER<<endl;
  cout<<"Pscale    for LER: ";
  cout<<PressureScaleLER<<"+/-"<<PScaleErrLER<<endl;

  cout<<"\n\n------------Results------------\n";
  cout<<"HER\tLER\n";
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;        
    cout<<ratios[i];
  }

  
  //------------------Systematics------------------
  
  
  vector<SystematicHolder> Systematics;
  Systematics.resize(nC);
  
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;    
    Systematics[i].setData(ratios[i]);
    Systematics[i].setPScale(PressureScaleHER, PressureScaleLER);
    Systematics[i].setPScaleError(PScaleErrHER, PScaleErrLER);

  }

  doSystematicStudy(Systematics, PScaleErrHER, PScaleErrLER);
  
  //print systematic uncertainties to console
  cout<<endl<<endl;
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;    
    cout<<i<<"\t"<<Systematics[i];
  }

  
  //print systematics in xml file
  TString filename = "data/Systematics.xml";

  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;  
    ss<<i;
    TString num=ss.str();
    Systematics[i].xmlPrint(inputBranchName+forwardOrBackward+num, filename);
    ss.str("");
  }
  /*

  //produce systematic plots using python script
  #ifdef PLOTSYSTEMATICS
  TString command = "python SystematicPlotter.py "+belle+" 0 "+imageType;
  gROOT->ProcessLine(".! "+command);
  #endif
  */

  		     
}

int main(int argc,char** argv) {

  if(argc<2){
    cout<<"Usage:  SamsAnalysis BranchName forwardorbackward\n";
    return 0;
  }
 
  doIt(argv[1], argv[2]);
  
}
