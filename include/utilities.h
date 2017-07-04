//get the number of channels in a branch
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
