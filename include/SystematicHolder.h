/*
 *  This class holds the systematic errors for the analysis. It can print them to the console 
 *  using cout<<SystematicHolder. It can also print an xml file.
 *
 *  by Sam de Jong
 */


using namespace std;


#ifndef SYSTEMATIC_h
#define SYSTEMATIC_h 1

class SystematicHolder{

 public:
  SystematicHolder(){
   ValueHER=0;
   ValueLER=0;
  
  }
  
  //print to console
  void printOn(ostream & out) const{
    
    out<<"------Systematic-Uncertainties------\n";
    
    out<<"HER  Value: "<<ValueHER<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<HERup[i] <<right<<setw(14)<<HERdown[i]<<endl;
    out<<endl;



    out<<"LER  Value: "<<ValueLER<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<LERup[i] <<right<<setw(14)<<LERdown[i]<<endl;
    out<<endl;




  }

  

  //print to an xml file
  void xmlPrint(TString DataBranchName, TString filename) {

    TString exists=gSystem->GetFromPipe("test -f "+filename+"; echo $?");          //check if the file exists
    if(exists=="0") gROOT->ProcessLine(".! sed -i -e 's/<\\/xml>//g' "+filename);  //remove the </xml> tag at the end of the file.
    
    ofstream out;
    out.open(filename, ios::app);  //append to file
    if(exists!="0") out<<"<xml>"<<endl;   //if the file doesn't exist, add <xml> tag


    for(int i=0; i<(int)name.size(); i++)
      name[i].ReplaceAll(" ", "_");

    out<<"<"<<DataBranchName<<">"<<endl;                      //the start tag is <DetectorBranchName>

    out<<"  <HER value=\""<<ValueHER<<"\">"<<endl      ;                         //start of HER background 
    out<<"    <PScale value=\""<<PScaleH<<"\">"<<endl;
    out<<"      "<<PScaleHEr<<endl;
    out<<"    </PScale>"<<endl;
    out<<"    <Uncertainty>"<<endl;
    for(int i=0; i<(int)name.size(); i++){
      out<<"      <"<<name[i]<<">"<<endl;                    //systematic name

      out<<"        <up>"<<endl;                             //up error
      out<<"          "<<HERup[i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;                           //down error
      out<<"          "<<HERdown[i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }

    out<<"    </Uncertainty>"<<endl;
    out<<"  </HER>"<<endl;                                   //end of HER

    out<<"  <LER value=\""<<ValueLER<<"\">"<<endl;                                    //start of LER
    out<<"    <PScale value=\""<<PScaleL<<"\">"<<endl;
    out<<"      "<<PScaleLEr<<endl;
    out<<"    </PScale>"<<endl;

    out<<"    <Uncertainty>"<<endl;
    for(int i=0; i<(int)name.size(); i++){
    out<<"      <"<<name[i]<<">"<<endl;

      out<<"        <up>"<<endl;
      out<<"          "<<LERup[i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<LERdown[i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;

    }
    
    out<<"    </Uncertainty>"<<endl;
    out<<"  </LER>"<<endl;                   //end of LER

    out<<"</"<<DataBranchName<<">"<<endl;    //end of detector tag

    out<<"</xml>"<<endl;

    out.close();

    for(int i=0; i<(int)name.size(); i++)
      name[i].ReplaceAll("_", " ");
  }
  
  
  //set the ratio values.
  void setData(dataSimRatio data){
    
   ValueHER = data.getHER(); 
   ValueLER = data.getLER();

      
  }

  void setPScale(double PScaleHER, double PScaleLER){
    PScaleH=PScaleHER;
    PScaleL=PScaleLER;
  }

  void setPScaleError(double PScaleHER, double PScaleLER){
    PScaleHEr=PScaleHER;
    PScaleLEr=PScaleLER;
  }

  //add an up systematic error. 
  void addSystematicUp(TString sysName, dataSimRatio data){
    bool found=false;
    //search the name vector for the systematic
    for(int i=0; i<(int)name.size(); i++){
      if(name[i]==sysName){   
	found=true;
	LERup[i] = data.getLER()-ValueLER;
	HERup[i] = data.getHER()-ValueHER;
	
      }
    }

    //if the systematic isn't found, add it to the vector
    if(!found){
      name.push_back(sysName);
      
      LERup.push_back(data.getLER()-ValueLER);
      HERup.push_back(data.getHER()-ValueHER);
	

      //add an entry to the down vector as well
      LERdown.push_back(0);
      HERdown.push_back(0);
	
    }
  }

  
//add a down systematic error.
void addSystematicDown(TString sysName, dataSimRatio data){
    bool found=false;
  for(int i=0; i<(int)name.size(); i++){
      if(name[i]==sysName){
	found=true;
	LERdown[i] = data.getLER()-ValueLER;
	HERdown[i] = data.getHER()-ValueHER;
	
      }
    }
    if(!found){
      name.push_back(sysName);
      
      LERdown.push_back(data.getLER()-ValueLER);
      HERdown.push_back(data.getHER()-ValueHER);
	
      LERup.push_back(0);
      HERup.push_back(0);
	
    }
  }




 private:

  //the data/sim ratios
  double ValueLER;
  double ValueHER;

  double PScaleH;
  double PScaleL;

  double PScaleHEr;
  double PScaleLEr;

  vector<TString> name;  //systematuc names 

  //LER uncertainties
  vector<double> LERup;
  vector<double> LERdown;

  //HER uncertainties
  vector<double> HERup;
  vector<double> HERdown;


};

//overload of << operator
ostream& operator<<(ostream& os, const SystematicHolder& H)  {  
  //os << dt.mo << '/' << dt.da << '/' << dt.yr;  
  H.printOn(os);
  return os;  
}  

#endif
