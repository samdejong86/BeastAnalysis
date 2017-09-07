/* 
 *  A simple method which determines the weighting factors for the study
 * 
 *  by Sam de Jong
 */


double getWeights(vector<TouschekSolver> data, vector<TouschekSolver> Sim, double &error, vector<int> badBeam){
  
  
  double WeightNum=0;
  double WeightDenom=0;

  int n=0;
  double meanNum = 0.0;
  double M2Num = 0.0;

  double meanDen = 0.0;
  double M2Den = 0.0;

  int ch=0;
    
  for(int i=0; i<(int)data.size(); i++){
    if(badCh[i]) continue;
    if(badBeam[i]) continue;
    ch=i;

    double dataBG = data[i].getBGFitParameters();
    double dataTous = data[i].getTousFitParameters();
       
    double simBG = Sim[i].getBGFitParameters();
    double simTous = Sim[i].getTousFitParameters();
      
    double x = dataBG/dataTous;
    double y = simBG/simTous;

    n+=1;
    double deltaNum = x-meanNum;
    meanNum += deltaNum/n;
    double delta2Num = x-meanNum;
    M2Num=deltaNum*delta2Num;

    double deltaDen = y-meanDen;
    meanDen += deltaDen/n;
    double delta2Den = y-meanDen;
    M2Den=deltaDen*delta2Den;

    WeightNum+=dataBG/dataTous;
    WeightDenom+=simBG/simTous;
  }
  
  

  double numErrorSq=M2Num/(n-1);
  double denErrorSq=M2Den/(n-1);


  if(n==1){
    
    numErrorSq = pow(RatioError(data[ch].getBGFitParameters(), data[ch].getTousFitParameters(), data[ch].getBGError(), data[ch].getTousError()),2);
    denErrorSq = pow(RatioError(Sim[ch].getBGFitParameters(), Sim[ch].getTousFitParameters(), Sim[ch].getBGError(), Sim[ch].getTousError()),2);

    
  }


  error = sqrt(numErrorSq/(WeightDenom*WeightDenom) + denErrorSq*WeightNum*WeightNum/( WeightDenom*WeightDenom*WeightDenom*WeightDenom));
    
  
  return WeightNum/WeightDenom;


}

