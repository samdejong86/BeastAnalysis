/*
 *   This class holds the data from a root file. 
 *
 * by Sam de Jong
 *
 */

using namespace std;

#include "dataReader.h"
#include "xmlParser.h"
//#include "BadChannel.h"

//copy constructor
dataReader::dataReader(const dataReader &obj){
  this->X.ResizeTo(obj.X.GetNrows(), obj.X.GetNcols());
  this->X = obj.X;

  this->inputFile = obj.inputFile;
  this->DataBranchName = obj.DataBranchName;
  this->nSubRun = obj.nSubRun;
  this->sim = obj.sim;
  this->Ring = obj.Ring;
  this->nChannels = obj.nChannels;
  
  this->BS.resize(obj.BS.size());
  this->n2.resize(obj.n2.size());
  
  for(int i=0; i<(int)obj.ydata.size(); i++)
    this->ydata.push_back(obj.ydata[i]);
  
  for(int i=0; i<(int)obj.errors.size(); i++)
    this->errors.push_back(obj.errors[i]);
  
  for(int i=0; i<(int)obj.BS.size(); i++) this->BS[i] = obj.BS[i];
  for(int i=0; i<(int)obj.n2.size(); i++) this->n2[i] = obj.n2[i];
  
}


//constructor that takes input file, data branch name, ring, and number of channels
dataReader::dataReader(TString inFile, TString DataBranch, TString ring, int nch){
  Ring = ring;
  inputFile=inFile;
  sim=inputFile.Contains("mc");
  DataBranchName=DataBranch;
  nChannels=nch;
  
  readRunTimes();
    
}


//plot the data vs bin
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

//get empty histograms (which are used by the TouschekSolver class). Label each bin with beamsize during that subrun
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
    h->GetYaxis()->SetTitle(displayname(DataBranchName));
    
    return h;

}

//read the start/stop times for each sum run
void dataReader::readRunTimes(){
    
    subrunStart.clear();
    subrunEnd.clear();

    XmlParser inputs("data/inputs.xml");
    
    string R="HER";
    if(Ring=="LER") R="LER";
    
    string startTag=R+"SubRunStartTimes";
    string endTag=R+"SubRunEndTimes";

    TString Start = inputs.getStringValue(startTag);
    TString End = inputs.getStringValue(endTag);

    int from=0;
    TString tok;
    
    while(Start.Tokenize(tok, from, "[,]")){
      double s = tok.Atoi();
      subrunStart.push_back(s);
    }
    from=0;
    while(End.Tokenize(tok, from, "[,]")){
      double e = tok.Atoi();
      subrunEnd.push_back(e);
    }
    
    nSubRun=subrunStart.size();

}

//read the data from the input file
void dataReader::readData(){
  readRunTimes();

  //open file, get ttree
  TFile *f = new TFile(inputFile);  
  TTree *tout = (TTree*)f->Get("tout");
  
  //initialize variables
  UInt_t ts;
  vector<double> *rate = new vector<double>();
  vector<double> *Current = new vector<double>();
  vector<double> *Pressure = new vector<double>();
  vector<double> *BeamSize = new vector<double>();
  vector<int> *injFlag = new vector<int>();
  vector<double> *nBunch = new vector<double>();
  vector<double> *Zeff = new vector<double>();
  
  //set branch addresses
  tout->SetBranchAddress("ts", &ts);
  tout->SetBranchAddress(DataBranchName, &rate);
  tout->SetBranchAddress("SKB_"+Ring+"_current", &Current);
  tout->SetBranchAddress("SKB_"+Ring+"_pressure_average", &Pressure);
  tout->SetBranchAddress("SKB_"+Ring+"_correctedBeamSize_xray_Y", &BeamSize);
  tout->SetBranchAddress("SKB_"+Ring+"_injectionFlag_safe", &injFlag);
  if(Ring=="LER"){
    tout->SetBranchAddress("SKB_LER_Zeff_D02", &Zeff);
    //Zeff->push_back(1);
    tout->SetBranchAddress("SKB_"+Ring+"_injectionNumberOfBunches", &nBunch);    
  } else {  //HER has no Zeff, and doesn't seem to have nbunches.
    Zeff->push_back(1);
    nBunch->push_back(1576);
  }
  
  //initialize arrays
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
    
  //initialize and resize vectors
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
  
  //loop over ttree
  int nentries = tout->GetEntries();
  for(int i=0; i<nentries; i++){
    tout->GetEntry(i);
    
    //data quality cuts
    if((int)rate->size()!=nChannels) continue;
    if(Pressure->size()!=1) continue;
    if(Current->size()!=1) continue;
    if(BeamSize->size()!=1) continue;
    if(injFlag->size()!=1) continue;
    
    if(Pressure->at(0)!=Pressure->at(0))continue;
    if(Current->at(0)!=Current->at(0))continue;
    if(BeamSize->at(0)!=BeamSize->at(0))continue;
    
    if(BeamSize->at(0)>200||BeamSize->at(0)<15) continue;
    
    if(injFlag->at(0)!=0) continue;  //skip injection time
    
    //loop over subruns, add data to appropriate entry of array/vector
    for(int j=0; j<nSubRun; j++){
      if(ts<subrunEnd[j]&&ts>subrunStart[j]){
	
	Cur[j] += Current->at(0);
	Pre[j] += Pressure->at(0);
	BS[j] += BeamSize->at(0);
	Zef[j] += Zeff->at(0);
	nBun[j] += nBunch->at(0);
	
	//calculate running stdev
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
  
  f->Close();  //close input file
    
  //clear and resized vectors of TMatrices
  ydata.clear();    
  errors.clear();
  
  ydata.resize(nChannels);    
  errors.resize(nChannels);
  
  //set TMatrix dimentions
  for(int i=0; i<nChannels; i++){
    ydata[i].ResizeTo(nSubRun, 1);
    errors[i] .ResizeTo(nSubRun,1);
  }
  
  
  X.ResizeTo(nSubRun,2);    
    
  //put data into TMatrices
  for(int i=0; i<nSubRun; i++){
    
    X[i][0] = (Cur[i]/n2[i])*(Pre[i]/n2[i])*(Zef[i]/n2[i])*(Zef[i]/n2[i]);  //beam gas component
    X[i][1] = (Cur[i]/n2[i])*(Cur[i]/n2[i])/(BS[i]/n2[i]*nBun[i]/n2[i]);    //Touschek component
    
    //detector observable and uncertainty
    for(int j=0; j<nChannels; j++){
      ydata[j][i][0] = Mean[i][j];
      errors[j][i][0]= sqrt(M2[i][j]/(n[i][j]-1))/sqrt(n[i][j]);
    }
    
  }

  //delete arrays
  for(int i=0; i<nSubRun; i++){
    delete[] Rate[i];
    delete[] Mean[i];
    delete[] n[i];
    delete[] M2[i];
  }
  
  delete[] Rate;
  delete[] Mean;
  delete[] n;
  delete[] M2;
    
  delete rate;
  delete Current;
  delete Pressure;
  delete BeamSize;
  delete injFlag;
  delete nBunch;
  delete Zeff;
}
