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
   ValueTousHER=0;
   ValueBGHER=0;

   ValueTousLER=0;
   ValueBGLER=0;
  
  }
  
  //print to console
  void printOn(ostream & out) const{
    
    out<<"------Systematic-Uncertainties------\n";
    
    out<<"HER Touschek. Value: "<<ValueTousHER<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<HERupTous[i] <<right<<setw(14)<<HERdownTous[i]<<endl;
    out<<endl;


    out<<"HER Beam Gas. Value: "<<ValueBGHER<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<HERupBG[i] <<right<<setw(14)<<HERdownBG[i]<<endl;
    out<<endl<<endl;


    out<<"LER Touschek. Value: "<<ValueTousLER<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<LERupTous[i] <<right<<setw(14)<<LERdownTous[i]<<endl;
    out<<endl;


    out<<"LER Beam Gas. Value: "<<ValueBGLER<<endl;
    out<<left<<setw(10)<<"Source"  <<right<<setw(12)<<"+error"<<right<<setw(14)<<"-error"<<endl;
    for(int i=0; i<(int)name.size(); i++)
      out<<left<<setw(10)<<name[i]<<right<<setw(12)<<LERupBG[i] <<right<<setw(14)<<LERdownBG[i]<<endl;
    out<<endl<<endl;


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

    out<<"  <HER>"<<endl;                                     //start of HER background components
    out<<"    <Touschek value=\""<<ValueTousHER<<"\">"<<endl; //HER touschek
    for(int i=0; i<(int)name.size(); i++){
      out<<"      <"<<name[i]<<">"<<endl;                    //systematic name

      out<<"        <up>"<<endl;                             //up error
      out<<"          "<<HERupTous[i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;                           //down error
      out<<"          "<<HERdownTous[i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    out<<"    </Touschek>"<<endl;                            //end of HER touschek

    out<<"    <Beam_Gas value=\""<<ValueBGHER<<"\">"<<endl;  //HER beam gas... (and so one)
    for(int i=0; i<(int)name.size(); i++){
     out<<"      <"<<name[i]<<">"<<endl; 

      out<<"        <up>"<<endl;
      out<<"          "<<HERupBG[i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<HERdownBG[i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    out<<"    </Beam_Gas>"<<endl;
    out<<"  </HER>"<<endl;                                   //end of HER

    out<<"  <LER>"<<endl;                                    //start of LER

    out<<"    <Touschek value=\""<<ValueTousLER<<"\">"<<endl;
    for(int i=0; i<(int)name.size(); i++){
    out<<"      <"<<name[i]<<">"<<endl;

      out<<"        <up>"<<endl;
      out<<"          "<<LERupTous[i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<LERdownTous[i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;

    }
    out<<"    </Touschek>"<<endl;

    out<<"    <Beam_Gas value=\""<<ValueBGLER<<"\">"<<endl;
    for(int i=0; i<(int)name.size(); i++){
     out<<"      <"<<name[i]<<">"<<endl;

      out<<"        <up>"<<endl;
      out<<"          "<<LERupBG[i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<LERdownBG[i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    out<<"    </Beam_Gas>"<<endl;
    out<<"  </LER>"<<endl;                   //end of LER

    out<<"</"<<DataBranchName<<">"<<endl;    //end of detector tag

    out<<"</xml>"<<endl;

    out.close();

    for(int i=0; i<(int)name.size(); i++)
      name[i].ReplaceAll("_", " ");
  }

  
  //set the ratio values.
  void setData(dataSimRatio data){
    
   ValueTousHER = data.getHERTouschek(); 
   ValueBGHER = data.getHERBeamGas();

   ValueTousLER=data.getLERTouschek();
   ValueBGLER = data.getLERBeamGas();

   
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
   

  }

  //add an up systematic error. 
  void addSystematicUp(TString sysName, dataSimRatio data){
    bool found=false;
    //search the name vector for the systematic
    for(int i=0; i<(int)name.size(); i++){
      if(name[i]==sysName){   
	found=true;
	LERupTous[i] = data.getLERTouschek()-ValueTousLER;
	HERupTous[i] = data.getHERTouschek()-ValueTousHER;
	
	LERupBG[i] = data.getLERBeamGas()-ValueBGLER;
	HERupBG[i] = data.getHERBeamGas()-ValueBGHER;
      }
    }

    //if the systematic isn't found, add it to the vector
    if(!found){
      name.push_back(sysName);
      
      LERupTous.push_back(data.getLERTouschek()-ValueTousLER);
      HERupTous.push_back(data.getHERTouschek()-ValueTousHER);
	
      LERupBG.push_back(data.getLERBeamGas()-ValueBGLER);
      HERupBG.push_back(data.getHERBeamGas()-ValueBGHER);

      //add an entry to the down vector as well
      LERdownTous.push_back(0);
      HERdownTous.push_back(0);
	
      LERdownBG.push_back(0);
      HERdownBG.push_back(0);
    }
  }

  
//add a down systematic error.
void addSystematicDown(TString sysName, dataSimRatio data){
    bool found=false;
    for(int i=0; i<(int)name.size(); i++){
      if(name[i]==sysName){
	found=true;
	LERdownTous[i] = data.getLERTouschek()-ValueTousLER;
	HERdownTous[i] = data.getHERTouschek()-ValueTousHER;
	
	LERdownBG[i] = data.getLERBeamGas()-ValueBGLER;
	HERdownBG[i] = data.getHERBeamGas()-ValueBGHER;
      }
    }

    if(!found){
      name.push_back(sysName);
      
      LERdownTous.push_back(data.getLERTouschek()-ValueTousLER);
      HERdownTous.push_back(data.getHERTouschek()-ValueTousHER);
	
      LERdownBG.push_back(data.getLERBeamGas()-ValueBGLER);
      HERdownBG.push_back(data.getHERBeamGas()-ValueBGHER);

      LERupTous.push_back(0);
      HERupTous.push_back(0);
	
      LERupBG.push_back(0);
      HERupBG.push_back(0);
    }
  }




 private:

  //the data/sim ratios
  double ValueTousLER;
  double ValueBGLER;
  double ValueTousHER;
  double ValueBGHER;

  vector<TString> name;  //systematuc names 

  //LER uncertainties
  vector<double> LERupTous;
  vector<double> LERdownTous;
  vector<double> LERupBG;
  vector<double> LERdownBG;

  //HER uncertainties
  vector<double> HERupTous;
  vector<double> HERdownTous;
  vector<double> HERupBG;
  vector<double> HERdownBG;


};

//overload of << operator
ostream& operator<<(ostream& os, const SystematicHolder& H)  {  
  //os << dt.mo << '/' << dt.da << '/' << dt.yr;  
  H.printOn(os);
  return os;  
}  

#endif
