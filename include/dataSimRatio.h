/*
 *  This class contains and calculates the data/sim ratio for the analysis.
 *
 *  by Sam de Jong
 */


using namespace std;

class dataSimRatio{
  
 public:
  dataSimRatio(){}


  //get the ratio
  double getHER(){return HER;}
  double getLER(){return LER;}


  //calculate data/sim ratio for HER or LER
  void addRatios(TGraphErrors *data, TGraphErrors *sim, TString Ring){
    
    double ratio;
    
      
    double dataPt, simPt;
    double x;
    double dataSum=0;
    double simSum=0;
      
    for(int j=0; j<data->GetN(); j++){
      data->GetPoint(j, x, dataPt);
      sim->GetPoint(j, x, simPt);
      dataSum+=dataPt;
      simSum+=simPt;
    }
    
    ratio = dataSum/simSum;    

    

    
    if(Ring=="HER") HER=ratio;
    else if(Ring=="LER") LER=ratio;
    
    
  }

  //cout method
  void printOn(ostream & out) const{

    out<<HER<<"\t"<<LER<<endl;

  }

 private:
  double HER;
  double LER;
};

//overload of << operator
ostream& operator<<(ostream& os, const dataSimRatio& H)  {  
  H.printOn(os);
  return os;  
}  
