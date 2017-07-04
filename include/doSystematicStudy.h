/*
 *  This method carries out the systematic study
 *
 *  by Sam de Jong
 */


void doSystematicStudy(vector<TouschekSolver> &solnHER, vector<TouschekSolver> &solnLER, SystematicHolder &Systematics, vector<TouschekSolver> &solnHER_sim, vector<TouschekSolver> &solnLER_sim, double PScaleHERErr, double PScaleLERErr){

  TString HERfile = "../../ntuples/v3/BEAST_run200789.root";
  TString LERfile = "../../ntuples/v3/BEAST_run300789.root";

  int nC = Params.getNChannels();
  TString DataBranchName = Params.getDataBranchName();


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
    
    vector<TouschekSolver> solnLERSystematic;
    vector<TouschekSolver> solnHERSystematic;
    solnLERSystematic.resize(nC);
    solnHERSystematic.resize(nC);
    

    //get the Touschek fit solution for each (good) channel
    for(int i=0; i<nC; i++){
      if(badCh[i]) continue;   
            
      solnLERSystematic[i].setVariables("LER", LERSystematic.getYData(i), LERSystematic.getErrors(i), LERSystematic.getX(), true); 
      solnLERSystematic[i].Solve(solnLER_sim[i].getTousFitParameters());
      
      solnHERSystematic[i].setVariables("HER", HERSystematic.getYData(i), HERSystematic.getErrors(i), HERSystematic.getX(), true);
      solnHERSystematic[i].Solve(solnHER_sim[i].getTousFitParameters());
            
    }
    
    //get the data/sim ratio
    dataSimRatio ratios;
    ratios.addRatios(solnLER,solnLERSystematic, "LER");
    ratios.addRatios(solnHER,solnHERSystematic, "HER");
        
    
    //delete the temporary files
    gROOT->ProcessLine(".! rm rootFiles/LERtemp.root");
    gROOT->ProcessLine(".! rm rootFiles/HERtemp.root");
    

    //add the systematics to the SystematicHolder, then perform the down perturbation
    if(q==0){
      Systematics.addSystematicDown("Current", ratios);
      Params.setCurrentPerturbation(0.03);  //3mA
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", false);
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", false);
    }else if(q==1){
      Systematics.addSystematicDown("Beam Size", ratios);
      Params.setBeamSizePerturbation(0.0137);   
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", false);
      Params.setBeamSizePerturbation(0.0371);    
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", false);
      
    }else if(q==2){
      Systematics.addSystematicDown("PScale", ratios);
      Params.setPScalePerturbation(PScaleLERErr);
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", false);
      Params.setPScalePerturbation(PScaleHERErr);
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", false);
    }

  
    
    dataReader LERSystematic2("rootFiles/LERtemp.root", DataBranchName, "LER", nC);
    dataReader HERSystematic2("rootFiles/HERtemp.root", DataBranchName, "HER", nC);
    LERSystematic2.readData();
    HERSystematic2.readData();
    


    solnLERSystematic.clear();
    solnHERSystematic.clear();
    solnLERSystematic.resize(nC);
    solnHERSystematic.resize(nC);


  
    for(int i=0; i<nC; i++){
      if(badCh[i]) continue;   
      
      solnLERSystematic[i].setVariables("LER", LERSystematic2.getYData(i), LERSystematic2.getErrors(i), LERSystematic2.getX(), true);
      solnLERSystematic[i].Solve(solnLER_sim[i].getTousFitParameters());
      
      solnHERSystematic[i].setVariables("HER", HERSystematic2.getYData(i), HERSystematic2.getErrors(i), HERSystematic2.getX(), true);
      solnHERSystematic[i].Solve(solnHER_sim[i].getTousFitParameters());
      
      
      
    }
    
    ratios.addRatios(solnLER,solnLERSystematic, "LER");
    ratios.addRatios(solnHER,solnHERSystematic, "HER");
        
    //add the down systematic, then reset the perturbations.
    if(q==0){
      Systematics.addSystematicUp("Current", ratios);
      Params.setCurrentPerturbation(0);  //3mA
    } else if(q==1){
      Systematics.addSystematicUp("Beam Size", ratios);
      Params.setBeamSizePerturbation(0); 
    }else if(q==2){
      Systematics.addSystematicUp("PScale", ratios);
      Params.setPScalePerturbation(0);
    }

    //delete temporary files
    gROOT->ProcessLine(".! rm rootFiles/LERtemp.root");
    gROOT->ProcessLine(".! rm rootFiles/HERtemp.root");

  }


}
