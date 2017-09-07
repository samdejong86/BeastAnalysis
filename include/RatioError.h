/*
 *  calculates the error on a ratio. It seems to come up a lot...
 */

double RatioError(double numerator, double denominator, double numeratorError, double denominatorError){
  double errorRatio = pow(numeratorError,2)/pow(denominator,2) + pow(numerator/pow(denominator,2),2)*pow(denominatorError,2);
  return sqrt(errorRatio);
}
