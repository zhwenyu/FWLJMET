#include "FWLJMET/LJMet/interface/DummyEventSelector.h"


DummyEventSelector::DummyEventSelector()
{
}

DummyEventSelector::~DummyEventSelector()
{
}

void DummyEventSelector::BeginJob( const edm::ParameterSet& iConfig, edm::ConsumesCollector && iC)
{
    BaseEventSelector::BeginJob(iConfig,(edm::ConsumesCollector &&)iC);

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

    bFirstEntry = true; //in case anything needs a first entry bool.
    
    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    push_back("No selection");  
    push_back("Cut A");
    push_back("All cuts");

    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    set("No selection",true);
    set("Cut A",true);
    set("All cuts",true);

    
   
} // end of BeginJob() 

bool DummyEventSelector::operator()( edm::Event const & event, pat::strbitset & ret)
{
	std::cout << "Processing Event in DummyEventSelector::operator()" << std::endl;
	std::cout << "=====================================" <<std::endl; 
	std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
	std::cout << "=====================================" <<std::endl; 

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
