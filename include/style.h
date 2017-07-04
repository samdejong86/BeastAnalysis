#include "TColor.h"
#include "TStyle.h"


#ifndef BELLE

TColor *CustomRed   = new TColor(1002, 210./256,  38./256,  48./256);
TColor *CustomGreen = new TColor(1004,  90./256, 143./256,  41./256);
TColor *CustomBlue  = new TColor(1003,  60./256, 125./256, 196./256);
TColor *CustomBlue2  = new TColor(1005,  60./256, 125./256, 196./256);
TColor *CustomGrey  = new TColor(1001,  43./256,  43./256,  43./256);

void thisStyle(){

  TStyle *WhiteBack = new TStyle("White", "White");
  int icol=0;
  WhiteBack->SetFrameFillColor(icol);
  WhiteBack->SetCanvasColor(icol);
  WhiteBack->SetFrameBorderMode(icol);
  WhiteBack->SetCanvasBorderMode(icol);
  WhiteBack->SetPadBorderMode(icol);
  WhiteBack->SetPadColor(icol);
  WhiteBack->SetStatColor(icol);

  //makes title centered, borderless, and transparent when saved
  WhiteBack->SetTitleX(0.3);
  WhiteBack->SetTitleW(0.4);
    
  TColor *col10 = gROOT->GetColor(10); col10->SetAlpha(0.0);
  WhiteBack->SetTitleFillColor(10);
  WhiteBack->SetTitleBorderSize(0);

  //WhiteBack->SetLabelFont(61);
  //WhiteBack->SetTitleFont(63);

  //hides stat box
  WhiteBack->SetOptStat(0);

  //adds ticks to the right and top
  WhiteBack->SetPadTickY(1);
  WhiteBack->SetPadTickX(1);
  WhiteBack->SetPalette(1);


  WhiteBack->SetPadGridX(true);
  WhiteBack->SetPadGridY(true);



  gROOT->SetStyle("White");
  gROOT->ForceStyle();

}

#else

TColor *BeastGray           =  new TColor(1001, 128./256, 128./256, 128./256);
TColor *BeastJava           =  new TColor(1003,  0.258824, 0.67451 , 0.698039); //coulomb
TColor *BeastEagleGreene    =  new TColor(1002,  0       , 0.282353, 0.333333); //HER
TColor *BeastBuddhaGold     =  new TColor(1004,  0.803922, 0.678431, 0.172549); //Touschek
TColor *BeastTomato         =  new TColor(1005,  0.85098 , 0.180392, 0.152941); //LER



void thisStyle(){
  TStyle *belle2Style = new TStyle("BELLE2","Belle2 style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  belle2Style->SetFrameBorderMode(icol);
  belle2Style->SetFrameFillColor(icol);
  belle2Style->SetCanvasBorderMode(icol);
  belle2Style->SetCanvasColor(icol);
  belle2Style->SetPadBorderMode(icol);
  belle2Style->SetPadColor(icol);
  belle2Style->SetStatColor(icol);
  //belle2Style->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  belle2Style->SetPaperSize(20,26);

  // set margin sizes
  belle2Style->SetPadTopMargin(0.05);
  belle2Style->SetPadRightMargin(0.12);
  belle2Style->SetPadBottomMargin(0.16);
  belle2Style->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  belle2Style->SetTitleXOffset(1.4);
  belle2Style->SetTitleYOffset(1.4);

 
  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  belle2Style->SetTextFont(font);

  belle2Style->SetTextSize(tsize);
  belle2Style->SetLabelFont(font,"x");
  belle2Style->SetTitleFont(font,"x");
  belle2Style->SetLabelFont(font,"y");
  belle2Style->SetTitleFont(font,"y");
  belle2Style->SetLabelFont(font,"z");
  belle2Style->SetTitleFont(font,"z");
  
  belle2Style->SetLabelSize(tsize,"x");
  belle2Style->SetTitleSize(tsize,"x");
  belle2Style->SetLabelSize(tsize,"y");
  belle2Style->SetTitleSize(tsize,"y");
  belle2Style->SetLabelSize(tsize,"z");
  belle2Style->SetTitleSize(tsize,"z");

  // use bold lines and markers
  belle2Style->SetMarkerStyle(20);
  belle2Style->SetMarkerSize(1.2);
  belle2Style->SetHistLineWidth(2.);
  belle2Style->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //belle2Style->SetErrorX(0.001);
  // get rid of error bar caps
  belle2Style->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  belle2Style->SetOptTitle(0);
  //belle2Style->SetOptStat(1111);
  belle2Style->SetOptStat(0);
  //belle2Style->SetOptFit(1111);
  belle2Style->SetOptFit(0);

  // put tick marks on top and RHS of plots
  belle2Style->SetPadTickX(1);
  belle2Style->SetPadTickY(1);

  //// Modifications for BEAST
  // Remove legend and statbox border if one chose to use them
  belle2Style->SetLegendBorderSize(0);
  belle2Style->SetStatBorderSize(0);

  //Define BEAST colour palette (6 colours + gray, colour-blind safe)
  // Credits Paul Tol:  https://personal.sron.nl/~pault/
  

  gROOT->SetStyle("BELLE2");
  gROOT->ForceStyle();


}

#endif

