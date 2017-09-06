/*
 *  This class contains and calculates the data/sim ratio for the analysis.
 *
 *  by Sam de Jong
 */


using namespace std;

class dataSimRatio{
  
 public:
  dataSimRatio(){
}

  //set the ratio
  void setHERTouschek(double num, int ch){HERTouschek[ch]=num;}
  void setHERBeamGas(double num, int ch){HERBeamGas[ch]=num;}
  void setLERTouschek(double num, int ch){LERTouschek[ch]=num;}
  void setLERBeamGas(double num, int ch){LERBeamGas[ch]=num;}

  //get the ratio
  double getHERTouschek(int c){return HERTouschek[c];}
  double getHERBeamGas(int c){return HERBeamGas[c];}
  double getLERTouschek(int c){return LERTouschek[c];}
  double getLERBeamGas(int c){return LERBeamGas[c];}

  //get location error
  double getHERTouschekError(){return HERTouschekErr;}
  double getHERBeamGasError(){return HERBeamGasErr;}
  double getLERTouschekError(){return LERTouschekErr;}
  double getLERBeamGasError(){return LERBeamGasErr;}


  //calculate data/sim ratio for HER or LER
  void addRatios(vector<TouschekSolver> data, vector<TouschekSolver> Sim, TString Ring){
    
    if(Ring=="LER"){
      LERTouschek.resize(data.size());
      LERBeamGas.resize(data.size());
    }else if(Ring="HER"){
      HERTouschek.resize(data.size());
      HERBeamGas.resize(data.size());
    }


    //calculate data/sim ratio for Touschek and beam-gas
    for(int i=0; i<(int)data.size(); i++){
      if(badCh[i]) continue;
      


      
      double dataBG = data[i].getBGFitParameters();
      double dataTous = data[i].getTousFitParameters();
      
      double simBG = Sim[i].getBGFitParameters();
      double simTous = Sim[i].getTousFitParameters();
       
      double Touschek = dataTous/simTous;
      double beamGas  = dataBG/simBG;
  
      
    
      if(Ring=="LER"){
	LERTouschek[i] = Touschek;
	LERBeamGas[i] = beamGas;
      }else if(Ring="HER"){
	HERTouschek[i] = Touschek;
	HERBeamGas[i] = beamGas;
      }
    }
  }

  //cout method
  void printOn(ostream & out) const{

    out<<"\n\n------------Results------------\n";
    for(int i=0; i<HERTouschek.size(); i++){
      if(badCh[i]) continue;
      out<<"Channel "<<i<<endl;
      out<<"HER: \ndata/sim for Touschek:  "<<HERTouschek[i]<<endl;
      out<<"data/sim for Beam Gas:  "<<HERBeamGas[i]<<endl<<endl;
      out<<"LER: \ndata/sim for Touschek:  "<<LERTouschek[i]<<endl;
      out<<"data/sim for Beam Gas:  "<<LERBeamGas[i]<<endl; 
    }
  }

 private:
  vector<double> HERTouschek;
  vector<double> LERTouschek;
  vector<double> HERBeamGas;
  vector<double> LERBeamGas;


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
