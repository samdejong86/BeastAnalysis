/*
 *  This header has several utility functions
 */

TString getSimBranchName(TString DataBranchName){

  if(DataBranchName.Contains("HE3")){
    return "HE3_rate";
  }else if(DataBranchName.Contains("BGO")){
    return "BGO_dose";
  }else if(DataBranchName.Contains("CSI")){
    return "CSI_dose";
  }else if(DataBranchName.Contains("DIA")){
    return "DIA_dose";
  } else {
    cout<<"Making a guess\n";
    return DataBranchName;
  }
}

int getNumberOfChannels(TString DataBranchName, TString dataFile){

  TFile *f = new TFile(dataFile);
  TTree *tout = (TTree*)f->Get("tout");

  vector<double> *branch = new vector<double>;

  tout->SetBranchAddress(DataBranchName, &branch);

  tout->GetEntry(2);
  int n = branch->size();
  
  f->Close();

  delete branch;

  return n;

}
