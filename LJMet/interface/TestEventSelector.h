#ifndef FWLJMET_LJMet_interface_TestEventSelector_h
#define FWLJMET_LJMet_interface_TestEventSelector_h

#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

using namespace std;

class TestEventSelector : public BaseEventSelector {

public:


    TestEventSelector();
    ~TestEventSelector();


    // executes before loop over events
    virtual void BeginJob(const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC);

    // main method where the cuts are applied
    virtual bool operator()( edm::Event const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );

    // executes after loop over events
    virtual void EndJob();

protected:

    // containers for config parameter values
    //std::map<std::string,std::string>    msPar;

    std::string legend;
    bool bFirstEntry;
    bool debug;

    edm::EDGetTokenT<edm::TriggerResults>       triggersToken;
    edm::EDGetTokenT<pat::MuonCollection>       muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection>   electronsToken;

    int       minLeptons;
    float     min_muPt;
    float     max_muEta;
    float     min_elPt;
    float     max_elEta;

    std::vector<std::string>  vTargetTrigs;

    bool TriggerSelection(edm::Event const & event);
    bool LeptonsSelection(edm::Event const & event);


private:


};




#endif