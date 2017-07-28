/*
 *  This method carries out the systematic study
 *
 *  by Sam de Jong
 */


void doSystematicStudy(vector<SystematicHolder> &Systematics, double PScaleHERErr, double PScaleLERErr){

  TString HERfile = inputs.getStringValue("HERfile");
  TString LERfile = inputs.getStringValue("LERfile");

  int nC = Params.getNChannels();
  TString DataBranchName = Params.getDataBranchName();

  //get data
  dataReader LER(LERfile, DataBranchName, "LER", nC);
  dataReader HER(HERfile, DataBranchName, "HER", nC);
  LER.readData();
  HER.readData();




  //loop over three systematics: current, beam size, and PScale
  for(int q=0; q<3; q++){
    
    
    if(q==0){ //perturb beam current up, then do the simulation
      Params.setCurrentPerturbation(0.03);  //perturbation
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", true);  //simulation
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", true);

    }else if(q==1){//perturb beam size up
       Params.setBeamSizePerturbation(0.0137);   
       Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", true);
       Params.setBeamSizePerturbation(0.0371);    
       Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", true);

    }else if(q==2){//perturb pscale up
      Params.setPScalePerturbation(PScaleLERErr);
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", true);
      Params.setPScalePerturbation(PScaleHERErr);
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", true);
    }
  
    
    //get the simulated data
    dataReader LERSystematic("rootFiles/LERtemp.root", DataBranchName, "LER", nC);
    dataReader HERSystematic("rootFiles/HERtemp.root", DataBranchName, "HER", nC);
    LERSystematic.readData();
    HERSystematic.readData();
    
    //get the data/sim ratio
    vector<dataSimRatio> ratios;
    ratios.resize(nC);
    
    for(int i=0; i<nC; i++){
      if(badCh[i]) continue;    
      ratios[i].addRatios(LER.getGraph(i), LERSystematic.getGraph(i), "LER");
      ratios[i].addRatios(HER.getGraph(i), HERSystematic.getGraph(i), "HER");
    }
    
    //delete the temporary files
    gROOT->ProcessLine(".! rm rootFiles/LERtemp.root");
    gROOT->ProcessLine(".! rm rootFiles/HERtemp.root");
    

    //add the systematics to the SystematicHolder, then perform the down perturbation
    if(q==0){
      for(int i=0; i<nC; i++){
	if(badCh[i]) continue;     
	Systematics[i].addSystematicDown("Current", ratios[i]);
      }
      Params.setCurrentPerturbation(0.03);  //3mA
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", false);
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", false);
    }else if(q==1){
      for(int i=0; i<nC; i++){
	if(badCh[i]) continue;  
	Systematics[i].addSystematicDown("Beam Size", ratios[i]);
      }
      Params.setBeamSizePerturbation(0.0137);   
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", false);
      Params.setBeamSizePerturbation(0.0371);    
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", false);
      
    }else if(q==2){
      for(int i=0; i<nC; i++){
	if(badCh[i]) continue;  
	Systematics[i].addSystematicDown("PScale", ratios[i]);
      }
      Params.setPScalePerturbation(PScaleLERErr);
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", false);
      Params.setPScalePerturbation(PScaleHERErr);
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", false);
    }

  
    
    dataReader LERSystematic2("rootFiles/LERtemp.root", DataBranchName, "LER", nC);
    dataReader HERSystematic2("rootFiles/HERtemp.root", DataBranchName, "HER", nC);
    LERSystematic2.readData();
    HERSystematic2.readData();
    

    ratios.clear();
    ratios.resize(nC);
    
    for(int i=0; i<nC; i++){
      if(badCh[i]) continue;          
      ratios[i].addRatios(LER.getGraph(i), LERSystematic.getGraph(i), "LER");
      ratios[i].addRatios(HER.getGraph(i), HERSystematic.getGraph(i), "HER");
    }
    
        
    //add the down systematic, then reset the perturbations.
    if(q==0){
      for(int i=0; i<nC; i++){
	if(badCh[i]) continue;     
	Systematics[i].addSystematicUp("Current", ratios[i]);
      }
      Params.setCurrentPerturbation(0);  //3mA
    } else if(q==1){
      for(int i=0; i<nC; i++){
	if(badCh[i]) continue; 
	Systematics[i].addSystematicUp("Beam Size", ratios[i]);
      }
      Params.setBeamSizePerturbation(0); 
    }else if(q==2){
      for(int i=0; i<nC; i++){
	if(badCh[i]) continue; 
	Systematics[i].addSystematicUp("PScale", ratios[i]);
      }
      Params.setPScalePerturbation(0);
    }

    //delete temporary files
    gROOT->ProcessLine(".! rm rootFiles/LERtemp.root");
    gROOT->ProcessLine(".! rm rootFiles/HERtemp.root");

  }


}
