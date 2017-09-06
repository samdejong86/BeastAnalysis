/*
 *   This class holds the data from a root file. Methods are described in detail in the .cc file in src/
 *
 * by Sam de Jong
 *
 */



#include <iostream>
#include <sstream>
#include <fstream>


#include "TString.h"
#include "TMatrixD.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"

using namespace std;

#ifndef DATAREADER_h
#define DATAREADER_h 1


class dataReader{

 public:

  dataReader(){}  
  dataReader(const dataReader &obj);
  dataReader(TString inFile, TString DataBranch, TString ring, int nch);

  //set file name
  void setFile(TString inFile){inputFile=inFile;}

  //get methods
  TMatrixD getYData(int ch){return ydata[ch];}
  TMatrixD getErrors(int ch){return errors[ch];}
  TMatrixD getX(){return X;}
  vector<double> getBS(){return BS;}
  vector<int> getN2(){return n2;}
  TGraphErrors *getGraph(int channel);
  TH1F *getEmptyHist(TString name, TString title, int fillColour);
  double getNChannels(){return nChannels;}
  TString getDataBranchName() { return DataBranchName; }

  void readRunTimes();
  void readData();

  TString displayname(TString branchName){

    if(branchName=="LYSO_hitRate_forward") return "Forward LYSO";
    else if(branchName=="CSI_Pure_hitRate_forward") return "Forward Pure CSI";
    else if(branchName=="BGO_dose_forward") return "Forward BGO";
    else if(branchName=="BGO_dose_backward") return "Backward BGO";
    else if(branchName=="HE3_rate") return "Helium-3 tube rate";
    else if(branchName=="BGO_dose") return "BGO dose [mRad/s]";
    else return branchName.ReplaceAll("_", " ");
        
  }


 private:

  vector<TMatrixD> ydata;
  vector<TMatrixD> errors;
  TMatrixD X;
  vector<UInt_t> subrunStart;
  vector<UInt_t> subrunEnd;
  TString Ring;
  TString inputFile;
  TString DataBranchName;
  int nSubRun;
  bool sim;
  vector<double> BS;
  vector<int> n2;
  int nChannels;

};

#endif
