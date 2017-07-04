/*
 *  This class contains and calculates the data/sim ratio for the analysis.
 *
 *  by Sam de Jong
 */


using namespace std;

class dataSimRatio{
  
 public:
  dataSimRatio(){
    HERTouschek=0;
    LERTouschek=0;
    HERBeamGas=0;
    LERBeamGas=0;
}

  //set the ratio
  void setHERTouschek(double num){HERTouschek=num;}
  void setHERBeamGas(double num){HERBeamGas=num;}
  void setLERTouschek(double num){LERTouschek=num;}
  void setLERBeamGas(double num){LERBeamGas=num;}

  //get the ratio
  double getHERTouschek(){return HERTouschek;}
  double getHERBeamGas(){return HERBeamGas;}
  double getLERTouschek(){return LERTouschek;}
  double getLERBeamGas(){return LERBeamGas;}

  //get location error
  double getHERTouschekError(){return HERTouschekErr;}
  double getHERBeamGasError(){return HERBeamGasErr;}
  double getLERTouschekError(){return LERTouschekErr;}
  double getLERBeamGasError(){return LERBeamGasErr;}


  //calculate data/sim ratio for HER or LER
  void addRatios(vector<TouschekSolver> data, vector<TouschekSolver> Sim, TString Ring){

    int n=0;
    double meanTou = 0.0;
    double M2Tou = 0.0;

    double meanBG = 0.0;
    double M2BG = 0.0;

    //calculate data/sim ratio for Touschek and beam-gas
    double beamGas=0;
    double Touschek=0;
    for(int i=0; i<(int)data.size(); i++){
      if(badCh[i]) continue;
      
      double dataBG = data[i].getBGFitParameters();
      double dataTous = data[i].getTousFitParameters();
      
      double simBG = Sim[i].getBGFitParameters();
      double simTous = Sim[i].getTousFitParameters();
      
      double x=dataTous/simTous;
      double y=dataBG/simBG;
      
      //running stdev calculation
      n+=1;
      double deltaTou = x-meanTou;
      meanTou += deltaTou/n;
      double delta2Tou = x-meanTou;
      M2Tou=deltaTou*delta2Tou;

      double deltaBG = y-meanBG;
      meanBG += deltaBG/n;
      double delta2BG = y-meanBG;
      M2BG=deltaBG*delta2BG;

      
      Touschek += dataTous/simTous;
      beamGas  += dataBG/simBG;
      
      
    }
    if(Ring=="LER"){
      LERTouschek = Touschek/n;
      LERBeamGas = beamGas/n;
      LERTouschekErr=sqrt(M2Tou/(n-1));
      LERBeamGasErr=sqrt(M2BG/(n-1));
    }else if(Ring="HER"){
      HERTouschek = Touschek/n;
      HERBeamGas = beamGas/n;
      HERTouschekErr=sqrt(M2Tou/(n-1));
      HERBeamGasErr=sqrt(M2BG/(n-1));
    }
 
  }

  //cout method
  void printOn(ostream & out) const{

    out<<"\n\n------------Results------------\n";
    out<<"HER: \ndata/sim for Touschek:  "<<HERTouschek<<endl;
    out<<"data/sim for Beam Gas:  "<<HERBeamGas<<endl<<endl;
    out<<"LER: \ndata/sim for Touschek:  "<<LERTouschek<<endl;
    out<<"data/sim for Beam Gas:  "<<LERBeamGas<<endl; 

  }

 private:
  double HERTouschek;
  double LERTouschek;
  double HERBeamGas;
  double LERBeamGas;


  double HERTouschekErr;
  double LERTouschekErr;
  double HERBeamGasErr;
  double LERBeamGasErr;

};

//overload of << operator
ostream& operator<<(ostream& os, const dataSimRatio& H)  {  
  //os << dt.mo << '/' << dt.da << '/' << dt.yr;  
  H.printOn(os);
  return os;  
}  
