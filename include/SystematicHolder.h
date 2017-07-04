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

  void xmlPrint(TString DataBranchName, TString filename) {

    TString exists=gSystem->GetFromPipe("test -f "+filename+"; echo $?");
    if(exists=="0") gROOT->ProcessLine(".! sed -i -e 's/<\\/xml>//g' "+filename);
    
    ofstream out;
    out.open(filename, ios::app);
    if(exists!="0") out<<"<xml>"<<endl;


    for(int i=0; i<(int)name.size(); i++)
      name[i].ReplaceAll(" ", "_");

    out<<"<"<<DataBranchName<<">"<<endl;

    out<<"  <HER>"<<endl;
    out<<"    <Touschek value=\""<<ValueTousHER<<"\">"<<endl;
    for(int i=0; i<(int)name.size(); i++){
      out<<"      <"<<name[i]<<">"<<endl;

      out<<"        <up>"<<endl;
      out<<"          "<<HERupTous[i]<<endl;
      out<<"        </up>"<<endl;

      out<<"        <down>"<<endl;
      out<<"          "<<HERdownTous[i]<<endl;
      out<<"        </down>"<<endl;

      out<<"      </"<<name[i]<<">"<<endl;
    }
    out<<"    </Touschek>"<<endl;

    out<<"    <Beam_Gas value=\""<<ValueBGHER<<"\">"<<endl;
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
    out<<"  </HER>"<<endl;

    out<<"  <LER>"<<endl;

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
    out<<"  </LER>"<<endl;

    out<<"</"<<DataBranchName<<">"<<endl;

    out<<"</xml>"<<endl;

    out.close();

    for(int i=0; i<(int)name.size(); i++)
      name[i].ReplaceAll("_", " ");
  }

  
  void setData(dataSimRatio data){
    
   ValueTousHER = data.getHERTouschek(); 
   ValueBGHER = data.getHERBeamGas();

   ValueTousLER=data.getLERTouschek();
   ValueBGLER = data.getLERBeamGas();

   
   name.push_back("Location");
   LERupTous.push_back(data.getLERTouschekError());
   HERupTous.push_back(data.getHERTouschekError());

   LERdownTous.push_back(data.getLERTouschekError());
   HERdownTous.push_back(data.getHERTouschekError());

   LERupBG.push_back(data.getLERBeamGasError());
   HERupBG.push_back(data.getHERBeamGasError());
   LERdownBG.push_back(data.getLERBeamGasError());
   HERdownBG.push_back(data.getHERBeamGasError());
   

  }



  void addSystematicUp(TString sysName, dataSimRatio data){
    bool found=false;
    for(int i=0; i<(int)name.size(); i++){
      if(name[i]==sysName){
	found=true;
	LERupTous[i] = data.getLERTouschek()-ValueTousLER;
	HERupTous[i] = data.getHERTouschek()-ValueTousHER;
	
	LERupBG[i] = data.getLERBeamGas()-ValueBGLER;
	HERupBG[i] = data.getHERBeamGas()-ValueBGHER;
      }
    }

    if(!found){
      name.push_back(sysName);
      
      LERupTous.push_back(data.getLERTouschek()-ValueTousLER);
      HERupTous.push_back(data.getHERTouschek()-ValueTousHER);
	
      LERupBG.push_back(data.getLERBeamGas()-ValueBGLER);
      HERupBG.push_back(data.getHERBeamGas()-ValueBGHER);

      LERdownTous.push_back(0);
      HERdownTous.push_back(0);
	
      LERdownBG.push_back(0);
      HERdownBG.push_back(0);
    }
  }

  
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

  double ValueTousLER;
  double ValueBGLER;

  double ValueTousHER;
  double ValueBGHER;

  vector<TString> name;

  vector<double> LERupTous;
  vector<double> LERdownTous;
  vector<double> LERupBG;
  vector<double> LERdownBG;

  vector<double> HERupTous;
  vector<double> HERdownTous;
  vector<double> HERupBG;
  vector<double> HERdownBG;







};

ostream& operator<<(ostream& os, const SystematicHolder& H)  {  
  //os << dt.mo << '/' << dt.da << '/' << dt.yr;  
  H.printOn(os);
  return os;  
}  

#endif
