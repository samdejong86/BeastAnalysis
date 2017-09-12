/*
 *  This class contains the simulation. It get the simulation parameters from the truth branch
 *  of one of Igal's simulation ntuples, then scales them by the current, pressure, and beam size
 *  from the data.
 *
 *  by Sam de Jong
 *
 */

using namespace std;

#include "beamSim.h"  //header

//copy constructor
beamSim::beamSim(const beamSim &obj){
    
  this->filename = obj.filename;
  this->SimBranchName = obj.SimBranchName;
  this->nChannels = obj.nChannels;
  this->DataBranchName = obj.DataBranchName;
  
  
  this->currentPerturb = obj.currentPerturb;
  this->beamSizePerturb = obj.beamSizePerturb;
  this->PScalePerturb = obj.PScalePerturb;

  
  this->PressureScaleLER=obj.PressureScaleLER;
  this->PressureScaleHER=obj.PressureScaleHER;
  
    
  for(int i=0; i<nChannels; i++){
    this->ParamLT.push_back(obj.ParamLT.at(i));
    this->ParamHT.push_back(obj.ParamHT.at(i));
    this->Efficiencies.push_back(obj.Efficiencies.at(i));
    
    vector<double> pLBG = obj.ParamLBGall[i];
    vector<double> pHBG = obj.ParamHBGall[i];
    
    ParamLBGall.push_back(pLBG);
    ParamHBGall.push_back(pHBG);
    
    
  }        
}

//constructor containing reference filename, DataBranch name and number of channels.
beamSim::beamSim(TString file, TString DataBranch, int nCh){
  filename=file;
  DataBranchName=DataBranch;
  SimBranchName = getSimBranchName();
  nChannels = nCh;
  
  //resize vectors to the appropriate size.
  ParamLT.resize(nChannels);
  ParamHT.resize(nChannels);
  
  ParamLBGall.resize(nChannels);
  ParamHBGall.resize(nChannels);
  for(int i=0; i<nChannels; i++){
    ParamLBGall[i].resize(12);
    ParamHBGall[i].resize(12);
  } 
  PressureScaleLER=1;
  PressureScaleHER=1;
  
  Efficiencies.resize(nCh);
  for(int i=0; i<nCh; i++) Efficiencies[i]=1;
  
  if(DataBranch.Contains("BGO"))
    for(int i=0; i<nCh; i++) Efficiencies[i]=5.11e-5;
  /*if(DataBranch.Contains("HE3")){
    Efficiencies[0] = 0.278;
    Efficiencies[1] = 0.201;
    Efficiencies[2] = 0.154;
    Efficiencies[3] = 0.282;
    }*/
  currentPerturb=0;
  beamSizePerturb=0;
  PScalePerturb=0;

}

//deterimine what the simulation branch name is
TString beamSim::getSimBranchName(){    
  if(DataBranchName.Contains("HE3")){
    return "HE3_rate";
  }else if(DataBranchName.Contains("BGO")){
    return "BGO_dose";
  }else if(DataBranchName.Contains("CSI")){
    return "CSI_dose";
  }else if(DataBranchName.Contains("DIA")){
    return "DIA_dose";
  }else if(DataBranchName.Contains("CLW")){
    return "CLAWS_rate";
  }else if(DataBranchName.Contains("SCI_rate")){
    return "QCSS_rate";
  } else {
    cout<<"Making a guess\n";
      return DataBranchName;
  }
}

