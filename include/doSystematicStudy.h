


void doSystematicStudy(vector<TouschekSolver> &solnHER, vector<TouschekSolver> &solnLER, SystematicHolder &Systematics, vector<TouschekSolver> &solnHER_sim, vector<TouschekSolver> &solnLER_sim, double PScaleHERErr, double PScaleLERErr){

  TString HERfile = "../../ntuples/v3/BEAST_run200789.root";
  TString LERfile = "../../ntuples/v3/BEAST_run300789.root";

  int nC = Params.getNChannels();
  TString DataBranchName = Params.getDataBranchName();


  for(int q=0; q<3; q++){

    if(q==0){
      Params.setCurrentPerturbation(0.03);  //0.03mA
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", true);
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", true);
    }else if(q==1){
       Params.setBeamSizePerturbation(0.0137);   
       Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", true);
       Params.setBeamSizePerturbation(0.0371);    
       Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", true);
    }else if(q==2){
      Params.setPScalePerturbation(PScaleLERErr);
      Params.PerturbedSimulate(LERfile, "rootFiles/LERtemp.root", true);
      Params.setPScalePerturbation(PScaleHERErr);
      Params.PerturbedSimulate(HERfile, "rootFiles/HERtemp.root", true);
    }
  
    
    
    dataReader LERSystematic("rootFiles/LERtemp.root", DataBranchName, "LER", nC);
    dataReader HERSystematic("rootFiles/HERtemp.root", DataBranchName, "HER", nC);
    LERSystematic.readData();
    HERSystematic.readData();
    
    vector<TouschekSolver> solnLERSystematic;
    vector<TouschekSolver> solnHERSystematic;
    solnLERSystematic.resize(nC);
    solnHERSystematic.resize(nC);
    

    for(int i=0; i<nC; i++){
      if(badCh[i]) continue;   
      
      
      solnLERSystematic[i].setVariables("LER", LERSystematic.getYData(i), LERSystematic.getErrors(i), LERSystematic.getX(), true);
      //solnLERSystematic[i].Solve(0);
 
      solnLERSystematic[i].Solve(solnLER_sim[i].getTousFitParameters());
      
      solnHERSystematic[i].setVariables("HER", HERSystematic.getYData(i), HERSystematic.getErrors(i), HERSystematic.getX(), true);
      //solnHERSystematic[i].Solve(0);

      solnHERSystematic[i].Solve(solnHER_sim[i].getTousFitParameters());
      

      
    }
    
    dataSimRatio ratios;

    ratios.addRatios(solnLER,solnLERSystematic, "LER");
    ratios.addRatios(solnHER,solnHERSystematic, "HER");
        
    

    gROOT->ProcessLine(".! rm rootFiles/LERtemp.root");
    gROOT->ProcessLine(".! rm rootFiles/HERtemp.root");
    

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
      //solnLERSystematic[i].Solve(0);
      
      
      solnHERSystematic[i].setVariables("HER", HERSystematic2.getYData(i), HERSystematic2.getErrors(i), HERSystematic2.getX(), true);
      //solnHERSystematic[i].Solve(0);
      solnHERSystematic[i].Solve(solnHER_sim[i].getTousFitParameters());
      
      
      
    }
    
    ratios.addRatios(solnLER,solnLERSystematic, "LER");
    ratios.addRatios(solnHER,solnHERSystematic, "HER");
        
    
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

    gROOT->ProcessLine(".! rm rootFiles/LERtemp.root");
    gROOT->ProcessLine(".! rm rootFiles/HERtemp.root");

  }





}
