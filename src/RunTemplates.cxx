#include <iostream>
#include <string>
#include "TemplateMaker.cxx"
using namespace std;
class TemplateMaker;
int main(int argc, char *argv[]){
  if (argc < 2){
    cout<<"Not enough command line arguments given. Exiting."<<endl;
    exit(EXIT_FAILURE);
  }
  else if (argc > 3){
    cout<<"Too many command line arguments given. Exiting."<<endl;
    exit(EXIT_FAILURE);
  }
  string sigFileName;
  if (argc == 2){ sigFileName = ""; }
  else{ sigFileName = argv[2]; }
  string bkgFileList = argv[1];
  TemplateMaker tm ( bkgFileList, sigFileName );
  tm.setTemplateFileName("test.root");
  tm.isMC();
  tm.makeTemplates();
}
