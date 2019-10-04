#ifndef FWLJMET_LJMet_interface_MultiLepEventSelector_h
#define FWLJMET_LJMet_interface_MultiLepEventSelector_h

#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"


#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "PhysicsTools/SelectorUtils/interface/PVSelector.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "FWLJMET/LJMet/interface/MiniIsolation.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include <TRandom3.h>
#include <boost/algorithm/string.hpp>
#include <regex>

#include "FWLJMET/LJMet/interface/JetMETCorrHelper.h"
#include "FWLJMET/LJMet/interface/BTagSFUtil.h"


using namespace std;

class MultiLepEventSelector : public BaseEventSelector { //adapted from singleLepEventSelector.cc

public:


    MultiLepEventSelector();
    ~MultiLepEventSelector();


    // executes before loop over events
    virtual void BeginJob(const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC);

    // main method where the cuts are applied
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );

    // executes after loop over events
    virtual void EndJob();


protected:

    //Misc
    bool bFirstEntry;
    bool debug;
    bool isMc;


    //Trigger
    bool trigger_cut;
    bool dump_trigger;
    std::vector<std::string> mctrigger_path_el;
    std::vector<std::string> mctrigger_path_mu;
    std::vector<std::string> mctrigger_path_hadronic;
    std::vector<std::string> trigger_path_el;
    std::vector<std::string> trigger_path_mu;
    std::vector<std::string> trigger_path_hadronic;

    //PV
    bool pv_cut;
    boost::shared_ptr<PVSelector> pvSel_;

    //MET Filter
    bool   metfilters;

    //MET
    bool   met_cuts;
    double min_met;
    double max_met;

    //Muon
    bool   muon_cuts;
    int    min_muon;
    double muon_minpt;
    double muon_maxeta;
    bool   muon_useMiniIso;
    double muon_dxy;
    double muon_dz;
    double muon_relIso;
    double loose_muon_minpt;
    double loose_muon_maxeta;
    double loose_muon_dxy;
    double loose_muon_dz;
    double loose_muon_relIso;

    //Electron
    bool   electron_cuts;
    int    min_electron;
    double electron_minpt;
    double electron_maxeta;
    bool   electron_useMiniIso;
    double electron_miniIso;
    double loose_electron_miniIso;
    double loose_electron_minpt;
    double loose_electron_maxeta;
    bool   UseElMVA;
    bool   UseElIDV1;

    //nLepton
    bool minLeptons_cut;
    int  minLeptons;
    bool maxLeptons_cut;
    int  maxLeptons;
    bool minLooseLeptons_cut;
    int  minLooseLeptons;
    bool maxLooseLeptons_cut;
    int  maxLooseLeptons;

    //Jets
    bool   JECup;
    bool   JECdown;
    bool   JERup;
    bool   JERdown;
    bool   doNewJEC;
    bool   doAllJetSyst;
    bool   doLepJetCleaning;
    bool   CleanLooseLeptons;
    double LepJetDR;
    double LepJetDRAK8;
    bool   jet_cuts;
    double jet_minpt;
    double jet_maxeta;
    double jet_minpt_AK8;
    double jet_maxeta_AK8;
    int    min_jet;
    int    max_jet;
    double leading_jet_pt;
    JetMETCorrHelper JetMETCorr;

    //Btag
    bool        btag_cuts;
    BTagSFUtil btagSfUtil;

    //Tokens
    edm::EDGetTokenT<GenEventInfoProduct>            genToken;
    edm::EDGetTokenT<edm::TriggerResults>            triggersToken;
    edm::EDGetTokenT<reco::VertexCollection>         PVToken;
    edm::EDGetTokenT<edm::TriggerResults>            METfilterToken;
    edm::EDGetTokenT<bool>                           METfilterToken_extra;
    edm::EDGetTokenT<std::vector<pat::MET> >         METtoken;
    edm::EDGetTokenT<pat::MuonCollection>            muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection>        electronsToken;
    edm::EDGetTokenT<pat::PackedCandidateCollection> PFCandToken;
    edm::EDGetTokenT<double>                         rhoJetsNC_Token;
	edm::EDGetTokenT<double>                         rhoJetsToken;
    edm::EDGetTokenT<pat::JetCollection>             jetsToken;
    edm::EDGetTokenT<pat::JetCollection>             AK8jetsToken;


    //Separate methods for each selction for organization
    bool TriggerSelection  (edm::Event const & event);
    bool PVSelection       (edm::Event const & event);
    bool METfilter         (edm::Event const & event);
    void MuonSelection     (edm::Event const & event);
    void ElectronSelection (edm::Event const & event);
    bool LeptonsSelection  (edm::Event const & event, pat::strbitset & ret);
    bool JetSelection      (edm::Event const & event, pat::strbitset & ret);
    void AK8JetSelection   (edm::Event const & event);
    bool METSelection      (edm::Event const & event);


};

#endif
