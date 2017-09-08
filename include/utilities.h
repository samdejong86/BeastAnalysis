//get the number of channels in a branch
int getNumberOfChannels(TString DataBranchName, TString dataFile){
  
  TFile *f = new TFile(dataFile);
  TTree *tout = (TTree*)f->Get("tout");

  vector<double> *branch = new vector<double>;

  tout->SetBranchAddress(DataBranchName, &branch);

  int n=0;
  for(int i=0; i<100; i++){
     tout->GetEntry(i);
     if(n<branch->size()) n=branch->size();

  }

 
  
  f->Close();

  delete branch;
  //cout<<dataFile<<endl;
  //cout<<n<<endl;

  return n;

}
