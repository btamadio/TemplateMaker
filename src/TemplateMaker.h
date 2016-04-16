#ifndef TEMPLATEMAKER_H
#define TEMPLATEMAKER_H
#include <string>
#include <vector>
#include <map>
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
using namespace std;
namespace TM{
  struct Event{
    int runNumber;
    int eventNumber;
    int lumiBlock;
    int coreFlags;
    int mcEventNumber;
    int mcChannelNumber;
    float mcEventWeight;
    float weight;
    float weight_pileup;
    vector<string> passedTriggers;
    vector<float> *jet_pt;
    vector<float> *jet_eta;
    vector<float> *jet_phi;
    vector<float> *jet_E;
    vector<int> *jet_clean_passLooseBad;
    vector<int> *jet_clean_passLooseBadUgly;
    vector<int> *jet_clean_passTightBad;
    vector<int> *jet_clean_passTightBadUgly;
    vector<int> *jet_MV2c20_isFlt70;
    vector<float> *fatjet_pt;
    vector<float> *fatjet_eta;
    vector<float> *fatjet_phi;
    vector<float> *fatjet_m;
    Event(){}
  };
}
class TemplateMaker{
 public:
  TemplateMaker(std::string,std::string);
  ~TemplateMaker();
  int m_dsid;
  void makeTemplates();
  void sampleJetMass(string);
  void isMC() { m_isMC = true; }
  void useSumOfWeights (){ m_useSumOfWeights = true; }
  void usePileupReweighting (){ m_usePileupReweighting = true; }
  void setNSamplesPerJet(int nSamples){ m_nSamplesPerJet = nSamples; }
  void setNSamplesPerEvent(int nSamples){ m_nSamplesPerEvent = nSamples; }
  void setBinEdgesPt( vector<float> binEdges ){ m_binEdgesPt = binEdges; }
  void setBinEdgesEta( vector<float> binEdges ){ m_binEdgesEta = binEdges; }
  void setTriggerString( string s ){ m_triggerStr = s; }
  void setNbTags( int n ){ m_nbTags = n; }
  void setHtCut( float h ){ m_htCut = h; }
  void setFatJetPtCut( float pt ){ m_fatJetPtCut = pt; }
  void setFatJetEtaCut( float eta ){ m_fatJetPtCut = eta; }
  void setJetPtCut( float pt ){ m_jetPtCut = pt; }
  void setJetEtaCut( float eta ){ m_jetEtaCut = eta; }
  void setLeadJetPtCut( float pt ){ m_leadJetPtCut = pt; }
  void setTemplateNbTags( int n ){ m_templateNbTags = n; }
  void setTemplateFileName( string s ){ m_templateFileName = s; }
  void setJetCleanString( string s ){ m_jetCleanStr = s; }
  void setDeltaEtaCut( float e ){ m_deltaEtaCut = e; }
  void setLumi( float lumi ){ m_lumi = lumi*1E6; }
  void setDSID( int d ) {m_dsid = d; }
  void setupTree();
  void calcDenoms();
  void inputList(){ m_inputList = true; }
  
 private:
  float m_lumi;
  TH1F m_cutFlowHist;
  bool m_isMC;
  bool m_inputList;
  string m_bkgFileList;
  string m_sigFileName;
  bool m_injectSignal;
  bool m_useSumOfWeights; //for MC normalization, use sum of weights or number of events
  bool m_usePileupReweighting;
  int m_nSamplesPerJet; //Number of samples to do for individual jets
  int m_nSamplesPerEvent; //Number of samples to do for total MJ
  vector<float> m_binEdgesPt;
  vector<float> m_binEdgesEta;
  string m_triggerStr;
  int m_nbTags;
  float m_htCut;
  float m_leadJetPtCut;
  float m_fatJetPtCut;
  float m_fatJetEtaCut;
  float m_jetPtCut;
  float m_jetEtaCut;
  int m_templateNbTags;
  string m_templateFileName;
  TFile *m_templateFile;
  TTree *m_tree;
  vector<string> m_inputFileNames;
  map<int,int> m_initialSumOfWeights;
  map<int,int> m_initialEvents;
  string m_jetCleanStr;
  float m_deltaEtaCut;
  TM::Event *m_event;
};
#endif
