#include "TemplateMaker.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "TTree.h"

using namespace std;
TemplateMaker::TemplateMaker(string bkgFileList,string sigFileName){
  cout<<"Creating TemplateMaker from background file list: "<<bkgFileList<<endl;
  if( sigFileName.empty() ){
    cout<<"No signal injection will be performed."<<endl;
    m_injectSignal = false;
  }
  else{
    cout<<"Signal will be injected from file "<<sigFileName<<endl;
    m_injectSignal = true;
  }
  m_bkgFileList = bkgFileList;
  m_sigFileName = sigFileName;
  //a bunch of default values
  m_isMC = false; 
  m_useSumOfWeights = true;
  m_usePileupReweighting = true;
  m_nSamplesPerJet = 2500;
  m_nSamplesPerEvent = 250;
  m_triggerStr = "HTL_ht850";
  m_nbTags = 1;
  m_htCut = 1000;
  m_fatJetPtCut = 100;
  m_fatJetEtaCut = 2.5;
  m_jetPtCut = 50;
  m_jetEtaCut = 2.5;
  m_leadJetPtCut = 200;
  m_templateNbTags = 0;
  m_jetCleanStr = "LooseBad";
  m_deltaEtaCut = 0.7;
}
TemplateMaker::~TemplateMaker(){
  delete m_templateFile; m_templateFile=0;
}
void TemplateMaker::makeTemplates(){
  m_cutFlowHist = TH1F("h_cutflow","cutflow",11,0.5,11.5);
  TH1F h_jet_pt = TH1F("h_jet_pt","small-R jet pT",300,0,3000);
  TH1F h_fatjet_pt = TH1F("h_fatjet_pt","fat jet pT",300,0,3000);
  cout<<"Making template file "<<m_templateFileName<<endl;
  string line;
  ifstream infile(m_bkgFileList);
  while(getline(infile,line)){
    istringstream iss(line);
    string s;
    if(!(iss>>s)){break;}
    m_inputFileNames.push_back(s);
    //    cout<<"Adding input file "<<s<<endl;
  }
  cout<<"Total input files: "<<m_inputFileNames.size()<<endl;
  if(m_isMC){ calcDenoms(); }
  m_templateFile = TFile::Open(m_templateFileName.c_str(),"RECREATE");
  m_cutFlowHist.Write();
  h_jet_pt.Write();
  h_fatjet_pt.Write();
}
void TemplateMaker::setupTree(){
  m_tree->SetBranchStatus("*",0);
  m_tree->SetBranchStatus("mcChannelNumber",1);
  m_tree->SetBranchAddress("mcChannelNumber",&m_event.mcChannelNumber);
}
void TemplateMaker::calcDenoms(){
  for( int fi = 0; fi < m_inputFileNames.size(); fi++){
    TFile *f = TFile::Open(m_inputFileNames.at(fi).c_str());
    m_tree = (TTree*)f->Get("outTree");
    int nEntries = m_tree->GetEntries();
    setupTree();
    m_tree->GetEntry(0);
    string cutFlowHistName = "MetaData_EventCount_"+to_string(m_event.mcChannelNumber);
    TH1F *cutFlowHist = (TH1F*)f->Get(cutFlowHistName.c_str());
    int bin = 1;
    if ( m_useSumOfWeights ){ bin = 3; }
    if ( m_denoms.find(m_event.mcChannelNumber) == m_denoms.end() ){
      m_denoms[m_event.mcChannelNumber] = cutFlowHist->GetBinContent(bin);
    }
    else{
      m_denoms.at(m_event.mcChannelNumber) += cutFlowHist->GetBinContent(bin);
    }
  }
  for( auto const &ent : m_denoms ){
    cout<<ent.first<<"\t"<<ent.second<<endl;
  }
}
