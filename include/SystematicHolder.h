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
    
    LERupTous.resize(nCh);
    LERdownTous.resize(nCh);
    LERupBG.resize(nCh);
    LERdownBG.resize(nCh);
    
    HERupTous.resize(nCh);
    HERdownTous.resize(nCh);
    HERupBG.resize(nCh);
    HERdownBG.resize(nCh);


    ValueTousLER.resize(nCh);
    ValueBGLER.resize(nCh);
    ValueTousHER.resize(nCh);
    ValueBGHER.resize(nCh);
  
  }
  
  //print to console
  void printOn(ostream & out) const{
    
    out<<"------Systematic-Uncertainties------\n";
    for(int j=0; j<nCh; j++){
      if(badCh[j]) continue;
      out<<j<<endl;
      

    out<<"HER Touschek. Value: "<<ValueTousHER[j]<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<HERupTous[j][i] <<right<<setw(14)<<HERdownTous[j][i]<<endl;
    out<<endl;


    out<<"HER Beam Gas. Value: "<<ValueBGHER[j]<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<HERupBG[j][i] <<right<<setw(14)<<HERdownBG[j][i]<<endl;
    out<<endl<<endl;


    out<<"LER Touschek. Value: "<<ValueTousLER[j]<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<LERupTous[j][i] <<right<<setw(14)<<LERdownTous[j][i]<<endl;
    out<<endl;


    out<<"LER Beam Gas. Value: "<<ValueBGLER[j]<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<LERupBG[j][i] <<right<<setw(14)<<LERdownBG[j][i]<<endl;
    out<<endl<<endl;
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

    out<<"<"<<DataBranchName<<" value=\""<<j<<"\">"<<endl;                      //the start tag is <DetectorBranchName>

    out<<"  <HER>"<<endl;                                     //start of HER background components
    out<<"    <PScale value=\""<<PScaleH<<"\">"<<endl;
    out<<"      "<<PScaleHEr<<endl;
    out<<"    </PScale>"<<endl;
    out<<"    <Touschek value=\""<<ValueTousHER[j]<<"\">"<<endl; //HER touschek
    for(int i=0; i<(int)name.size(); i++){
      out<<"      <"<<name[i]<<">"<<endl;                    //systematic name

      out<<"        <up>"<<endl;                             //up error
      out<<"          "<<HERupTous[j][i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;                           //down error
      out<<"          "<<HERdownTous[j][i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    out<<"    </Touschek>"<<endl;                            //end of HER touschek

    out<<"    <Beam_Gas value=\""<<ValueBGHER[j]<<"\">"<<endl;  //HER beam gas... (and so one)
    for(int i=0; i<(int)name.size(); i++){
     out<<"      <"<<name[i]<<">"<<endl; 

      out<<"        <up>"<<endl;
      out<<"          "<<HERupBG[j][i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<HERdownBG[j][i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    out<<"    </Beam_Gas>"<<endl;
    out<<"  </HER>"<<endl;                                   //end of HER

    out<<"  <LER>"<<endl;                                    //start of LER
    out<<"    <PScale value=\""<<PScaleL<<"\">"<<endl;
    out<<"      "<<PScaleLEr<<endl;
    out<<"    </PScale>"<<endl;
    out<<"    <Touschek value=\""<<ValueTousLER[j]<<"\">"<<endl;
    for(int i=0; i<(int)name.size(); i++){
    out<<"      <"<<name[i]<<">"<<endl;

      out<<"        <up>"<<endl;
      out<<"          "<<LERupTous[j][i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<LERdownTous[j][i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;

    }
    out<<"    </Touschek>"<<endl;

    out<<"    <Beam_Gas value=\""<<ValueBGLER[j]<<"\">"<<endl;
    for(int i=0; i<(int)name.size(); i++){
     out<<"      <"<<name[i]<<">"<<endl;

      out<<"        <up>"<<endl;
      out<<"          "<<LERupBG[j][i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<LERdownBG[j][i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    out<<"    </Beam_Gas>"<<endl;
    out<<"  </LER>"<<endl;                   //end of LER

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

      ValueTousHER[j] = data.getHERTouschek(j); 
      ValueBGHER[j] = data.getHERBeamGas(j);
      
      ValueTousLER[j]=data.getLERTouschek(j);
      ValueBGLER[j] = data.getLERBeamGas(j);
    }
    /*
   //add location uncertainty
   name.push_back("Location");
   LERupTous.push_back(data.getLERTouschekError());
   HERupTous.push_back(data.getHERTouschekError());
   LERdownBG.push_back(data.getLERBeamGasError());
   HERdownBG.push_back(data.getHERBeamGasError());

   LERdownTous.push_back(data.getLERTouschekError());
   HERdownTous.push_back(data.getHERTouschekError());
   LERupBG.push_back(data.getLERBeamGasError());
   HERupBG.push_back(data.getHERBeamGasError());
    */
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

	  LERupTous[j][i] = data.getLERTouschek(j)-ValueTousLER[j];
	  HERupTous[j][i] = data.getHERTouschek(j)-ValueTousHER[j];
	  
	  LERupBG[j][i] = data.getLERBeamGas(j)-ValueBGLER[j];
	  HERupBG[j][i] = data.getHERBeamGas(j)-ValueBGHER[j];
	}
      }
    }

    //if the systematic isn't found, add it to the vector
    if(!found){
      name.push_back(sysName);

      for(int j=0; j<nCh; j++){
      
	LERupTous[j].push_back(data.getLERTouschek(j)-ValueTousLER[j]);
	HERupTous[j].push_back(data.getHERTouschek(j)-ValueTousHER[j]);
	
	LERupBG[j].push_back(data.getLERBeamGas(j)-ValueBGLER[j]);
	HERupBG[j].push_back(data.getHERBeamGas(j)-ValueBGHER[j]);
	
	//add an entry to the down vector as well
	LERdownTous[j].push_back(0);
	HERdownTous[j].push_back(0);
	
	LERdownBG[j].push_back(0);
	HERdownBG[j].push_back(0);
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
	  LERdownTous[j][i] = data.getLERTouschek(j)-ValueTousLER[j];
	  HERdownTous[j][i] = data.getHERTouschek(j)-ValueTousHER[j];
	  
	  LERdownBG[j][i] = data.getLERBeamGas(j)-ValueBGLER[j];
	  HERdownBG[j][i] = data.getHERBeamGas(j)-ValueBGHER[j];
	}
      }
    }

    if(!found){
      name.push_back(sysName);
      
      for(int j=0; j<nCh; j++){
	LERdownTous[j].push_back(data.getLERTouschek(j)-ValueTousLER[j]);
	HERdownTous[j].push_back(data.getHERTouschek(j)-ValueTousHER[j]);
	
	LERdownBG[j].push_back(data.getLERBeamGas(j)-ValueBGLER[j]);
	HERdownBG[j].push_back(data.getHERBeamGas(j)-ValueBGHER[j]);
	
	LERupTous[j].push_back(0);
	HERupTous[j].push_back(0);
	
	LERupBG[j].push_back(0);
	HERupBG[j].push_back(0);
      }
    }
  }




 private:

  //the data/sim ratios
  vector<double> ValueTousLER;
  vector<double> ValueBGLER;
  vector<double> ValueTousHER;
  vector<double> ValueBGHER;

  double PScaleH;
  double PScaleL;

  double PScaleHEr;
  double PScaleLEr;

  int nCh;

  vector<TString> name;  //systematuc names 

  //LER uncertainties
  vector< vector<double> > LERupTous;
  vector< vector<double> > LERdownTous;
  vector< vector<double> > LERupBG;
  vector< vector<double> > LERdownBG;

  //HER uncertainties
  vector< vector<double> > HERupTous;
  vector< vector<double> > HERdownTous;
  vector< vector<double> > HERupBG;
  vector< vector<double> > HERdownBG;


};

//overload of << operator
ostream& operator<<(ostream& os, const SystematicHolder& H)  {  
  //os << dt.mo << '/' << dt.da << '/' << dt.yr;  
  H.printOn(os);
  return os;  
}  

#endif
