/*
 *  This simple header reads the input file (BadChannel.list). It produces a vector with 64 entries
 *  An entry is 0 if it corresponds to a good channel and 1 if it corresponds to a bad channel.
 *  Note that there are more entries in the vector than channels for most detectors. Entries greater than
 *  the number of channels will be 0.
 *
 *   vector<int> badChannels = getBadChannels("Branch Name");
 *
 *  If the branch name is not in the input file, a vector of size 64 with all entries equal to 0 is returned.
 */

vector<int> getBadChannels(TString &BranchName){

  int nChannels=64;
  TString bChannelList;
  vector<int> badChannels;
  badChannels.resize(nChannels);
  
  //The CSI channels are made up of LYSO and CSI detectors
  if(BranchName.Contains("LYSO")){
    for(int i=0; i<(int)badChannels.size(); i++)
      if(i%3==2) 
	badChannels[i]=true;

    BranchName = "CSI_hitRate";
  }else if(BranchName.Contains("Doped")){
    for(int i=0; i<(int)badChannels.size(); i++)
      if(i%3==1) 
	badChannels[i]=true;
    
    BranchName = "CSI_hitRate";
  }
    


  ifstream InFile;
  InFile.open("data/BadChannel.list");
  while(!InFile.eof()){
    TString a;
    InFile>>a;
    if(a==BranchName){
      InFile>>bChannelList;
    }
  }
  

  int from=0;
  TString tok;

  while(bChannelList.Tokenize(tok, from, "[,]")){
    if(tok.IsDigit()){
      int index = tok.Atoi();
      badChannels[index]=true;
    }
  }

  return badChannels;

}

//Some detectors have forward and backward components. Seperate them
void forwardBackward(TString DataBranchName, TString &forwardOrBackward, vector<int> &badChannels){

  if(DataBranchName.Contains("HE3")){
    forwardOrBackward="";
    return;
  }
  if(forwardOrBackward=="") return;


  int num=64;
  if(DataBranchName.Contains("CSI")) num=9;
  if(DataBranchName.Contains("BGO")) num=4;

  
  bool forward=false;
  if(forwardOrBackward.Contains("for"))
    forward=true;

  for(int i=0; i<(int)badChannels.size(); i++){
    if(i>num&&forward) badChannels[i]=true;
    if(i<=num&&!forward) badChannels[i]=true;
  }

  if(!forwardOrBackward.Contains("_"))
    forwardOrBackward = "_"+forwardOrBackward;
  


}
