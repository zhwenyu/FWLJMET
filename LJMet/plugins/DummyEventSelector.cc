#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

using namespace std;

class DummyEventSelector : public BaseEventSelector {

public:

  
    DummyEventSelector();  
    ~DummyEventSelector();
  
  
    // executes before loop over events
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par);

    // main method where the cuts are applied
    virtual bool operator()( edm::EventBase const & event, pat::strbitset & ret);

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



static int reg = LjmetFactory::GetInstance()->Register(new DummyEventSelector(), "DummySelector");

DummyEventSelector::DummyEventSelector()
{
}

DummyEventSelector::~DummyEventSelector()
{
}

void DummyEventSelector::BeginJob( std::map<std::string, edm::ParameterSet const> par)
{
    BaseEventSelector::BeginJob(par);

    std::string _key;

//     _key = "event_selector";
//     if ( par.find(_key)!=par.end() ){
//         mbPar["debug"]                    = par[_key].getParameter<bool>         ("debug");
//     }   
//     else {
//         std::cout << mLegend << "event selector not configured, exiting"
//                   << std::endl;
//         std::exit(-1);
//     }
  
    std::cout << mLegend << "initializing Dummy selection" << std::endl;

    bFirstEntry = true;
    
    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    push_back("No selection");  
    push_back("Cut A");
    push_back("All cuts");

    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    set("No selection",true);
    set("Cut A",true);
    set("All cuts",true);

    
   
} // end of BeginJob() 

bool DummyEventSelector::operator()( edm::EventBase const & event, pat::strbitset & ret)
{

	std::cout<< " " <<std::endl; // DEBUG - rizki
	std::cout<< "=====================================" <<std::endl; 
	std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
	std::cout<< "=====================================" <<std::endl; 
	std::cout<< " " <<std::endl; // DEBUG - rizki

    while(1){ // standard infinite while loop trick to avoid nested ifs
    
        passCut(ret, "No selection");
        
        passCut(ret, "Cut A");

        passCut(ret, "All cuts");
        break;

    } // end of while loop


    bFirstEntry = false;
    
    return (bool)ret;
  
    setIgnored(ret);

    return false;
}// end of operator()


void DummyEventSelector::AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec )
{
    //
    // Compute analysis-specific quantities in the event,
    // return via event content
    //

//   ec.SetValue("pi", 3.14);
  

    return;
}

void DummyEventSelector::EndJob()
{
}
