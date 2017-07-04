#include <iostream>
#include <sstream>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

/*
 * This header gets the simulation parameters from the truth branch of the mc sample
 */

using namespace std;


#ifndef BEAMSIM_h
#define BEAMSIM_h 1


class beamSim{

 public:
  beamSim(){} 
  beamSim(const beamSim &obj);
  beamSim(TString file, TString DataBranch, int nCh);
 
  void setPressureScaleLER(double scale){PressureScaleLER=scale;}
  void setPressureScaleHER(double scale){PressureScaleHER=scale;}
  void setFilename(TString file){ filename=file;}
  void setCurrentPerturbation(double perturbation){ currentPerturb=perturbation;}
  void setBeamSizePerturbation(double perturbation){ beamSizePerturb=perturbation;}
  void setPScalePerturbation(double perturbation){ PScalePerturb=perturbation;}


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
