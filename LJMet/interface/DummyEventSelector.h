#ifndef FWLJMET_LJMet_interface_DummyEventSelector_h
#define FWLJMET_LJMet_interface_DummyEventSelector_h

#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

using namespace std;

class DummyEventSelector : public BaseEventSelector {

public:

  
    DummyEventSelector();  
    ~DummyEventSelector();
  
  
    // executes before loop over events
    virtual void BeginJob(const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC);

    // main method where the cuts are applied
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );
    
    // executes after loop over events
    virtual void EndJob();

protected:
    std::string legend;
    bool bFirstEntry;

    // containers for config parameter values
    std::map<std::string,bool>           mbPar;
    std::map<std::string,int>            miPar;
    std::map<std::string,double>         mdPar;
    std::map<std::string,std::string>    msPar;
    std::map<std::string, edm::InputTag> mtPar;
    std::map<std::string,std::vector<std::string> > mvsPar;

    edm::Handle<edm::TriggerResults >           mhEdmTriggerResults;
    edm::Handle<std::vector<pat::Jet> >         mhJets;
    edm::Handle<std::vector<pat::Jet> >         mhJets_AK8;
    edm::Handle<std::vector<pat::Muon> >        mhMuons;
    edm::Handle<std::vector<pat::Electron> >    mhElectrons;
    edm::Handle<std::vector<pat::MET> >         mhMet;
    edm::Handle<std::vector<reco::PFMET> >      mhType1CorrMet;
    edm::Handle<double>                         h_rho;
    edm::Handle<std::vector<reco::Vertex> >     h_primVtx;

    std::vector<edm::Ptr<reco::Vertex> >  good_pvs_;



private:
  
    void initialize(std::map<std::string, edm::ParameterSet const> par);

};



#endif