//get the SAD simulated data
void beamSim::getSimBeamPars(){
  TFile *f = new TFile(filename);  //open the reference file
  TTree *truth = (TTree*)f->Get("truth"); //get the truth branch
  
  //beam parameters from the simulation
  vector<double> *SAD_I_HER = new vector<double>;
  vector<double> *SAD_I_LER = new vector<double>;
  vector<double> *SAD_P_HER = new vector<double>;
  vector<double> *SAD_P_LER = new vector<double>;
  vector<double> *SAD_sigma_HER = new vector<double>;
  vector<double> *SAD_sigma_LER = new vector<double>;
  vector<double> *SAD_bunchNB_HER = new vector<double>;
  vector<double> *SAD_bunchNB_LER = new vector<double>;
  
  truth->SetBranchAddress("SAD_I_HER", &SAD_I_HER);
  truth->SetBranchAddress("SAD_I_LER", &SAD_I_LER);
  truth->SetBranchAddress("SAD_P_HER", &SAD_P_HER);
  truth->SetBranchAddress("SAD_P_LER", &SAD_P_LER);
  //truth->SetBranchAddress("SAD_sigma_HER", &SAD_sigma_HER);
  //truth->SetBranchAddress("SAD_sigma_LER", &SAD_sigma_LER);
  
  truth->SetBranchAddress("SAD_sigma_y_HER", &SAD_sigma_HER);
  truth->SetBranchAddress("SAD_sigma_y_LER", &SAD_sigma_LER);
  
  
  truth->SetBranchAddress("SAD_bunchNb_HER", &SAD_bunchNB_HER);
  truth->SetBranchAddress("SAD_bunchNb_LER", &SAD_bunchNB_LER);
  
  
  //rate from the different background components
  vector<double> *MC_LT_rate = new vector<double>;  //LT = LER Touschek
  vector<double> *MC_HT_rate = new vector<double>;  //HT = HER Touschek
  
  
  truth->SetBranchAddress("MC_LT_"+SimBranchName,   &MC_LT_rate);
  truth->SetBranchAddress("MC_HT_"+SimBranchName,   &MC_HT_rate);
  

  vector<double> *MC_LC_rate[12];  //LC = LER Coulomb
  vector<double> *MC_HC_rate[12];  //HC = HER Coulomb
  vector<double> *MC_LB_rate[12];  //LB = LER Brems
  vector<double> *MC_HB_rate[12];  //HB = HER Brems
  
  //get the beam gas gas rates from each of the 12 'D' sections
  stringstream ss;
  for(int i=0; i<12; i++){
    ss<<i;
    TString num=ss.str();
    ss.str("");
    
    MC_LC_rate[i] = new vector<double>();
    MC_HC_rate[i] = new vector<double>();
    MC_LB_rate[i] = new vector<double>();
    MC_HB_rate[i] = new vector<double>();
    truth->SetBranchAddress("MC_LC_"+SimBranchName+"_"+num, &MC_LC_rate[i]);
    truth->SetBranchAddress("MC_HC_"+SimBranchName+"_"+num, &MC_HC_rate[i]);
    truth->SetBranchAddress("MC_LB_"+SimBranchName+"_"+num, &MC_LB_rate[i]);
    truth->SetBranchAddress("MC_HB_"+SimBranchName+"_"+num, &MC_HB_rate[i]);
    
  }
  
  truth->GetEntry(0);
  double Zef=7;  //Zeff in the simulation is always 7.

  //for(int i=0; i<12; i++) cout<<MC_LC_rate[i]->size()<<endl;
 
  //cout<<"MC_HT_"+SimBranchName<<endl;

  for(int i=0; i<nChannels; i++){
  //for(int i=0; i<6; i++){
    ParamLT.at(i) = MC_LT_rate->at(i)*SAD_sigma_LER->at(0)*SAD_bunchNB_LER->at(0)/(SAD_I_LER->at(0)*SAD_I_LER->at(0));
    ParamHT.at(i) = MC_HT_rate->at(i)*SAD_sigma_HER->at(0)*SAD_bunchNB_HER->at(0)/(SAD_I_HER->at(0)*SAD_I_HER->at(0));;
    
    
    for(int j=0; j<12; j++){
      
      ParamLBGall.at(i)[j] = (MC_LC_rate[j]->at(i)+MC_LB_rate[j]->at(i))/(SAD_P_LER->at(0)*133.32e-9*SAD_I_LER->at(0)*Zef*Zef);
      ParamHBGall.at(i)[j] = (MC_HC_rate[j]->at(i)+MC_HB_rate[j]->at(i))/(SAD_P_HER->at(0)*133.32e-9*SAD_I_HER->at(0)*Zef*Zef);
      
    }
  }

  for(int i=0; i<nChannels; i++){
    cout<<ParamLT[i]<<"\t"<<ParamHT[i]<<endl;
  }
  
  //close the truth file.
  f->Close();
}


