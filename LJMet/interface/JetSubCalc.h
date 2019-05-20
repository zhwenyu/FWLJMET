#ifndef FWLJMET_LJMet_interface_JetSubCalc_h
#define FWLJMET_LJMet_interface_JetSubCalc_h

/*
 Calculator for substructure variables

 Author: Joshua Swanson, 2014. Updated Julie Hogan, 2015.
 */

#include <iostream>
#include <limits>   // std::numeric_limits
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include "TLorentzVector.h"
#include "TFile.h"
#include "TF1.h"

#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PATObject.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "FWLJMET/LJMet/interface/JetMETCorrHelper.h"
#include "FWLJMET/LJMet/interface/BTagSFUtil.h"

using namespace std;

class LjmetFactory;

class JetSubCalc : public BaseCalc {

public:
    JetSubCalc(){};
    virtual ~JetSubCalc(){};
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob();

private:

  edm::EDGetTokenT<reco::GenParticleCollection>      genParticlesToken;
  edm::EDGetTokenT<double>                           rhoJetsToken;

  std::string bDiscriminant;
  std::string bbDiscriminant;
  std::string cDiscriminant;
  std::string udsgDiscriminant;
  double kappa;
  bool killHF;
  bool isMc;
  std::string puppiCorrPath;
  TRandom3 JERrand;
  TF1 *puppisd_corrGEN;
  TF1 *puppisd_corrRECO_cen;
  TF1 *puppisd_corrRECO_for;

  bool   JECup;
  bool   JECdown;
  bool   JERup;
  bool   JERdown;
  bool   doNewJEC;
  bool   doAllJetSyst;
  JetMETCorrHelper JetMETCorr;

  BTagSFUtil btagSfUtil;

};


#endif
