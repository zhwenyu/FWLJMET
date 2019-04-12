
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

static int reg = LjmetFactory::GetInstance()->Register(new JetSubCalc(), "JetSubCalc");

struct sortclass {
  //  bool operator() (double i,double j) { return (i>j);}
  bool operator() (pat::Jet i, pat::Jet j){ return (i.pt() > j.pt());}
} ptsorter;



int JetSubCalc::BeginJob(edm::ConsumesCollector && iC)
{

  std::cout << "["+GetName()+"]: "<< "initializing parameters" << std::endl;

  isMc  = mPset.getParameter<bool>("isMc");

  genParticlesToken   = iC.consumes<reco::GenParticleCollection>(mPset.getParameter<edm::InputTag>("genParticles"));

  //HARDCODING !
  bDiscriminant    = "pfDeepCSVJetTags:probb";
  bbDiscriminant   = "pfDeepCSVJetTags:probbb";
  cDiscriminant    = "pfDeepCSVJetTags:probb";
  udsgDiscriminant = "pfDeepCSVJetTags:probudsg";
  std::cout << "["+GetName()+"]: "<< "Hardcoded btagging strings:" << std::endl;
  std::cout << "\t\t"<< "bDiscriminant    :" << bDiscriminant << std::endl;
  std::cout << "\t\t"<< "bbDiscriminant   :" << bbDiscriminant << std::endl;
  std::cout << "\t\t"<< "cDiscriminant    :" << cDiscriminant << std::endl;
  std::cout << "\t\t"<< "udsgDiscriminant :" << udsgDiscriminant << std::endl;

  kappa = mPset.getParameter<double>("kappa");
  killHF = mPset.getParameter<bool>("killHF");

  puppiCorrPath        = mPset.getParameter<edm::FileInPath>("puppiCorrPath").fullPath();
  TFile* file          = TFile::Open(puppiCorrPath.c_str(),"READ");
  puppisd_corrGEN      = (TF1*)file->Get("puppiJECcorr_gen");
  puppisd_corrRECO_cen = (TF1*)file->Get("puppiJECcorr_reco_0eta1v3");
  puppisd_corrRECO_for = (TF1*)file->Get("puppiJECcorr_reco_1v3eta2v5");
  file->Close();

  rhoJetsToken         = iC.consumes<double>(mPset.getParameter<edm::InputTag>("rhoJetsInputTag"));

  //JetMETCorr parameter initialization
  JECup                    = mPset.getParameter<bool>("JECup");
  JECdown                  = mPset.getParameter<bool>("JECdown");
  JERup                    = mPset.getParameter<bool>("JERup");
  JERdown                  = mPset.getParameter<bool>("JERdown");
  doNewJEC                 = mPset.getParameter<bool>("doNewJEC");
  doAllJetSyst             = mPset.getParameter<bool>("doAllJetSyst");
  JetMETCorr.Initialize(mPset); // REMINDER: THIS NEEDS --if(!isMc)JetMETCorr.SetFacJetCorr(event)-- somewhere correcting jets for data, since data JEC is era dependent. !!

  //BTAG parameter initialization
  btagSfUtil.Initialize(mPset);

  return 0;

}

int JetSubCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector)
{

    if(!isMc) JetMETCorr.SetFacJetCorr(event);

    // ----- Get AK4 jet objects from the selector -----
    // This is updated -- original version used all AK4 jets without selection
    std::vector<pat::Jet>                       const & theJets = selector->GetSelCorrJets();
    std::vector<pat::Jet>                       const & theAK8Jets = selector->GetSelCorrJetsAK8();

    double theJetHT = 0;

    // Available variables
    std::vector<double> theJetPt;
    std::vector<double> theJetEta;
    std::vector<double> theJetPhi;
    std::vector<double> theJetEnergy;
    std::vector<double> theJetCSV;

    // Discriminator for the MVA PileUp id.
    // NOTE: Training used is for ak5PFJetsCHS in CMSSW 5.3.X and Run 1 pileup
    std::vector<double> theJetPileupJetId;

    //Identity
    std::vector<int> theJetIndex;
    std::vector<int> theJetnDaughters;

    //Daughter four std::vector and index
    std::vector<double> theJetDaughterPt;
    std::vector<double> theJetDaughterEta;
    std::vector<double> theJetDaughterPhi;
    std::vector<double> theJetDaughterEnergy;

    std::vector <double> theJetCSVb;
    std::vector <double> theJetCSVbb;
    std::vector <double> theJetCSVc;
    std::vector <double> theJetCSVudsg;

    std::vector<int> theJetDaughterMotherIndex;
    std::vector<int> theJetCSVLSubJets;
    std::vector<int> theJetCSVMSubJets;
    std::vector<int> theJetCSVTSubJets;

    std::vector<int> theJetPFlav;
    std::vector<int> theJetHFlav;
    std::vector<int> theJetBTag;
    std::vector<int> theJetBTag_bSFup;
    std::vector<int> theJetBTag_bSFdn;
    std::vector<int> theJetBTag_lSFup;
    std::vector<int> theJetBTag_lSFdn;

    std::vector<int> maxProb;

    double thePileupJetId;

    for (std::vector<pat::Jet>::const_iterator ijet = theJets.begin(); ijet != theJets.end(); ijet++) {
      int index = (int)(ijet-theJets.begin());

      if(killHF && fabs(ijet->eta()) > 2.4) continue;

      thePileupJetId = -std::numeric_limits<double>::max();
      thePileupJetId = (double)ijet->userFloat("pileupJetId:fullDiscriminant");
      theJetPileupJetId.push_back(thePileupJetId);

      theJetPt     . push_back(ijet->pt());
      theJetEta    . push_back(ijet->eta());
      theJetPhi    . push_back(ijet->phi());
      theJetEnergy . push_back(ijet->energy());

      theJetCSVb.    push_back(ijet->bDiscriminator( bDiscriminant ));
      theJetCSVbb.   push_back(ijet->bDiscriminator( bbDiscriminant ));
      theJetCSVc.    push_back(ijet->bDiscriminator( cDiscriminant ));
      theJetCSVudsg. push_back(ijet->bDiscriminator( udsgDiscriminant ));

      TLorentzVector jetP4; jetP4.SetPtEtaPhiE(ijet->pt(), ijet->eta(), ijet->phi(), ijet->energy() );

      theJetBTag.push_back(btagSfUtil.isJetTagged(*ijet, jetP4, event, isMc, 0));
      theJetBTag_bSFup.push_back(btagSfUtil.isJetTagged(*ijet, jetP4, event, isMc, 1));
      theJetBTag_bSFdn.push_back(btagSfUtil.isJetTagged(*ijet, jetP4, event, isMc, 2));
      theJetBTag_lSFup.push_back(btagSfUtil.isJetTagged(*ijet, jetP4, event, isMc, 3));
      theJetBTag_lSFdn.push_back(btagSfUtil.isJetTagged(*ijet, jetP4, event, isMc, 4));
      theJetPFlav.push_back(abs(ijet->partonFlavour()));
      theJetHFlav.push_back(abs(ijet->hadronFlavour()));

      theJetIndex.push_back(index);
      theJetnDaughters.push_back((int)ijet->numberOfDaughters());

      //HT
      theJetHT += ijet->pt();

    }

    double leading_pt = -999.;
    for (std::vector<pat::Jet>::const_iterator ijet = theJets.begin(); ijet != theJets.end(); ijet++) {

      if(killHF && fabs(ijet->eta()) > 2.4) continue;
      if (ijet->pt() > leading_pt){leading_pt = ijet->pt();}
    }

    double second_leading_pt =-999.;
    for (std::vector<pat::Jet>::const_iterator ijet = theJets.begin(); ijet != theJets.end(); ijet++) {

      if(killHF && fabs(ijet->eta()) > 2.4) continue;
      if(ijet->pt() > second_leading_pt && ijet->pt() < leading_pt){
        second_leading_pt = ijet->pt();
      }
    }

    SetValue("theJetPt",     theJetPt);
    SetValue("theJetEta",    theJetEta);
    SetValue("theJetPhi",    theJetPhi);
    SetValue("theJetEnergy", theJetEnergy);

    SetValue("theJetDeepCSVb",    theJetCSVb);
    SetValue("theJetDeepCSVbb",   theJetCSVbb);
    SetValue("theJetDeepCSVc",    theJetCSVc);
    SetValue("theJetDeepCSVudsg", theJetCSVudsg);

    SetValue("theJetPFlav",  theJetPFlav);
    SetValue("theJetHFlav",  theJetHFlav);
    SetValue("theJetBTag",   theJetBTag);
    SetValue("theJetBTag_bSFup",   theJetBTag_bSFup);
    SetValue("theJetBTag_bSFdn",   theJetBTag_bSFdn);
    SetValue("theJetBTag_lSFup",   theJetBTag_lSFup);
    SetValue("theJetBTag_lSFdn",   theJetBTag_lSFdn);

    SetValue("theJetHT", theJetHT);
    SetValue("theJetLeadPt", leading_pt);
    SetValue("theJetSubLeadPt", second_leading_pt);

    SetValue("theJetPileupJetId", theJetPileupJetId);
    SetValue("theJetnDaughters", theJetnDaughters);

    // Load in AK8 jets (no selection performed on these)

    // Four std::vector
    std::vector<double> theJetAK8Pt;
    std::vector<double> theJetAK8Eta;
    std::vector<double> theJetAK8Phi;
    std::vector<double> theJetAK8Energy;
    std::vector<double> theJetAK8CSV;
    std::vector<double> theJetAK8DoubleB;
    std::vector<double> theJetAK8JetCharge;
    std::vector<double> theJetAK8GenPt;
    std::vector<double> theJetAK8GenDR;
    std::vector<double> theJetAK8GenMass;

    // CHS values
    std::vector<double> theJetAK8CHSPt;
    std::vector<double> theJetAK8CHSEta;
    std::vector<double> theJetAK8CHSPhi;
    std::vector<double> theJetAK8CHSMass;
    std::vector<double> theJetAK8CHSTau1;
    std::vector<double> theJetAK8CHSTau2;
    std::vector<double> theJetAK8CHSTau3;

    std::vector<double> theJetAK8SoftDropRaw;
    std::vector<double> theJetAK8SoftDropCorr;
    std::vector<double> theJetAK8CHSSoftDropMass;
    std::vector<double> theJetAK8CHSPrunedMass;

    std::vector<double> theJetAK8SoftDrop_JMSup;
    std::vector<double> theJetAK8SoftDrop_JMSdn;
    std::vector<double> theJetAK8SoftDrop_JMRup;
    std::vector<double> theJetAK8SoftDrop_JMRdn;

    // Pruned, trimmed and filtered masses available
    std::vector<double> theJetAK8SoftDrop;

    // n-subjettiness variables tau1, tau2, and tau3 available
    std::vector<double> theJetAK8NjettinessTau1;
    std::vector<double> theJetAK8NjettinessTau2;
    std::vector<double> theJetAK8NjettinessTau3;

    std::vector<double> theJetAK8Mass;
    std::vector<int>    theJetAK8Index;
    std::vector<int>    theJetAK8nDaughters;

    std::vector<double> theJetAK8SDSubjetPt;
    std::vector<double> theJetAK8SDSubjetEta;
    std::vector<double> theJetAK8SDSubjetPhi;
    std::vector<double> theJetAK8SDSubjetMass;
    std::vector<double> theJetAK8SDSubjetCSVb;
    std::vector<double> theJetAK8SDSubjetCSVc;
    std::vector<double> theJetAK8SDSubjetCSVudsg;
    std::vector<double> theJetAK8SDSubjetCSVbb;
    std::vector<int>    theJetAK8SDSubjetHFlav;
    std::vector<int>    theJetAK8SDSubjetBTag;
    std::vector<double> theJetAK8SDSubjetDR;
    std::vector<int> theJetAK8SDSubjetIndex;
    std::vector<int> theJetAK8SDSubjetSize;
    std::vector<int> theJetAK8SDSubjetNDeepCSVL;
    std::vector<int> theJetAK8SDSubjetNDeepCSVMSF;
    std::vector<int> theJetAK8SDSubjetNDeepCSVM_lSFup;
    std::vector<int> theJetAK8SDSubjetNDeepCSVM_lSFdn;
    std::vector<int> theJetAK8SDSubjetNDeepCSVM_bSFup;
    std::vector<int> theJetAK8SDSubjetNDeepCSVM_bSFdn;

    std::vector<double> theJetAK8SoftDropn2b1;
    std::vector<double> theJetAK8SoftDropn3b1;
    std::vector<double> theJetAK8SoftDropn2b2;
    std::vector<double> theJetAK8SoftDropn3b2;

    double jetCharge;
    double theSoftDrop;
    double theCHSPrunedMass,theCHSSoftDropMass;
    double theNjettinessTau1, theNjettinessTau2, theNjettinessTau3;
    double theCHSTau1, theCHSTau2, theCHSTau3;
    double theSoftDropn2b1, theSoftDropn3b1, theSoftDropn2b2, theSoftDropn3b2;

    double SDsubjetPt;
    double SDsubjetEta;
    double SDsubjetPhi;
    double SDsubjetMass;
    double SDsubjetDeepCSVb;
    double SDsubjetDeepCSVc;
    double SDsubjetDeepCSVudsg;
    double SDsubjetDeepCSVbb;
    int    SDsubjetHFlav;
    double SDsubjetBTag;
    double SDdeltaRsubjetJet;

    int nSDSubJets;
    int nSDSubsDeepCSVL;
    int nSDSubsDeepCSVMSF;
    int nSDSubsDeepCSVM_bSFup;
    int nSDSubsDeepCSVM_bSFdn;
    int nSDSubsDeepCSVM_lSFup;
    int nSDSubsDeepCSVM_lSFdn;
    int SDSubJetIndex;

    //for (std::vector<pat::Jet>::const_iterator ijet = theAK8Jets->begin(); ijet != theAK8Jets->end(); ijet++) {
    for (std::vector<pat::Jet>::const_iterator ii = theAK8Jets.begin(); ii != theAK8Jets.end(); ii++){
      int index = (int)(ii-theAK8Jets.begin());

      if (ii->pt() < 170) continue;

      pat::Jet corrak8;
      corrak8 = *ii;

      if(killHF && fabs(corrak8.eta()) > 2.4) continue;

      theJetAK8Pt    .push_back(corrak8.pt());
      theJetAK8Eta   .push_back(corrak8.eta());
      theJetAK8Phi   .push_back(corrak8.phi());
      theJetAK8Energy.push_back(corrak8.energy());
      theJetAK8Mass  .push_back(corrak8.mass());

      double theCHSPt = -std::numeric_limits<double>::max();
      double theCHSEta = -std::numeric_limits<double>::max();
      double theCHSPhi = -std::numeric_limits<double>::max();
      double theCHSMass = -std::numeric_limits<double>::max();
      theCHSPt = corrak8.userFloat("ak8PFJetsCHSValueMap:pt");
      theCHSEta = corrak8.userFloat("ak8PFJetsCHSValueMap:eta");
      theCHSPhi = corrak8.userFloat("ak8PFJetsCHSValueMap:phi");
      theCHSMass = corrak8.userFloat("ak8PFJetsCHSValueMap:mass");
      theJetAK8CHSPt.push_back(theCHSPt);
      theJetAK8CHSEta.push_back(theCHSEta);
      theJetAK8CHSPhi.push_back(theCHSPhi);
      theJetAK8CHSMass.push_back(theCHSMass);

      double genDR = -99;
      double genpt = -99;
      double genmass = -99;
      TLorentzVector ak8jet;
      ak8jet.SetPtEtaPhiE(corrak8.pt(),corrak8.eta(),corrak8.phi(),corrak8.energy());
      const reco::GenJet * genJet = corrak8.genJet();
      if(genJet){
        TLorentzVector genP4;
        genP4.SetPtEtaPhiE(genJet->pt(),genJet->eta(),genJet->phi(),genJet->energy());
        genDR = ak8jet.DeltaR(genP4);
        genpt = genJet->pt();
        genmass = genJet->mass();
      }
      theJetAK8GenPt.push_back(genpt);
      theJetAK8GenMass.push_back(genmass);
      theJetAK8GenDR.push_back(genDR);

      theCHSPrunedMass   = -std::numeric_limits<double>::max();
      theCHSSoftDropMass = -std::numeric_limits<double>::max();
      theSoftDrop = -std::numeric_limits<double>::max();

      theCHSPrunedMass   = (double)corrak8.userFloat("ak8PFJetsCHSValueMap:ak8PFJetsCHSPrunedMass");
      theCHSSoftDropMass = (double)corrak8.userFloat("ak8PFJetsCHSValueMap:ak8PFJetsCHSSoftDropMass");
      theSoftDrop = (double)corrak8.userFloat("ak8PFJetsPuppiSoftDropMass");

      theNjettinessTau1 = std::numeric_limits<double>::max();
      theNjettinessTau2 = std::numeric_limits<double>::max();
      theNjettinessTau3 = std::numeric_limits<double>::max();
      theNjettinessTau1 = (double)corrak8.userFloat("NjettinessAK8Puppi:tau1");
      theNjettinessTau2 = (double)corrak8.userFloat("NjettinessAK8Puppi:tau2");
      theNjettinessTau3 = (double)corrak8.userFloat("NjettinessAK8Puppi:tau3");

      theCHSTau1 = std::numeric_limits<double>::max();
      theCHSTau2 = std::numeric_limits<double>::max();
      theCHSTau3 = std::numeric_limits<double>::max();
      theCHSTau1 = (double)corrak8.userFloat("ak8PFJetsCHSValueMap:NjettinessAK8CHSTau1");
      theCHSTau2 = (double)corrak8.userFloat("ak8PFJetsCHSValueMap:NjettinessAK8CHSTau2");
      theCHSTau3 = (double)corrak8.userFloat("ak8PFJetsCHSValueMap:NjettinessAK8CHSTau3");

      theSoftDropn2b1 = std::numeric_limits<double>::max();
      theSoftDropn3b1 = std::numeric_limits<double>::max();
      theSoftDropn2b2 = std::numeric_limits<double>::max();
      theSoftDropn3b2 = std::numeric_limits<double>::max();
      theSoftDropn2b1 = (double)corrak8.userFloat("ak8PFJetsPuppiSoftDropValueMap:nb1AK8PuppiSoftDropN2");
      theSoftDropn3b1 = (double)corrak8.userFloat("ak8PFJetsPuppiSoftDropValueMap:nb1AK8PuppiSoftDropN3");
      theSoftDropn2b2 = (double)corrak8.userFloat("ak8PFJetsPuppiSoftDropValueMap:nb2AK8PuppiSoftDropN2");
      theSoftDropn3b2 = (double)corrak8.userFloat("ak8PFJetsPuppiSoftDropValueMap:nb2AK8PuppiSoftDropN3");

      theJetAK8CSV.push_back(corrak8.bDiscriminator( bDiscriminant ));
      theJetAK8DoubleB.push_back(corrak8.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags"));

      theJetAK8CHSPrunedMass.push_back(theCHSPrunedMass); // JEC only
      theJetAK8CHSSoftDropMass.push_back(theCHSSoftDropMass); // JEC only
      theJetAK8SoftDrop.push_back(theSoftDrop);

      theJetAK8CHSTau1.push_back(theCHSTau1);
      theJetAK8CHSTau2.push_back(theCHSTau2);
      theJetAK8CHSTau3.push_back(theCHSTau3);
      theJetAK8NjettinessTau1.push_back(theNjettinessTau1);
      theJetAK8NjettinessTau2.push_back(theNjettinessTau2);
      theJetAK8NjettinessTau3.push_back(theNjettinessTau3);

      theJetAK8SoftDropn2b1.push_back(theSoftDropn2b1);
      theJetAK8SoftDropn2b2.push_back(theSoftDropn2b2);
      theJetAK8SoftDropn3b1.push_back(theSoftDropn3b1);
      theJetAK8SoftDropn3b2.push_back(theSoftDropn3b2);

      theJetAK8nDaughters.push_back((int)corrak8.numberOfDaughters());
      theJetAK8Index.push_back(index);

      //JetCharge calculation

      reco::Jet::Constituents constituents = corrak8.getJetConstituents();

      double sumWeightedCharge = 0.0;
      int con_charge = 0;
      double con_pt = 0.0;
      for(auto constituentItr=constituents.begin(); constituentItr!=constituents.end(); ++constituentItr){
        edm::Ptr<reco::Candidate> constituent=*constituentItr;

        con_charge = (int)constituent->charge();
        con_pt     = (double)constituent->pt();

        sumWeightedCharge = sumWeightedCharge + ( con_charge * pow(con_pt,kappa) );

      }

      jetCharge  = 1.0/( pow( (corrak8.pt()), kappa) ) * sumWeightedCharge;

      theJetAK8JetCharge.push_back(jetCharge);

      // Get Soft drop subjets for subjet b-tagging
      SDSubJetIndex = (int)theJetAK8SDSubjetPt.size();
      nSDSubJets  = std::numeric_limits<int>::min();
      nSDSubsDeepCSVL = 0;
      nSDSubsDeepCSVMSF = 0;
      nSDSubsDeepCSVM_bSFup = 0;
      nSDSubsDeepCSVM_bSFdn = 0;
      nSDSubsDeepCSVM_lSFup = 0;
      nSDSubsDeepCSVM_lSFdn = 0;

      TLorentzVector puppi_softdrop, puppi_softdrop_subjet;
      std::vector<std::string> labels = corrak8.subjetCollectionNames();
      if(labels.size() == 0) std::cout << "there are no subjet collection labels" << std::endl;
      // for (unsigned int j = 0; j < labels.size(); j++){
      //        std::cout << labels.at(j) << std::endl;
      // }
      auto const & sdSubjets = corrak8.subjets("SoftDropPuppi");
      //auto const & sdSubjets = corrak8.subjets("SoftDrop");
      nSDSubJets = (int)sdSubjets.size();
      //std::cout << "Found " << nSDSubJets << " subjets" << std::endl;
      for ( auto const & it : sdSubjets ) {

        puppi_softdrop_subjet.SetPtEtaPhiM(it->correctedP4(0).pt(),it->correctedP4(0).eta(),it->correctedP4(0).phi(),it->correctedP4(0).mass());
        puppi_softdrop+=puppi_softdrop_subjet;


        //for jet correction
        unsigned int syst;
        if (JECup){syst=1;}
        else if (JECdown){syst=2;}
        else if (JERup){syst=3;}
        else if (JERdown){syst=4;}
        else syst = 0; //nominal
        bool isAK8 = false;

        pat::Jet corrsubjet;
        corrsubjet = JetMETCorr.correctJetReturnPatJet(*it, event, rhoJetsToken, isAK8, doNewJEC, syst);


        SDsubjetPt          = -std::numeric_limits<double>::max();
        SDsubjetEta         = -std::numeric_limits<double>::max();
        SDsubjetPhi         = -std::numeric_limits<double>::max();
        SDsubjetMass        = -std::numeric_limits<double>::max();
        SDsubjetDeepCSVb    = -std::numeric_limits<double>::max();
        SDsubjetDeepCSVc    = -std::numeric_limits<double>::max();
        SDsubjetDeepCSVudsg = -std::numeric_limits<double>::max();
        SDsubjetDeepCSVbb   = -std::numeric_limits<double>::max();
        SDdeltaRsubjetJet   = std::numeric_limits<double>::max();

        SDsubjetPt           = corrsubjet.pt();
        SDsubjetEta          = corrsubjet.eta();
        SDsubjetPhi          = corrsubjet.phi();
        SDsubjetMass         = corrsubjet.mass();
        SDsubjetDeepCSVb     = corrsubjet.bDiscriminator(bDiscriminant);
        SDsubjetDeepCSVbb    = corrsubjet.bDiscriminator(bbDiscriminant);
        SDsubjetDeepCSVc     = corrsubjet.bDiscriminator(cDiscriminant);
        SDsubjetDeepCSVudsg  = corrsubjet.bDiscriminator(udsgDiscriminant);
        SDsubjetHFlav        = corrsubjet.hadronFlavour();

        TLorentzVector subjetP4; subjetP4.SetPtEtaPhiE(ii->pt(), ii->eta(), ii->phi(), ii->energy() );

        SDsubjetBTag         = btagSfUtil.isJetTagged(corrsubjet, subjetP4, event, isMc, 0, true);
        SDdeltaRsubjetJet    = deltaR(corrak8.eta(), corrak8.phi(), SDsubjetEta, SDsubjetPhi);

        if(SDsubjetDeepCSVb + SDsubjetDeepCSVbb > 0.1522) nSDSubsDeepCSVL++;
        if(SDsubjetBTag > 0) nSDSubsDeepCSVMSF++;
        if(btagSfUtil.isJetTagged(corrsubjet, subjetP4, event, isMc, 1, true) > 0) nSDSubsDeepCSVM_bSFup++;
        if(btagSfUtil.isJetTagged(corrsubjet, subjetP4, event, isMc, 2, true) > 0) nSDSubsDeepCSVM_bSFdn++;
        if(btagSfUtil.isJetTagged(corrsubjet, subjetP4, event, isMc, 3, true) > 0) nSDSubsDeepCSVM_lSFup++;
        if(btagSfUtil.isJetTagged(corrsubjet, subjetP4, event, isMc, 4, true) > 0) nSDSubsDeepCSVM_lSFdn++;

        theJetAK8SDSubjetPt.push_back(SDsubjetPt);
        theJetAK8SDSubjetEta.push_back(SDsubjetEta);
        theJetAK8SDSubjetPhi.push_back(SDsubjetPhi);
        theJetAK8SDSubjetMass.push_back(SDsubjetMass);
        theJetAK8SDSubjetCSVb.push_back(SDsubjetDeepCSVb);
        theJetAK8SDSubjetCSVc.push_back(SDsubjetDeepCSVc);
        theJetAK8SDSubjetCSVudsg.push_back(SDsubjetDeepCSVudsg);
        theJetAK8SDSubjetCSVbb.push_back(SDsubjetDeepCSVbb);
        theJetAK8SDSubjetHFlav.push_back(SDsubjetHFlav);
        theJetAK8SDSubjetBTag.push_back(SDsubjetBTag);
        theJetAK8SDSubjetDR.push_back(SDdeltaRsubjetJet);
      }

      theJetAK8SDSubjetIndex.push_back(SDSubJetIndex);
      theJetAK8SDSubjetSize.push_back(nSDSubJets);

      // Get CHS Soft drop subjets for subjet b-tagging
      theJetAK8SDSubjetNDeepCSVL.push_back(nSDSubsDeepCSVL);
      theJetAK8SDSubjetNDeepCSVMSF.push_back(nSDSubsDeepCSVMSF);
      theJetAK8SDSubjetNDeepCSVM_bSFup.push_back(nSDSubsDeepCSVM_bSFup);
      theJetAK8SDSubjetNDeepCSVM_bSFdn.push_back(nSDSubsDeepCSVM_bSFdn);
      theJetAK8SDSubjetNDeepCSVM_lSFup.push_back(nSDSubsDeepCSVM_lSFup);
      theJetAK8SDSubjetNDeepCSVM_lSFdn.push_back(nSDSubsDeepCSVM_lSFdn);

      double puppicorr = 1.0;
      float genCorr  = 1.;
      float recoCorr = 1.;

      genCorr =  puppisd_corrGEN->Eval(corrak8.pt());
      if(fabs(corrak8.eta()) <= 1.3 ) recoCorr = puppisd_corrRECO_cen->Eval(corrak8.pt());
      else recoCorr = puppisd_corrRECO_for->Eval(corrak8.pt());

      puppicorr = genCorr * recoCorr;

      theSoftDrop = puppi_softdrop.M();
      double theSoftDropCorrected = theSoftDrop*puppicorr;

      double jmr_sd = 1.0;
      double jmr_sd_up = 1.0;
      double jmr_sd_dn = 1.0;
      double jms_sd = 1.0;
      double jms_sd_up = 1.0;
      double jms_sd_dn = 1.0;

      if(isMc){  // updated for tau21 <= 0.45 WP, the only one there is...
        double res = 8.753/theSoftDropCorrected;
        double factor_sd = 1.09;
        double uncert_sd = 0.05;
        double factor_sd_up = factor_sd + uncert_sd;
        double factor_sd_dn = factor_sd - uncert_sd;

        if (factor_sd>1) {
          JERrand.SetSeed(abs(static_cast<int>(corrak8.phi()*1e4)));
          jmr_sd = 1 + JERrand.Gaus(0,res)*sqrt(factor_sd*factor_sd - 1.0);
        }
        if (factor_sd_up>1) {
          JERrand.SetSeed(abs(static_cast<int>(corrak8.phi()*1e4)));
          jmr_sd_up = 1 + JERrand.Gaus(0,res)*sqrt(factor_sd_up*factor_sd_up - 1.0);
        }
        if (factor_sd_dn>1) {
          JERrand.SetSeed(abs(static_cast<int>(corrak8.phi()*1e4)));
          jmr_sd_dn = 1 + JERrand.Gaus(0,res)*sqrt(factor_sd_dn*factor_sd_dn - 1.0);
        }
        jms_sd = 0.982;
        jms_sd_up = jms_sd + 0.004;
        jms_sd_dn = jms_sd - 0.004;
      }

      int MaxProb = 10;
      double doubleB = corrak8.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags");
      if (theSoftDropCorrected > 135 && theSoftDropCorrected < 210 && theNjettinessTau3/theNjettinessTau2 < 0.65) MaxProb = 1; //top
      else if (theSoftDropCorrected > 105 && theSoftDropCorrected < 135 && doubleB > 0.6) MaxProb = 2; //H
      else if (theSoftDropCorrected < 105 && theSoftDropCorrected > 85 && theNjettinessTau2/theNjettinessTau1 < 0.55) MaxProb = 3; //Z
      else if (theSoftDropCorrected < 85 && theSoftDropCorrected > 65 && theNjettinessTau2/theNjettinessTau1 < 0.55) MaxProb = 4; //W
      else if (nSDSubsDeepCSVMSF > 0) MaxProb = 5;
      else if (nSDSubsDeepCSVMSF == 0) MaxProb = 0;
      else MaxProb = 10;

      maxProb.push_back(MaxProb);

      theJetAK8SoftDropRaw.push_back(theSoftDrop);
      theJetAK8SoftDropCorr.push_back(theSoftDropCorrected);
      theJetAK8SoftDrop.push_back(theSoftDropCorrected*jmr_sd*jms_sd);
      theJetAK8SoftDrop_JMSup.push_back(theSoftDropCorrected*jmr_sd*jms_sd_up);
      theJetAK8SoftDrop_JMSdn.push_back(theSoftDropCorrected*jmr_sd*jms_sd_dn);
      theJetAK8SoftDrop_JMRup.push_back(theSoftDropCorrected*jmr_sd_up*jms_sd);
      theJetAK8SoftDrop_JMRdn.push_back(theSoftDropCorrected*jmr_sd_dn*jms_sd);


    }

    SetValue("maxProb", maxProb);
    SetValue("theJetAK8Pt",     theJetAK8Pt);
    SetValue("theJetAK8Eta",    theJetAK8Eta);
    SetValue("theJetAK8Phi",    theJetAK8Phi);
    SetValue("theJetAK8Energy", theJetAK8Energy);
    SetValue("theJetAK8CSV",    theJetAK8CSV);
    SetValue("theJetAK8DoubleB",    theJetAK8DoubleB);
    SetValue("theJetAK8JetCharge", theJetAK8JetCharge);
    SetValue("theJetAK8GenPt",  theJetAK8GenPt);
    SetValue("theJetAK8GenDR",  theJetAK8GenDR);
    SetValue("theJetAK8GenMass",  theJetAK8GenMass);

    SetValue("theJetAK8CHSPt",     theJetAK8CHSPt);
    SetValue("theJetAK8CHSEta",    theJetAK8CHSEta);
    SetValue("theJetAK8CHSPhi",    theJetAK8CHSPhi);
    SetValue("theJetAK8CHSMass", theJetAK8CHSMass);
    SetValue("theJetAK8SoftDropRaw", theJetAK8SoftDropRaw);
    SetValue("theJetAK8SoftDropCorr", theJetAK8SoftDropCorr);
    SetValue("theJetAK8SoftDrop", theJetAK8SoftDrop);
    SetValue("theJetAK8SoftDrop_JMSup", theJetAK8SoftDrop_JMSup);
    SetValue("theJetAK8SoftDrop_JMSdn", theJetAK8SoftDrop_JMSdn);
    SetValue("theJetAK8SoftDrop_JMRup", theJetAK8SoftDrop_JMRup);
    SetValue("theJetAK8SoftDrop_JMRdn", theJetAK8SoftDrop_JMRdn);

    SetValue("theJetAK8CHSPrunedMass",   theJetAK8CHSPrunedMass);
    SetValue("theJetAK8CHSSoftDropMass", theJetAK8CHSSoftDropMass);

    SetValue("theJetAK8NjettinessTau1", theJetAK8NjettinessTau1);
    SetValue("theJetAK8NjettinessTau2", theJetAK8NjettinessTau2);
    SetValue("theJetAK8NjettinessTau3", theJetAK8NjettinessTau3);
    SetValue("theJetAK8CHSTau1", theJetAK8CHSTau1);
    SetValue("theJetAK8CHSTau2", theJetAK8CHSTau2);
    SetValue("theJetAK8CHSTau3", theJetAK8CHSTau3);

    SetValue("theJetAK8SoftDropn2b1",theJetAK8SoftDropn2b1);
    SetValue("theJetAK8SoftDropn2b2",theJetAK8SoftDropn2b2);
    SetValue("theJetAK8SoftDropn3b1",theJetAK8SoftDropn3b1);
    SetValue("theJetAK8SoftDropn3b2",theJetAK8SoftDropn3b2);

    SetValue("theJetAK8Mass",   theJetAK8Mass);
    SetValue("theJetAK8nDaughters", theJetAK8nDaughters);

    SetValue("theJetAK8SDSubjetPt",   theJetAK8SDSubjetPt);
    SetValue("theJetAK8SDSubjetEta",  theJetAK8SDSubjetEta);
    SetValue("theJetAK8SDSubjetPhi",  theJetAK8SDSubjetPhi);
    SetValue("theJetAK8SDSubjetMass", theJetAK8SDSubjetMass);
    SetValue("theJetAK8SDSubjetCSVb",  theJetAK8SDSubjetCSVb);
    SetValue("theJetAK8SDSubjetCSVc",  theJetAK8SDSubjetCSVc);
    SetValue("theJetAK8SDSubjetCSVudsg",  theJetAK8SDSubjetCSVudsg);
    SetValue("theJetAK8SDSubjetCSVbb",  theJetAK8SDSubjetCSVbb);
    SetValue("theJetAK8SDSubjetHFlav", theJetAK8SDSubjetHFlav);
    SetValue("theJetAK8SDSubjetBTag",  theJetAK8SDSubjetBTag);
    SetValue("theJetAK8SDSubjetDR",   theJetAK8SDSubjetDR);
    SetValue("theJetAK8SDSubjetIndex",theJetAK8SDSubjetIndex);
    SetValue("theJetAK8SDSubjetSize", theJetAK8SDSubjetSize);

    SetValue("theJetAK8SDSubjetNDeepCSVL",theJetAK8SDSubjetNDeepCSVL);
    SetValue("theJetAK8SDSubjetNDeepCSVMSF",theJetAK8SDSubjetNDeepCSVMSF);
    SetValue("theJetAK8SDSubjetNDeepCSVM_bSFup",theJetAK8SDSubjetNDeepCSVM_bSFup);
    SetValue("theJetAK8SDSubjetNDeepCSVM_bSFdn",theJetAK8SDSubjetNDeepCSVM_bSFdn);
    SetValue("theJetAK8SDSubjetNDeepCSVM_lSFup",theJetAK8SDSubjetNDeepCSVM_lSFup);
    SetValue("theJetAK8SDSubjetNDeepCSVM_lSFdn",theJetAK8SDSubjetNDeepCSVM_lSFdn);

    //////////////// TRUE HADRONIC W/Z/H/Top decays //////////////////
    std::vector<int>    HadronicVHtID;
    std::vector<int>    HadronicVHtStatus;
    std::vector<double> HadronicVHtPt;
    std::vector<double> HadronicVHtEta;
    std::vector<double> HadronicVHtPhi;
    std::vector<double> HadronicVHtEnergy;
    std::vector<double> HadronicVHtD0Pt;
    std::vector<double> HadronicVHtD0Eta;
    std::vector<double> HadronicVHtD0Phi;
    std::vector<double> HadronicVHtD0E;
    std::vector<double> HadronicVHtD1Pt;
    std::vector<double> HadronicVHtD1Eta;
    std::vector<double> HadronicVHtD1Phi;
    std::vector<double> HadronicVHtD1E;
    std::vector<double> HadronicVHtD2Pt;
    std::vector<double> HadronicVHtD2Eta;
    std::vector<double> HadronicVHtD2Phi;
    std::vector<double> HadronicVHtD2E;

    // Get the generated particle collection
    edm::Handle<reco::GenParticleCollection> genParticles;
    if(isMc && event.getByToken(genParticlesToken, genParticles)){

      for(size_t i = 0; i < genParticles->size(); i++){
        const reco::GenParticle &p = (*genParticles).at(i);
        int id = p.pdgId();

        bool hasRadiation = false;
        bool hasLepton = false;

        if(abs(id) == 23 || abs(id) == 24 || abs(id) == 25 || abs(id) == 6){

          size_t nDs = p.numberOfDaughters();
          for(size_t j = 0; j < nDs; j++){
            int dauId = (p.daughter(j))->pdgId();
            const reco::Candidate *d = p.daughter(j);
            if(d->pdgId() != dauId) std::cout << "making daughter GenParticle didn't work" << std::endl;

            if(abs(dauId) == abs(id)) hasRadiation = true;
            else if(abs(dauId) == 24){  // check t->Wb->leptons and H->WW->leptons
              while(d->numberOfDaughters() == 1) d = d->daughter(0);
              if(abs(d->daughter(0)->pdgId()) > 10 && abs(d->daughter(0)->pdgId()) < 17) hasLepton = true;
              if(abs(d->daughter(1)->pdgId()) > 10 && abs(d->daughter(1)->pdgId()) < 17) hasLepton = true;
            }
            else if(abs(dauId) == 23){  // check H->ZZ->leptons
              while(d->numberOfDaughters() == 1) d = d->daughter(0);
              if(abs(d->daughter(0)->pdgId()) > 10 && abs(d->daughter(0)->pdgId()) < 17) hasLepton = true;
              if(abs(d->daughter(1)->pdgId()) > 10 && abs(d->daughter(1)->pdgId()) < 17) hasLepton = true;
            }
            else if(abs(dauId) > 10 && abs(dauId) < 17) hasLepton = true;

          }

          if(hasRadiation) continue;
          if(hasLepton) continue;
          if(p.pt() < 175) continue;

          if(abs(id) == 24){
            double dRWb = 1000;
            double dRWW = 1000;

            const reco::Candidate *mother = p.mother();
            while(abs(mother->pdgId()) == 24) mother = mother->mother();

            if(abs(mother->pdgId()) == 6){
              double dr = reco::deltaR(p.p4(),mother->daughter(1)->p4());
              if(abs(mother->daughter(1)->pdgId()) == 24) dr = reco::deltaR(p.p4(),mother->daughter(0)->p4());
              if(dr < dRWb) dRWb = dr;
            }else if(abs(mother->pdgId()) == 25){
              double dr = 1000;
              if(p.pdgId()*mother->daughter(0)->pdgId() > 0){
                dr = reco::deltaR(p.p4(),mother->daughter(1)->p4());
              }else{
                dr = reco::deltaR(p.p4(),mother->daughter(0)->p4());
              }
              if(dr < dRWW) dRWW = dr;
            }

            if(dRWW < 0.8) continue; // W from merged H
            if(dRWb < 0.8) continue; // W from merged t
          }

          if(abs(id) == 23){
            double dRZZ = 1000;

            const reco::Candidate *mother = p.mother();
            while(abs(mother->pdgId()) == 23) mother = mother->mother();

            if(abs(mother->pdgId()) == 25){
              double dr = 1000;
              if(p.pdgId()*mother->daughter(0)->pdgId() > 0){
                dr = reco::deltaR(p.p4(),mother->daughter(1)->p4());
              }else{
                dr = reco::deltaR(p.p4(),mother->daughter(0)->p4());
              }
              if(dr < dRZZ) dRZZ = dr;
            }

            if(dRZZ < 0.8) continue; // Z from merged H
          }

          if(p.numberOfDaughters() < 2){
            std::cout << p.numberOfDaughters() << " daughters from " << p.pdgId() << std::endl;
            continue;
          }

          HadronicVHtStatus.push_back( p.status() );
          HadronicVHtID.push_back( p.pdgId() );
          HadronicVHtPt.push_back( p.pt() );
          HadronicVHtEta.push_back( p.eta() );
          HadronicVHtPhi.push_back( p.phi() );
          HadronicVHtEnergy.push_back( p.energy() );

          if(abs(id) != 6){
            HadronicVHtD0Pt.push_back( p.daughter(0)->pt());
            HadronicVHtD0Eta.push_back( p.daughter(0)->eta());
            HadronicVHtD0Phi.push_back( p.daughter(0)->phi());
            HadronicVHtD0E.push_back( p.daughter(0)->energy());
            HadronicVHtD1Pt.push_back( p.daughter(1)->pt());
            HadronicVHtD1Eta.push_back( p.daughter(1)->eta());
            HadronicVHtD1Phi.push_back( p.daughter(1)->phi());
            HadronicVHtD1E.push_back( p.daughter(1)->energy());
            HadronicVHtD2Pt.push_back(-99.9);
            HadronicVHtD2Eta.push_back(-99.9);
            HadronicVHtD2Phi.push_back(-99.9);
            HadronicVHtD2E.push_back(-99.9);
          }else{
            const reco::Candidate *W = p.daughter(0);
            const reco::Candidate *b = p.daughter(1);
            if(fabs(W->pdgId()) != 24){
              W = p.daughter(1);
              b = p.daughter(0);
            }
            while(W->numberOfDaughters() == 1) W = W->daughter(0);
            if(W->daughter(1)->pdgId() == 22) W = W->daughter(0);
            while(W->numberOfDaughters() == 1) W = W->daughter(0);
            if(W->daughter(1)->pdgId() == 22) W = W->daughter(0);
            while(W->numberOfDaughters() == 1) W = W->daughter(0);
            if(W->daughter(1)->pdgId()==22) cout << "weird W decay to photons" << endl;

            HadronicVHtD0Pt.push_back( b->pt());
            HadronicVHtD0Eta.push_back( b->eta());
            HadronicVHtD0Phi.push_back( b->phi());
            HadronicVHtD0E.push_back( b->energy());
            HadronicVHtD1Pt.push_back( W->daughter(0)->pt());
            HadronicVHtD2Pt.push_back( W->daughter(1)->pt());
            HadronicVHtD1Eta.push_back( W->daughter(0)->eta());
            HadronicVHtD2Eta.push_back( W->daughter(1)->eta());
            HadronicVHtD1Phi.push_back( W->daughter(0)->phi());
            HadronicVHtD2Phi.push_back( W->daughter(1)->phi());
            HadronicVHtD1E.push_back( W->daughter(0)->energy());
            HadronicVHtD2E.push_back( W->daughter(1)->energy());
          }
        }
      }
    }

    SetValue("HadronicVHtStatus",HadronicVHtStatus);
    SetValue("HadronicVHtID",HadronicVHtID);
    SetValue("HadronicVHtPt",HadronicVHtPt);
    SetValue("HadronicVHtEta",HadronicVHtEta);
    SetValue("HadronicVHtPhi",HadronicVHtPhi);
    SetValue("HadronicVHtEnergy",HadronicVHtEnergy);
    SetValue("HadronicVHtD0Pt",HadronicVHtD0Pt);
    SetValue("HadronicVHtD0Eta",HadronicVHtD0Eta);
    SetValue("HadronicVHtD0Phi",HadronicVHtD0Phi);
    SetValue("HadronicVHtD0E",HadronicVHtD0E);
    SetValue("HadronicVHtD1Pt",HadronicVHtD1Pt);
    SetValue("HadronicVHtD1Eta",HadronicVHtD1Eta);
    SetValue("HadronicVHtD1Phi",HadronicVHtD1Phi);
    SetValue("HadronicVHtD1E",HadronicVHtD1E);
    SetValue("HadronicVHtD2Pt",HadronicVHtD2Pt);
    SetValue("HadronicVHtD2Eta",HadronicVHtD2Eta);
    SetValue("HadronicVHtD2Phi",HadronicVHtD2Phi);
    SetValue("HadronicVHtD2E",HadronicVHtD2E);

    return 0;
}

int JetSubCalc::EndJob()
{
  return 0;
}
