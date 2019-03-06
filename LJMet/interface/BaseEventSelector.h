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
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par);
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par, edm::ConsumesCollector && iC);
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret) = 0;
    virtual void EndJob();
    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec ) { }
    std::string GetName() { return mName; }
    
    std::vector<unsigned int> const & GetSelTriggers() const { return mvSelTriggers; }
    std::vector<edm::Ptr<pat::Muon>> const & GetSelMuons() const { return mvSelMuons; }
    std::vector<edm::Ptr<pat::Electron>> const & GetSelElectrons() const { return mvSelElectrons; }
    std::vector<edm::Ptr<pat::Jet>> const & GetSelJets() const { return mvSelJets; }
    edm::Ptr<pat::MET> const & GetMet() const { return mpMet; }
    std::vector<edm::Ptr<reco::Vertex>> const & GetSelPVs() const { return mvSelPVs; }
    void SetMc(bool isMc) { mbIsMc = isMc; }
    bool IsMc() { return mbIsMc; }
    
    // LJMET event content setters
    void Init( void );
    void SetEventContent(LjmetEventContent * pEc) { mpEc = pEc; }

    /// Declare a new histogram to be created for the module
    void SetHistogram(std::string name, int nbins, double low, double high) { mpEc->SetHistogram(mName, name, nbins, low, high); }
    void SetHistValue(std::string name, double value) { mpEc->SetHistValue(mName, name, value); }
            
protected:
    std::vector<unsigned int> mvSelTriggers;
    std::vector<edm::Ptr<pat::Muon>> mvSelMuons;
    std::vector<edm::Ptr<pat::Electron>> mvSelElectrons;
    std::vector<edm::Ptr<pat::Jet>> mvSelJets;
    edm::Ptr<pat::MET> mpMet;
    std::vector<edm::Ptr<reco::Vertex>> mvSelPVs;
    
    // containers for config parameter values
    std::map<std::string, bool> mbPar;
    std::map<std::string, int> miPar;
    std::map<std::string, double> mdPar;
    std::map<std::string, std::vector<double>> mvdPar;
    std::map<std::string, std::string> msPar;
    std::map<std::string, edm::InputTag> mtPar;
    std::map<std::string, std::vector<std::string>> mvsPar;

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
