#include "TemplateMaker.h"
#include <string>
#include <iostream>
using namespace std;
TemplateMaker::TemplateMaker(string bkgFileList,string sigFileName){
  cout<<"Creating TemplateMaker from background file list: "<<bkgFileList<<endl;
  if( sigFileName.empty() ){
    cout<<"No signal injection will be performed."<<endl;
  }
  else{
    cout<<"Signal will be injected from file "<<sigFileName<<endl;
  }
}
TemplateMaker::~TemplateMaker(){
}
