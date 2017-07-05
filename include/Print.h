
//prints fit results to console as well as to a latex file
void Print(vector<TouschekSolver> soln, TString BranchName, TString Ring, TString title){
   
  cout<<title<<endl;
  TString label = BranchName+"_"+Ring+"_"+title;
  //label.ReplaceAll("_","");
  label.ReplaceAll(" ","_");
 

  ofstream out;
  out.open("tex/"+BranchName+".tex", ios::app);  //append to file

  BranchName.ReplaceAll("_", " ");

  out<<"\\begin{frame}{"<<BranchName<<", "<<Ring<<", "<<title<<"}\n";
  out<<"\\label{"<<label<<"}\n";
  out<<"\\begin{table}[htb]\n";
  out<<"\\footnotesize\n";
  out<<"\\centering\n";
  out<<"\\begin{tabular}{ccccccc}\n";
  out<<"Channel&Beam Gas&$\\sigma_{\\mathrm{Beam~Gas}}$ &Touschek&$\\sigma_{\\mathrm{Touschek}}$&$\\chi^2$&ndf\\\\ \\hline  \n";


  cout<<right<<setw(8)<<"Channel";
  soln[0].PrintTitle(cout);
  
  for(int i=0; i<nC; i++){
    if(badCh[i]) continue;
    cout<<right<<setw(8)<<i<<soln[i]<<endl;
    
    out<<"\\hyperlink{"<<label<<"-"<<i<<"}{"<<i<<"}";
    
    out<<"&";
    soln[i].printSolnTex(out);
  }
  out<<"\\end{tabular}\n";
  out<<"\\caption{"<<BranchName<<", "<<Ring<<", "<<title<<"}\n";
  out<<"\\label{tab:"<<BranchName<<Ring<<title<<"}\n";
  out<<"\\end{table}\n";
  out<<"\\end{frame}\n";
  out<<endl<<endl;
   
  
  

}
