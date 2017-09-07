/*
 *  Perform the Touschek & beam gas fit to data. This fits the data branch to  R = A*I*P*Zeff^2+B*I^2/(sigma_y*NBunch)
 *  methods are described in detail in .cc file in src/
 *
 *  by Sam de Jong
 */


#include "TMatrixD.h"
#include "TString.h"
#include "TF2.h"
#include "TGraph2DErrors.h"
#include "TPaveText.h"
#include "TH1F.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGraphErrors.h"

using namespace std;

#ifndef TOUSCHEK_h
#define TOUSCHEK_h 1

class TouschekSolver{

 public:

  TouschekSolver(){}
  TouschekSolver(const TouschekSolver &obj);
  TouschekSolver(TString ring, TMatrixD y, TMatrixD err, TMatrixD x, bool isSim);

  //set method
  void setVariables(TString ring, TMatrixD y, TMatrixD err, TMatrixD x, bool isSim);
  
  //get methods
  TMatrixD getSoln(){return soln;}
  TMatrixD getAppliedSoln(){return appliedSoln;}
  TMatrixD getVariance() {return Var;}
  double getTousFitParameters(){return soln[1][0];}
  double getBGFitParameters(){return soln[0][0];}
  double getTousError(){return sqrt(Var[1][1]);}
  double getBGError(){return sqrt(Var[0][0]);}
  
  
  
  void draw(TGraphErrors *data, TH1F *Coulomb, TH1F *Touschek);
  void Solve(double tousParam);
  void calculateVariance();
  double chiSq();  
  int ndf();
  void printSoln(ostream & out) const;
  void PrintTitle(ostream & out) const;

  void printSolnTex(ostream & out) const;

  friend ostream& operator<<(ostream& os, const TouschekSolver& r);


 private:
  TMatrixD ydata;
  TMatrixD X;
  TMatrixD errors;
  TMatrixD appliedSoln;
  TMatrixD soln;
  TMatrixD Var;

  double chiSquare;
  double ndof;

  bool sim;
  TString Ring;

  TMatrixD MatrixMultiply(TMatrixD a, TMatrixD b){
    
    int r1=a.GetNrows();
    //int r2=b.GetNrows();
    int c1=a.GetNcols();
    int c2=b.GetNcols();
    
    TMatrixD mult(r1,c2);
    
    int i,j,k;
  
    /* Initializing elements of matrix mult to 0.*/
    for(i=0; i<r1; ++i){
      for(j=0; j<c2; ++j){
	mult[i][j]=0;
      }
    }
    /* Multiplying matrix a and b and storing in array mult. */
    for(i=0; i<r1; ++i){
      for(j=0; j<c2; ++j){
	for(k=0; k<c1; ++k){
	  mult[i][j]+=a[i][k]*b[k][j];
	}
      }
    }
    
    return mult;
  }
  
  
};

#endif


