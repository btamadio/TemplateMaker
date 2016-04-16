#include "TemplateMaker.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "TTree.h"
#include "TH2.h"
#include "TRandom3.h"

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
  m_dsid=0;
  m_lumi = 1;
  m_inputList = false;
  m_isMC = false; 
  m_useSumOfWeights = false;
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
  m_event = new TM::Event;
}
TemplateMaker::~TemplateMaker(){
  delete m_templateFile; m_templateFile=0;
}
void TemplateMaker::makeTemplates(){
  m_cutFlowHist = TH1F("h_cutflow","cutflow",10,0.5,10.5);
  m_cutFlowHist.Sumw2();
  TH1F h_jet_pt = TH1F("h_jet_pt","small-R jet pT",500,0,5000);
  TH1F h_HT = TH1F("h_HT","small-R H_{T}",500,0,5000);
  TH1F h_MJ = TH1F("h_MJ","MJ",200,0,2000);
  TH2F h_MJdEta = TH2F("h_MJdEta","MJ vs. #Delta#eta",300,0,3000,500,0,5);
  TH1F h_fatjet_pt = TH1F("h_fatjet_pt","fat jet pT",300,0,3000);
  cout<<"Making template file "<<m_templateFileName<<endl;
  string line;
  if(m_inputList){
    ifstream infile(m_bkgFileList);
    while(getline(infile,line)){
      istringstream iss(line);
      string s;
      if(!(iss>>s)){break;}
      m_inputFileNames.push_back(s);
      //    cout<<"Adding input file "<<s<<endl;
    }
  }
  else{ m_inputFileNames.push_back(m_bkgFileList); }
  cout<<"Total input files: "<<m_inputFileNames.size()<<endl;
  cout<<"Calculating denominators"<<endl;
  calcDenoms();
  cout<<"Looping over input files"<<endl;
  for( int fi = 0; fi < m_inputFileNames.size(); fi++){
    TFile *f = TFile::Open(m_inputFileNames.at(fi).c_str());
    m_tree = (TTree*)f->Get("outTree");
    int nEntries = m_tree->GetEntries();
    setupTree();
    for( int entry = 0; entry < nEntries; entry++){
      m_tree->GetEntry(entry);
      float w = 1;
      int denom = 1;
      if(m_isMC){ 
	if (m_dsid == 0){
	  m_dsid=m_event->mcChannelNumber;
	}
	int denom = m_initialEvents.at(m_dsid);
	if(m_useSumOfWeights){ denom = m_initialSumOfWeights.at(m_dsid); }
	w = m_event->weight*m_lumi/denom;
      }
      //all events
      m_cutFlowHist.Fill(1,w);
      //TODO: implement trigger
      m_cutFlowHist.Fill(2,w);
      //TODO: implement jet cleaning
      m_cutFlowHist.Fill(3,w);
      //Basic event selection
      float ht = 0;
      bool passLeadJet = false;
      int nBJet = 0;
      for( int i = 0 ; i < m_event->jet_pt->size(); i++){
	if( fabs(m_event->jet_eta->at(i)) < m_jetEtaCut ){
	  if( m_event->jet_pt->at(i) > m_jetPtCut ){
	    ht += m_event->jet_pt->at(i);
	    //	    if( m_event->jet_MV2c20_isFlt70->at(i) == 1){
	    //	      nBJet++;
	    //	    }
	    if( m_event->jet_pt->at(i) > m_leadJetPtCut ){
	      passLeadJet = true;
	    }
	  }
	}
      }
      if (ht < m_htCut){ continue; }
      m_cutFlowHist.Fill(4,w);
      if (!passLeadJet){ continue; }
      m_cutFlowHist.Fill(5,w);
      h_HT.Fill(ht,w);
      for( int i = 0 ; i < m_event->jet_pt->size(); i++){
	if( fabs(m_event->jet_eta->at(i)) < m_jetEtaCut ){
	  if( m_event->jet_pt->at(i) > m_jetPtCut ){
	    h_jet_pt.Fill(m_event->jet_pt->at(i),w);
	  }
	}
      }
      int nFatJets = 0;
      float dy = 0;
      vector<float> fatJetEta;
      //      vector<float> fatJetM;
      float mj=0;
      for( int i = 0 ; i < m_event->fatjet_pt->size(); i++){
	if( m_event->fatjet_pt->at(i) > m_fatJetPtCut && fabs(m_event->fatjet_eta->at(i)) < m_fatJetEtaCut){
	  nFatJets++;
	  if( i < 4 ){ mj+=m_event->fatjet_m->at(i); }
	  fatJetEta.push_back(m_event->fatjet_eta->at(i));

	}
      }
      if (nFatJets < 5){ continue; }
      m_cutFlowHist.Fill(6,w);
      //TRandom3 r(0);
      //if(r.Rndm() > 0.83){continue;}
      //if ( nBJet == 0 ){ continue; }
      m_cutFlowHist.Fill(7,w);
      dy = fabs(fatJetEta.at(0)-fatJetEta.at(1));
      h_MJ.Fill(mj,w);
      h_MJdEta.Fill(mj,dy,w);
      if( mj < 800 ){continue;}
      m_cutFlowHist.Fill(8,w);
      for(int i = 0 ; i < m_event->fatjet_pt->size(); i++){
	if( m_event->fatjet_pt->at(i) > m_fatJetPtCut && fabs(m_event->fatjet_eta->at(i)) < m_fatJetEtaCut){
	  h_fatjet_pt.Fill(m_event->fatjet_pt->at(i),w);
	}
      }
      if( dy > 0.7 ){ continue; }
      m_cutFlowHist.Fill(9,w);
    }
  }
  m_templateFile = TFile::Open(m_templateFileName.c_str(),"RECREATE");
  m_cutFlowHist.Write();
  h_jet_pt.Write();
  h_HT.Write();
  h_MJ.Write();
  h_fatjet_pt.Write();
  h_MJdEta.Write();
}
void TemplateMaker::setupTree(){
  m_tree->SetBranchStatus("*",0);
  if(m_isMC){
    m_tree->SetBranchStatus("mcChannelNumber",1);
    m_tree->SetBranchStatus("mcEventWeight",1);
    m_tree->SetBranchAddress("mcChannelNumber",&m_event->mcChannelNumber);
    m_tree->SetBranchAddress("mcEventWeight",&m_event->mcEventWeight);
  }
  m_tree->SetBranchStatus("weight",1);
  m_tree->SetBranchStatus("jet_pt",1);
  m_tree->SetBranchStatus("jet_eta",1);
  m_tree->SetBranchStatus("jet_phi",1);
  m_tree->SetBranchStatus("jet_E",1);
  m_tree->SetBranchStatus("fatjet_pt",1);
  m_tree->SetBranchStatus("fatjet_eta",1);
  m_tree->SetBranchStatus("fatjet_phi",1);
  m_tree->SetBranchStatus("fatjet_m",1);
  //  m_tree->SetBranchStatus("jet_MV2c20_isFlt70",1);
  m_tree->SetBranchAddress("weight",&m_event->weight);
  m_tree->SetBranchAddress("jet_pt",&m_event->jet_pt);
  m_tree->SetBranchAddress("jet_eta",&m_event->jet_eta);
  m_tree->SetBranchAddress("jet_phi",&m_event->jet_phi);
  m_tree->SetBranchAddress("jet_E",&m_event->jet_E);
  m_tree->SetBranchAddress("fatjet_pt",&m_event->fatjet_pt);
  m_tree->SetBranchAddress("fatjet_eta",&m_event->fatjet_eta);
  m_tree->SetBranchAddress("fatjet_phi",&m_event->fatjet_phi);
  m_tree->SetBranchAddress("fatjet_m",&m_event->fatjet_m);
  //  m_tree->SetBranchAddress("jet_MV2c20_isFlt70",&m_event->jet_MV2c20_isFlt70);
}
void TemplateMaker::calcDenoms(){
  for( int fi = 0; fi < m_inputFileNames.size(); fi++){
    TFile *f = TFile::Open(m_inputFileNames.at(fi).c_str());
    m_tree = (TTree*)f->Get("outTree");
    int nEntries = m_tree->GetEntries();
    setupTree();
    m_tree->GetEntry(0);
    int key = 0;
    if(m_isMC){ key = m_event->mcChannelNumber; }
    else{ key = m_event->runNumber; }
    if(key==0 && m_dsid!=0){
      key=m_dsid;
    }
    string cutFlowHistName = "MetaData_EventCount_"+to_string(key);
    if(!f->GetListOfKeys()->Contains(cutFlowHistName.c_str())){
      cout<<"Couldn't find cutflow histogram. Exiting."<<endl;
      cout<<cutFlowHistName<<endl;
      exit(EXIT_FAILURE);
    }
    TH1F *cutFlowHist = (TH1F*)f->Get(cutFlowHistName.c_str());
    if( m_initialSumOfWeights.find(key) == m_initialSumOfWeights.end()){
      m_initialSumOfWeights[key] = cutFlowHist->GetBinContent(3);
      m_initialEvents[key] = cutFlowHist->GetBinContent(1);
    }
    else{
      m_initialSumOfWeights.at(key) += cutFlowHist->GetBinContent(3);
      m_initialEvents.at(key) += cutFlowHist->GetBinContent(1);
    }
  }
  int totalsumofweights = 0;
  for( auto const &ent : m_initialSumOfWeights ){
    totalsumofweights+=ent.second;
  }
  int totalevents = 0;

  for( auto const &ent : m_initialEvents ){
    totalevents+=ent.second;
    cout<<ent.first<<"\t"<<ent.second<<endl;
  }
  cout<<"Total events = "<<totalevents<<endl;
  cout<<"Total sum of weights = "<<totalsumofweights<<endl;
}
