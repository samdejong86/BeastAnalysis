/* 
 *  A simple method which determines the weighting factors for the study
 * 
 *  by Sam de Jong
 */

double getWeights(vector<TouschekSolver> data, vector<TouschekSolver> Sim, double &error){
  
    double weightSum = 0;
  double weightDenom = 0;

    
  for(int i=0; i<(int)data.size(); i++){
    if(badCh[i]) continue;

    double dataBG = data[i].getBGFitParameters();
    double dataTous = data[i].getTousFitParameters();
       
    double simBG = Sim[i].getBGFitParameters();
    double simTous = Sim[i].getTousFitParameters();
      
    double dataBGError = data[i].getBGFitError();
    double dataTousError = data[i].getTousFitError();

    double simBGError = Sim[i].getBGFitError();
    double simTousError = Sim[i].getTousFitError();

    double errorTop =  dataBGError/pow(dataTous,2) + pow(dataBG/pow(dataTous,2),2)*dataTousError;
    double errorBottom = simBGError/pow(simTous,2) + pow(simBG/pow(simTous,2),2)*simTousError;




    double x = dataBG/dataTous;
    double y = simBG/simTous;


    double ratio = x/y;

    double errorRatio = errorTop/pow(y,2) + pow(x/pow(y,2),2)*errorBottom;

    weightSum += ratio/errorRatio;
    weightDenom += 1/errorRatio;
    



  }
  


  error = sqrt(1/weightDenom);
    
  
  return weightSum/weightDenom;


}

