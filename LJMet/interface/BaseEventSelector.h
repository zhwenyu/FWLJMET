#ifndef FWLJMET_LJMet_interface_BaseEventSelector_h
#define FWLJMET_LJMet_interface_BaseEventSelector_h

/*
 Interface class for FWLite PAT analyzer-selectors
 Specific selectors must implement the () operator

 Author: Gena Kukartsev, 2010, 2012
         Orduna@2014
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility> // std::pair

#include "FWCore/Framework/interface/Event.h"

#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

#include "PhysicsTools/SelectorUtils/interface/EventSelector.h"

#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"


class BaseEventSelector : public Selector<edm::Event> {
    //
    // Base class for all event selector plugins
    //

    friend class LjmetFactory;

public:
    BaseEventSelector();
    virtual ~BaseEventSelector() { };
    virtual void BeginJob(const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC);
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret) = 0;
    virtual void EndJob();
    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec ) { }
    std::string GetName() { return mName; }

    // -----------------------------------------------------------------------------------------------------------------------------------------
    // Note: below probably needs to be recoded so it can be written in individual Selectors, but still accessible to different calculators -start
    // -----------------------------------------------------------------------------------------------------------------------------------------

    //Triggers
    std::vector<unsigned int>           const & GetSelTriggers()  const { return vSelTriggers; }
    std::map<std::string, unsigned int> const & GetSelectedTriggersEl()   const { return mvSelTriggersEl; }
    std::map<std::string, unsigned int> const & GetSelectedTriggersMu()   const { return mvSelTriggersMu; }
    std::map<std::string, unsigned int> const & GetSelectedTriggersHad()   const { return mvSelTriggersHad; }
    std::map<std::string, unsigned int> const & GetSelectedMCTriggersEl() const { return mvSelMCTriggersEl; }
    std::map<std::string, unsigned int> const & GetSelectedMCTriggersMu() const { return mvSelMCTriggersMu; }
    std::map<std::string, unsigned int> const & GetSelectedMCTriggersHad() const { return mvSelMCTriggersHad; }

    //Leptons
    std::vector<edm::Ptr<pat::Muon>>     const & GetSelMuons()     const { return vSelMuons; }
    std::vector<edm::Ptr<pat::Muon>>     const & GetSelLooseMuons()const { return vSelLooseMuons; }
    std::vector<edm::Ptr<pat::Electron>> const & GetSelElectrons()      const { return vSelElectrons; }
    std::vector<edm::Ptr<pat::Electron>> const & GetSelLooseElectrons() const { return vSelLooseElectrons; }
    std::vector<pat::Electron>           const & GetSelElectronsForLepJetClean() const { return vSelElectronsForJetLepClean; } // Used by DileptonEventSelector and DilepCalc. Probably this can be rewritten renamed/removed. -- June 4, 2010.

    //Jets
    std::vector<edm::Ptr<pat::Jet>>      const & GetAllJets()      const { return vAllJets; }
    std::vector<edm::Ptr<pat::Jet>>      const & GetSelJets()      const { return vSelJets; }
    std::vector<pat::Jet>                const & GetSelCorrJets()  const { return vSelCorrJets; }
    std::vector<edm::Ptr<pat::Jet>>      const & GetSelBtagJets()  const { return vSelBtagJets; }
    std::vector<std::pair<TLorentzVector, bool>>         const & GetSelCorrJetsWithBTags() const { return vSelCorrJetsWithBTags; }
    std::vector<pat::Jet>                const & GetSelCorrJetsAK8()  const { return vSelCorrJets_AK8; }
    std::vector<pat::Jet>                const & GetSelCleanedJets()      const { return vSelCleanedJets; } // Used by DileptonEventSelector and DilepCalc. Probably this can be rewritten renamed/removed. -- June 4, 2010.

    //MET
    edm::Ptr<pat::MET>                   const & GetMet()          const { return pMet; }
    TLorentzVector                       const & GetCorrectedMet() const { return correctedMET_p4; }

    //PV
    std::vector<edm::Ptr<reco::Vertex>>  const & GetSelPVs()       const { return vSelPVs; }

    // -----------------------------------------------------------------------------------------------------------------------------------------
    // Note: above probably needs to be recoded so it can be written in individual Selectors, but still accessible to different calculators - end
    // -----------------------------------------------------------------------------------------------------------------------------------------



    void SetMc(bool isMc) { mbIsMc = isMc; }
    bool IsMc() { return mbIsMc; }

    // LJMET event content setters
    void Init( void );
    void SetEventContent(LjmetEventContent * pEc) { mpEc = pEc; }

    /// Declare a new histogram to be created for the module
    void SetHistogram(std::string name, int nbins, double low, double high) { mpEc->SetHistogram(mName, name, nbins, low, high); }
    void SetHistValue(std::string name, double value) { mpEc->SetHistValue(mName, name, value); }
    void FillHist(std::string name, double value) { mpEc->FillHist(mName, name, value); }


protected:

    // -----------------------------------------------------------------------------------------------------------------------------------------
    // Note: below probably needs to be recoded so it can be written in individual Selectors, but still accessible to different calculators -start
    // -----------------------------------------------------------------------------------------------------------------------------------------

    //Triggers
    std::vector<unsigned int>            vSelTriggers;
    std::map<std::string, unsigned int> mvSelTriggersEl;
    std::map<std::string, unsigned int> mvSelTriggersMu;
    std::map<std::string, unsigned int> mvSelTriggersHad;
    std::map<std::string, unsigned int> mvSelMCTriggersEl;
    std::map<std::string, unsigned int> mvSelMCTriggersMu;
    std::map<std::string, unsigned int> mvSelMCTriggersHad;

    //Leptons
    std::vector<edm::Ptr<pat::Muon>>     vSelMuons;
    std::vector<edm::Ptr<pat::Muon>>     vSelLooseMuons;
    std::vector<edm::Ptr<pat::Electron>> vSelElectrons;
    std::vector<edm::Ptr<pat::Electron>> vSelLooseElectrons;
    std::vector<pat::Electron>           vSelElectronsForJetLepClean; // Used by DileptonEventSelector and DilepCalc. Probably this can be rewritten renamed/removed. -- June 4, 2010.

    //Jets
    std::vector<edm::Ptr<pat::Jet>>      vAllJets;
    std::vector<edm::Ptr<pat::Jet>>      vSelJets;
    std::vector<pat::Jet>                vSelCorrJets;
    std::vector<edm::Ptr<pat::Jet>>      vSelBtagJets;
    std::vector<std::pair<TLorentzVector, bool>> vSelCorrJetsWithBTags;
    std::vector<pat::Jet>                vSelCorrJets_AK8;
    std::vector<pat::Jet>                vSelCleanedJets; // Used by DileptonEventSelector and DilepCalc. Probably this can be rewritten renamed/removed. -- June 4, 2010.

    //MET
    edm::Ptr<pat::MET>     pMet;
    TLorentzVector         correctedMET_p4;

    //PV
    std::vector<edm::Ptr<reco::Vertex>>  vSelPVs;

    // -----------------------------------------------------------------------------------------------------------------------------------------
    // Note: above probably needs to be recoded so it can be written in individual Selectors, but still accessible to different calculators - end
    // -----------------------------------------------------------------------------------------------------------------------------------------


    std::string mName;
    std::string mLegend;
    bool mbIsMc;

private:
    LjmetEventContent * mpEc;

    /// Private init method to be called by LjmetFactory when registering the selector
    void init() { mLegend = "[" + mName + "]: "; std::cout << mLegend << "registering " << mName << std::endl; }
    void setName(std::string name) { mName = name; }

    /// Do what any event selector must do before event gets checked
    void BeginEvent(edm::EventBase const & event, LjmetEventContent & ec);

    /// Do what any event selector must do after event processing is done, but before event content gets saved to file
    void EndEvent(edm::EventBase const & event, LjmetEventContent & ec);

};

#endif
