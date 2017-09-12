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
 
  
  }

  SystematicHolder(int nChannels){
    nCh=nChannels;
    
    LERup.resize(nCh);
    LERdown.resize(nCh);
    
    HERup.resize(nCh);
    HERdown.resize(nCh);


    ValueLER.resize(nCh);
    ValueHER.resize(nCh);
  
  }
  
  //print to console
  void printOn(ostream & out) const{
    
    out<<"------Systematic-Uncertainties------\n";
    for(int j=0; j<nCh; j++){
      if(badCh[j]) continue;
      if(badHER[j]&&badLER[j]) continue;
      out<<j<<endl;
      
      if(!badHER[j]){
	
    out<<"HER Value: "<<ValueHER[j]<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<HERup[j][i] <<right<<setw(14)<<HERdown[j][i]<<endl;
    out<<endl;


      }

      if(!badLER[j]){
	
    out<<"LER Value: "<<ValueLER[j]<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<LERup[j][i] <<right<<setw(14)<<LERdown[j][i]<<endl;
    out<<endl;



      }
    }

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

    for(int j=0; j<nCh; j++){
      if(badCh[j]) continue;
      if(badHER[j]&&badLER[j]) continue;
    out<<"<"<<DataBranchName<<" value=\""<<j<<"\">"<<endl;                      //the start tag is <DetectorBranchName>

    if(!badHER[j]){

    out<<"  <HER value=\""<<ValueHER[j]<<"\">"<<endl;         //start of HER background components
    out<<"    <PScaleVal value=\""<<PScaleH<<"\">"<<endl;
    out<<"      "<<PScaleHEr<<endl;
    out<<"    </PScaleVal>"<<endl;
    //out<<"    <Ratio value=\""<<ValueHER[j]<<"\">"<<endl; //HER touschek
    for(int i=0; i<(int)name.size(); i++){
      out<<"      <"<<name[i]<<">"<<endl;                    //systematic name

      out<<"        <up>"<<endl;                             //up error
      out<<"          "<<HERup[j][i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;                           //down error
      out<<"          "<<HERdown[j][i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    //out<<"    </Touschek>"<<endl;                            //end of HER touschek

    out<<"  </HER>"<<endl;                                   //end of HER

    }

    if(!badLER[j]){


    out<<"  <LER value=\""<<ValueLER[j]<<"\">"<<endl;                //start of LER
    out<<"    <PScaleVal value=\""<<PScaleL<<"\">"<<endl;
    out<<"      "<<PScaleLEr<<endl;
    out<<"    </PScaleVal>"<<endl;
    
    for(int i=0; i<(int)name.size(); i++){
    out<<"      <"<<name[i]<<">"<<endl;

      out<<"        <up>"<<endl;
      out<<"          "<<LERup[j][i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<LERdown[j][i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;

    }
    //out<<"    </Touschek>"<<endl;

    out<<"  </LER>"<<endl;                   //end of LER
    }

    out<<"</"<<DataBranchName<<">"<<endl;    //end of detector tag

 

    }
    out<<"</xml>"<<endl;
    out.close();

    for(int i=0; i<(int)name.size(); i++)
      name[i].ReplaceAll("_", " ");
  }

  
  //set the ratio values.
  void setData(dataSimRatio data){
    for(int j=0; j<nCh; j++){
      if(badCh[j]) continue;
      
      
      if(!badHER[j]){
	ValueHER[j] = data.getHER(j); 
      }
      
      if(!badLER[j]){
	ValueLER[j]=data.getLER(j);
      }
    }
    
    
    //add statistical uncertainty
    name.push_back("Statistical");
    for(int j=0; j<nCh; j++){
      if(badCh[j]) continue;
      if(!badLER[j]){
	
	LERup[j].push_back(data.getLERError(j));
	LERdown[j].push_back(-data.getLERError(j));
      }
      
      if(!badHER[j]){
	HERdown[j].push_back(-data.getHERError(j));
	HERup[j].push_back(data.getHERError(j));
      }
    }
    
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
	for(int j=0; j<nCh; j++){

	  LERup[j][i] = data.getLER(j)-ValueLER[j];
	  HERup[j][i] = data.getHER(j)-ValueHER[j];
	  
	}
      }
    }

    //if the systematic isn't found, add it to the vector
    if(!found){
      name.push_back(sysName);

      for(int j=0; j<nCh; j++){
      
	LERup[j].push_back(data.getLER(j)-ValueLER[j]);
	HERup[j].push_back(data.getHER(j)-ValueHER[j]);
	
	
	//add an entry to the down vector as well
	LERdown[j].push_back(0);
	HERdown[j].push_back(0);
	
      }
    }
  }

  
//add a down systematic error.
void addSystematicDown(TString sysName, dataSimRatio data){
    bool found=false;
    
    

    for(int i=0; i<(int)name.size(); i++){
      if(name[i]==sysName){
	found=true;
	for(int j=0; j<nCh; j++){
	  LERdown[j][i] = data.getLER(j)-ValueLER[j];
	  HERdown[j][i] = data.getHER(j)-ValueHER[j];
	  
	}
      }
    }

    if(!found){
      name.push_back(sysName);
      
      for(int j=0; j<nCh; j++){
	LERdown[j].push_back(data.getLER(j)-ValueLER[j]);
	HERdown[j].push_back(data.getHER(j)-ValueHER[j]);
	
	
	LERup[j].push_back(0);
	HERup[j].push_back(0);
	
      }
    }
  }




 private:

  //the data/sim ratios
  vector<double> ValueLER;
  vector<double> ValueHER;

  double PScaleH;
  double PScaleL;

  double PScaleHEr;
  double PScaleLEr;

  int nCh;

  vector<TString> name;  //systematuc names 

  //LER uncertainties
  vector< vector<double> > LERup;
  vector< vector<double> > LERdown;

  //HER uncertainties
  vector< vector<double> > HERup;
  vector< vector<double> > HERdown;


};

//overload of << operator
ostream& operator<<(ostream& os, const SystematicHolder& H)  {  
  //os << dt.mo << '/' << dt.da << '/' << dt.yr;  
  H.printOn(os);
  return os;  
}  

#endif