//perform the simulation
void beamSim::Simulate(TString inputFile, TString outfilename){

  //open the data file to get the beam parameters
  TFile *f = new TFile(inputFile);
  TTree *tout = (TTree*)f->Get("tout");

  UInt_t ts;
  vector<double> *preLER = new vector<double>;
  vector<double> *curLER = new vector<double>;
  vector<double> *bszLER = new vector<double>;
  vector<double> *nBnLER = new vector<double>;
  vector<double> *zefLER2 = new vector<double>;
  vector<double> *zefLER6 = new vector<double>;
  vector<int> *injLER = new vector<int>;
    
  vector<double> *preHER = new vector<double>;
  vector<double> *curHER = new vector<double>;
  vector<double> *bszHER = new vector<double>;
  vector<double> *presLER = new vector<double>;
  vector<double> *presHER = new vector<double>;
  vector<int> *injHER = new vector<int>;
  
  
  tout->SetBranchAddress("ts", &ts);
  tout->SetBranchAddress("SKB_LER_current", &curLER);
  tout->SetBranchAddress("SKB_LER_pressures", &presLER);
  tout->SetBranchAddress("SKB_LER_pressure_average",&preLER);
  tout->SetBranchAddress("SKB_LER_correctedBeamSize_xray_Y",&bszLER);
  tout->SetBranchAddress("SKB_LER_injectionNumberOfBunches",&nBnLER);
  tout->SetBranchAddress("SKB_LER_injectionFlag_safe", &injLER);
  tout->SetBranchAddress("SKB_LER_Zeff_D02",&zefLER2);
  tout->SetBranchAddress("SKB_LER_Zeff_D06",&zefLER6);
    
  tout->SetBranchAddress("SKB_HER_pressures", &presHER);
  tout->SetBranchAddress("SKB_HER_current", &curHER);
  tout->SetBranchAddress("SKB_HER_pressure_average",&preHER);
  tout->SetBranchAddress("SKB_HER_correctedBeamSize_xray_Y",&bszHER);
  tout->SetBranchAddress("SKB_HER_injectionFlag_safe", &injHER);
  
  double zefHER=1;
  double nBnHER=1576;
    
  //create a new ttree with the simulated data
  TFile *oFile = new TFile(outfilename, "RECREATE");
  TTree *outtree = new TTree("tout", "tout");
  
  vector<double> *simBranch = new vector<double>;
  
  outtree->Branch("ts", &ts, "ts/i");
  outtree->Branch("SKB_LER_current", "std::vector<double>", &curLER);
  outtree->Branch("SKB_LER_pressure_average", "std::vector<double>",&preLER);
  outtree->Branch("SKB_LER_correctedBeamSize_xray_Y", "std::vector<double>",&bszLER);
  outtree->Branch("SKB_LER_injectionNumberOfBunches", "std::vector<double>",&nBnLER);
  outtree->Branch("SKB_LER_Zeff_D02", "std::vector<double>",&zefLER2);
  outtree->Branch("SKB_LER_Zeff_D06", "std::vector<double>",&zefLER6);
  outtree->Branch("SKB_LER_injectionFlag_safe", "std::vector<int>", &injLER);
  outtree->Branch("SKB_LER_pressures", "vector<double>", presLER);
  outtree->Branch("SKB_HER_pressures", "vector<double>", presHER);
    
  outtree->Branch("SKB_HER_current", "std::vector<double>", &curHER);
  outtree->Branch("SKB_HER_pressure_average", "std::vector<double>",&preHER);
  outtree->Branch("SKB_HER_correctedBeamSize_xray_Y", "std::vector<double>",&bszHER);
  outtree->Branch("SKB_HER_injectionFlag_safe", "std::vector<int>", &injHER);
  
  outtree->Branch(DataBranchName, "std::vector<double>", &simBranch);  //the simulated data
    
  double ZeffLERother=2.7;
    
  //scale the simulation
  int nentries = tout->GetEntries();
  for(int i=0; i<nentries; i++){
    tout->GetEntry(i);
    
    simBranch->clear();
    
    double lBunch = nBnLER->at(0);
    if(lBunch==0) lBunch=1576;
    
    for(int j=0; j<nChannels; j++){
      
      double LBG = 0;
      double HBG = 0;
      
      double LERCUR = curLER->at(0);
      double HERCUR = curHER->at(0);
      
      //scale beam gas simulation: rate_sim = (P*I*Z^2)_data/(P*I*Z^2)_sim
      for(int k=0; k<12; k++){
	
	double ZL = ZeffLERother;
	if(k==1) ZL = zefLER2->at(0);
	if(k==5) ZL = zefLER6->at(0);
	
	//perform scaling on each 'D' section
	LBG += ParamLBGall.at(j)[k]*LERCUR*PressureScaleLER*presLER->at(k)*ZL*ZL;
	HBG += ParamHBGall.at(j)[k]*HERCUR*PressureScaleHER*presHER->at(k)*zefHER*zefHER;
      }
      
      
      double simRate = LBG+HBG+
	//scale Touschek simulation: rate_sim = (I^2/sigma_y*nBunch)_data/(I^2/sigma_y*nBunch)_sim
	(ParamLT.at(j)*LERCUR*LERCUR/(bszLER->at(0)*lBunch))+
	(ParamHT.at(j)*HERCUR*HERCUR/(bszHER->at(0)*nBnHER));
      
      simBranch->push_back(Efficiencies[j]*simRate); //combine with efficiencies
      
      }
    outtree->Fill();

       
  }
  
  
  
  outtree->Write();
  oFile->Close();
  f->Close();
  
  //delete pointers
  delete simBranch;
  delete preLER;
  delete curLER;
  delete bszLER;
  delete nBnLER;
  delete zefLER2;
  delete zefLER6;
  delete injLER;
    
  delete preHER;
  delete curHER;
  delete bszHER;
  delete presLER;
  delete presHER;
  delete injHER;
    
}



