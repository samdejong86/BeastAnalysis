/*
 *  This class contains the simulation. It get the simulation parameters from the truth branch
 *  of one of Igal's simulation ntuples, then scales them by the current, pressure, and beam size
 *  from the data. Methods are described in detail in .cc file in src/
 *
 *  by Sam de Jong
 *
 */



#include <iostream>
#include <sstream>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;


#ifndef BEAMSIM_h
#define BEAMSIM_h 1


class beamSim{

 public:
  beamSim(){} 
  beamSim(const beamSim &obj);
  beamSim(TString file, TString DataBranch, int nCh);
 
  //set methods
  void setPressureScaleLER(double scale){PressureScaleLER=scale;}
  void setPressureScaleHER(double scale){PressureScaleHER=scale;}
  void setFilename(TString file){ filename=file;}

  //for running a perturbed simulation
  void setCurrentPerturbation(double perturbation){ currentPerturb=perturbation;}
  void setBeamSizePerturbation(double perturbation){ beamSizePerturb=perturbation;}
  void setPScalePerturbation(double perturbation){ PScalePerturb=perturbation;}
  void setEfficiencies(vector<double> eff){ Efficiencies=eff;}

  //get methods
  vector<double> getLT(){return ParamLT;}
  vector<double> getHT(){return ParamHT;}
  vector< vector<double> > getLBG() {return ParamLBGall;}
  vector< vector<double> > getHBG() {return ParamHBGall;}
  TString getDataBranchName(){return DataBranchName;}
  int getNChannels(){return nChannels;}
  
  TString getSimBranchName();
  void getSimBeamPars();
  
  void Simulate(TString inputFile, TString outfilename);
  void PerturbedSimulate(TString inputFile, TString outfilename, bool positive);


 
 private:
  vector<double> ParamLT;
  vector<double> ParamHT;
  vector<double> Efficiencies;
  vector< vector<double> > ParamLBGall;
  vector< vector<double> > ParamHBGall;
  TString filename;
  TString SimBranchName;
  TString DataBranchName;
  int nChannels;
  double PressureScaleLER;
  double PressureScaleHER;

  double currentPerturb;
  double beamSizePerturb;
  double PScalePerturb;
  
};
 #endif
