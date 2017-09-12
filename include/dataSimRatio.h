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

  //get the ratio
  double getHER(int c){return HER[c];}
  double getLER(int c){return LER[c];}

  //get location error
  double getHERError(int c){return HERErr[c];}
  double getLERError(int c){return LERErr[c];}


  //calculate data/sim ratio for HER or LER
  void addRatios(dataReader data, dataReader Sim, TString Ring, int nChannels){
    
    if(Ring=="LER"){
      LER.resize(nChannels);
      LERErr.resize(nChannels);
    }else if(Ring="HER"){
      HER.resize(nChannels);
      HERErr.resize(nChannels);
    }


    //calculate data/sim ratio for Touschek and beam-gas
    for(int i=0; i<nChannels; i++){
      if(badCh[i]) continue;
      if(Ring=="HER"&&badHER[i]) continue;
      if(Ring=="LER"&&badLER[i]) continue;

      
      TGraphErrors *d = data.getGraph(i);
      TGraphErrors *s = Sim.getGraph(i);

      double dataPt, simPt;
      double x;
      double dataSum=0;
      double simSum=0;
      
      double errSumData=0;
      double errSimSum=0;
      
      for(int j=0; j<d->GetN(); j++){
	d->GetPoint(j, x, dataPt);
	s->GetPoint(j, x, simPt);
	dataSum+=dataPt;
	simSum+=simPt;

	errSumData += pow(d->GetErrorY(j),2);
	errSimSum += pow(s->GetErrorY(j),2);

      }
      
      double ratio= dataSum/simSum;    

      double errorRatio = RatioError(dataSum, simSum, sqrt(errSumData), sqrt(errSimSum));
       
    
      if(Ring=="LER"){
	LER[i] = ratio;
	LERErr[i] = errorRatio;
      }else if(Ring="HER"){
	HER[i] = ratio;
	HERErr[i] = errorRatio;
      }
    }
  }

  //cout method
  void printOn(ostream & out) const{

    out<<"\n\n------------Results------------\n";
    for(int i=0; i<(int)HER.size(); i++){
      if(badCh[i]) continue;
      if(badHER[i]&&badLER[i]) continue;
      out<<"Channel "<<i<<endl;
      if(!badHER[i]){
      out<<"HER: \ndata/sim:  "<<HER[i]<<endl;
      }
      if(!badLER[i]){
      out<<"LER: \ndata/sim:  "<<LER[i]<<endl;
      }
    }
  }

 private:
  vector<double> HER;
  vector<double> LER;

  vector<double> HERErr;
  vector<double> LERErr;


};

//overload of << operator
ostream& operator<<(ostream& os, const dataSimRatio& H)  {  
  //os << dt.mo << '/' << dt.da << '/' << dt.yr;  
  H.printOn(os);
  return os;  
}  