//perform perturbed simulation
void beamSim::PerturbedSimulate(TString inputFile, TString outfilename, bool positive){
  
  //if positive is true, the perturbation is in the +tive direction, otherwise perturbation is -tive.
  int direction=1;
  if(!positive) direction=-1;

  double currentScale = direction*currentPerturb;
  double beamsizeScale = 1+direction*beamSizePerturb;
  double pscaleScale = direction*PScalePerturb;

   
  TFile *f = new TFile(inputFile);
  TTree *tout = (TTree*)f->Get("tout");
  
  UInt_t ts;
  vector<double> *preLER = new vector<double>;
  vector<double> *curLER = new vector<double>;
  vector<double> *bszLER = new vector<double>;
  vector<double> *nBnLER = new vector<double>;
  vector<double> *zefLER2 = new vector<double>;
  vector<double> *zefLER6 = new vector<double>;
  vector<int> *injLER = new vector<int>;
    
    vector<double> *preHER = new vector<double>;
    vector<double> *curHER = new vector<double>;
    vector<double> *bszHER = new vector<double>;
    vector<double> *presLER = new vector<double>;
    vector<double> *presHER = new vector<double>;
    vector<int> *injHER = new vector<int>;
    

    tout->SetBranchAddress("ts", &ts);
    tout->SetBranchAddress("SKB_LER_current", &curLER);
    tout->SetBranchAddress("SKB_LER_pressures", &presLER);
    tout->SetBranchAddress("SKB_LER_pressure_average",&preLER);
    tout->SetBranchAddress("SKB_LER_correctedBeamSize_xray_Y",&bszLER);
    tout->SetBranchAddress("SKB_LER_injectionNumberOfBunches",&nBnLER);
    tout->SetBranchAddress("SKB_LER_injectionFlag_safe", &injLER);
    tout->SetBranchAddress("SKB_LER_Zeff_D02",&zefLER2);
    tout->SetBranchAddress("SKB_LER_Zeff_D06",&zefLER6);
    
    tout->SetBranchAddress("SKB_HER_pressures", &presHER);
    tout->SetBranchAddress("SKB_HER_current", &curHER);
    tout->SetBranchAddress("SKB_HER_pressure_average",&preHER);
    tout->SetBranchAddress("SKB_HER_correctedBeamSize_xray_Y",&bszHER);
    tout->SetBranchAddress("SKB_HER_injectionFlag_safe", &injHER);
  
    double zefHER=1;
    double nBnHER=1576;
    
    TFile *oFile = new TFile(outfilename, "RECREATE");
    TTree *outtree = new TTree("tout", "tout");
    
    vector<double> *simBranch = new vector<double>;
    
    outtree->Branch("ts", &ts, "ts/i");
    outtree->Branch("SKB_LER_current", "std::vector<double>", &curLER);
    outtree->Branch("SKB_LER_pressure_average", "std::vector<double>",&preLER);
    outtree->Branch("SKB_LER_correctedBeamSize_xray_Y", "std::vector<double>",&bszLER);
    outtree->Branch("SKB_LER_injectionNumberOfBunches", "std::vector<double>",&nBnLER);
    outtree->Branch("SKB_LER_Zeff_D02", "std::vector<double>",&zefLER2);
    outtree->Branch("SKB_LER_Zeff_D06", "std::vector<double>",&zefLER6);
    outtree->Branch("SKB_LER_injectionFlag_safe", "std::vector<int>", &injLER);
    outtree->Branch("SKB_LER_pressures", "vector<double>", presLER);
    outtree->Branch("SKB_HER_pressures", "vector<double>", presHER);
    
    outtree->Branch("SKB_HER_current", "std::vector<double>", &curHER);
    outtree->Branch("SKB_HER_pressure_average", "std::vector<double>",&preHER);
    outtree->Branch("SKB_HER_correctedBeamSize_xray_Y", "std::vector<double>",&bszHER);
    outtree->Branch("SKB_HER_injectionFlag_safe", "std::vector<int>", &injHER);
    
    outtree->Branch(DataBranchName, "std::vector<double>", &simBranch);
    
    double ZeffLERother=2.7;
    
    int nentries = tout->GetEntries();
    for(int i=0; i<nentries; i++){
      tout->GetEntry(i);
      
      simBranch->clear();
      
      double lBunch = nBnLER->at(0);
      if(lBunch==0) lBunch=1576;
      
      for(int j=0; j<nChannels; j++){
	
	double LBG = 0;
	double HBG = 0;
	
	double LERCUR = curLER->at(0)+currentScale;
	double HERCUR = curHER->at(0)+currentScale;
	
	for(int k=0; k<12; k++){
	  
	  double ZL = ZeffLERother;
	  if(k==1) ZL = zefLER2->at(0);
	  if(k==5) ZL = zefLER6->at(0);
	  
	  LBG += ParamLBGall.at(j)[k]*LERCUR*(PressureScaleLER+pscaleScale)*presLER->at(k)*ZL*ZL;
	  HBG += ParamHBGall.at(j)[k]*HERCUR*(PressureScaleHER+pscaleScale)*presHER->at(k)*zefHER*zefHER;
	}
	
	double simRate = LBG+HBG+
	  (ParamLT.at(j)*LERCUR*LERCUR/(bszLER->at(0)*beamsizeScale*lBunch))+
	  (ParamHT.at(j)*HERCUR*HERCUR/(bszHER->at(0)*beamsizeScale*nBnHER));
	
	simBranch->push_back(Efficiencies[j]*simRate);
	
      }
      outtree->Fill();
      
    }
    
    
    outtree->Write();
    oFile->Close();
    f->Close();
    
    delete simBranch;
    delete preLER;
    delete curLER;
    delete bszLER;
    delete nBnLER;
    delete zefLER2;
    delete zefLER6;
    delete injLER;
    
    delete preHER;
    delete curHER;
    delete bszHER;
    delete presLER;
    delete presHER;
    delete injHER;
    
}
