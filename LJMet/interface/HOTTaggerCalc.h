#ifndef FWLJMET_LJMet_interface_HOTTaggerCalc_h
#define FWLJMET_LJMet_interface_HOTTaggerCalc_h

/*
  Calculator for the Run 2 inclusive TprimeTprime analysis
  Finds Tprime or Bprime particles and tags the decays chain so branching ratios can be scanned

  Author: Julie Hogan, 2015
*/


#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <string>
#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

#include "TLorentzVector.h" 
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfo.h"
#include "DataFormats/JetReco/interface/GenJet.h"

//#include "FWCore/Framework/interface/Frameworkfwd.h"
//#include "FWCore/Framework/interface/stream/EDProducer.h"
//#include "FWCore/Framework/interface/Event.h"
//#include "FWCore/Framework/interface/MakerMacros.h"

//#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "FWCore/Utilities/interface/StreamID.h"

//#include "FWCore/Utilities/interface/InputTag.h"
//#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Utilities/interface/Exception.h"

//#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
//#include "DataFormats/PatCandidates/interface/Jet.h"
//#include "DataFormats/PatCandidates/interface/Muon.h"
//#include "DataFormats/PatCandidates/interface/Electron.h"

//#include "FWCore/Framework/interface/EventSetup.h"
//#include "FWCore/Framework/interface/ESHandle.h"

#include "TopTagger/TopTagger/interface/TopTagger.h"
#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/TopTagger/interface/TopObject.h"
#include "TopTagger/TopTagger/interface/Constituent.h"
#include "TopTagger/TopTagger/interface/TopObjLite.h"

//this include is necessary to handle exceptions thrown by the top tagger code
#include "TopTagger/CfgParser/include/TTException.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"

using namespace std;

class LjmetFactory;


class HOTTaggerCalc : public BaseCalc{
  
public:
  
  HOTTaggerCalc(){};
  virtual ~HOTTaggerCalc(){}
  virtual int BeginJob(edm::ConsumesCollector && iC);
  virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
  virtual int EndJob(){return 0;}
  
  
private:

  double ak4ptCut_;
  std::string qgTaggerKey_;
  std::string deepCSVBJetTags_;
  std::string bTagKeyString_;
  std::string taggerCfgFile_;
  double discriminatorCut_;
    
  TopTagger tt;
 
};



#endif