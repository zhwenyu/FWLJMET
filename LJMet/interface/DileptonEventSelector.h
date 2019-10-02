// -*- C++ -*-
//
// FWLite PAT analyzer-selector for dilepton analyses
//
// Adapted from StopEventSelector
// Aram Avetisyan, September 2012
// Updated
// Rizki Syarif, March 2019
//
#ifndef FWLJMET_LJMet_interface_DileptonEventSelector_h
#define FWLJMET_LJMet_interface_DileptonEventSelector_h



#include <cmath>
#include <iostream>

#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"


#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"
#include "TVector3.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "FWLJMET/LJMet/interface/MiniIsolation.h"

#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "PhysicsTools/SelectorUtils/interface/PFMuonSelector.h"
#include "PhysicsTools/SelectorUtils/interface/PVSelector.h"
#include "DataFormats/METReco/interface/HcalNoiseSummary.h"

#include "FWLJMET/LJMet/interface/JetMETCorrHelper.h"
#include "FWLJMET/LJMet/interface/BTagSFUtil.h"

using trigger::TriggerObject;


class DileptonEventSelector : public BaseEventSelector {

public:


    DileptonEventSelector();
    ~DileptonEventSelector();


    // executes before loop over events
    virtual void BeginJob(const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC);

    // main method where the cuts are applied
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret);

    // executes after loop over events
    virtual void EndJob(){}


    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );


    boost::shared_ptr<PFJetIDSelectionFunctor> const & jetSel()        const { return jetSel_;}
    boost::shared_ptr<PVSelector>              const & pvSel()         const { return pvSel_;}


protected:

    std::string legend;
    bool bFirstEntry;

private:

    void initialize(std::map<std::string, edm::ParameterSet const> par);


    bool debug;
    bool isMc;

    //Triggers
    bool trigger_cut;
    bool dump_trigger;
    std::vector<std::string> trigger_path_ee;
    std::vector<std::string> trigger_path_em;
    std::vector<std::string> trigger_path_mm;

    //PV
    bool pv_cut;
    boost::shared_ptr<PVSelector>              pvSel_;

    //MET Filter
    bool   metfilters;

	//Muon
    bool muon_cuts;
    int min_muon;
    int max_muon;
    double muon_minpt;
    double muon_maxeta;
    
    //Electron
    bool electron_cuts;
    int min_electron;
    int max_electron;
    double electron_minpt;
    double electron_maxeta;
    bool UseElMVA;
    
    //nLepton
    int min_lepton;

    //Jets
    boost::shared_ptr<PFJetIDSelectionFunctor> jetSel_;
    bool   JECup;
    bool   JECdown;
    bool   JERup;
    bool   JERdown;
    bool   doNewJEC;
    bool   doLepJetCleaning;
    bool jet_cuts;
    bool jet_minpt;
    double jet_maxeta;
    int min_jet;
    int max_jet;
    JetMETCorrHelper JetMETCorr;
    
    //MET
    bool met_cuts;
    bool min_met;
    
    //Btag
    BTagSFUtil btagSfUtil;

    edm::Handle<edm::TriggerResults >           TriggerHandle;
    edm::Handle<std::vector<pat::Muon> >        muonsHandle;
    edm::Handle<std::vector<pat::Electron> >    electronsHandle;
    edm::Handle<std::vector<pat::Jet> >         jetsHandle;
    edm::Handle<std::vector<pat::MET> >         mhMet;

    // Tokens
    edm::EDGetTokenT<edm::TriggerResults>            triggersToken;
    edm::EDGetTokenT<reco::VertexCollection>         PVToken;
    edm::EDGetTokenT<edm::TriggerResults>            METfilterToken;
    edm::EDGetTokenT<bool>                           METfilterToken_extra;
    edm::EDGetTokenT<pat::MuonCollection>            muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection>        electronsToken;
    edm::EDGetTokenT<pat::JetCollection>             jetsToken;
    edm::EDGetTokenT<double>                         rhoJetsToken;
    edm::EDGetTokenT<std::vector<pat::MET> >         METtoken;
    edm::EDGetTokenT<pat::PackedCandidateCollection> PFCandToken;
    edm::EDGetTokenT<double>                         rhoJetsNC_Token;

    // Separate methods for each selction for organization
    bool TriggerSelection  (edm::Event const & event);
    bool PVSelection       (edm::Event const & event);
    bool METfilter         (edm::Event const & event);
    bool MuonSelection     (edm::Event const & event, pat::strbitset & ret);
    bool ElectronSelection (edm::Event const & event, pat::strbitset & ret);
    bool LeptonsSelection  (edm::Event const & event, pat::strbitset & ret);
    bool JetSelection      (edm::Event const & event, pat::strbitset & ret);
    bool METSelection      (edm::Event const & event);

};

#endif
