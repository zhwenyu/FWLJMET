#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

using namespace std;

class TestEventSelector : public BaseEventSelector {

public:


    TestEventSelector();
    ~TestEventSelector();


    // executes before loop over events
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par);
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par, edm::ConsumesCollector && iC);

    // main method where the cuts are applied
    virtual bool operator()( edm::EventBase const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );

    // executes after loop over events
    virtual void EndJob();

protected:
    std::string legend;
    bool bFirstEntry;

    // containers for config parameter values
    //std::map<std::string,std::string>    msPar;

    edm::EDGetTokenT<edm::TriggerResults> 		triggersToken;
    std::vector<std::string>			        vTargetTrigs;

    bool						debug;



private:



};



static int reg = LjmetFactory::GetInstance()->Register(new TestEventSelector(), "TestSelector");

TestEventSelector::TestEventSelector()
{
}

TestEventSelector::~TestEventSelector()
{
}

void TestEventSelector::BeginJob( std::map<std::string, edm::ParameterSet const> par, edm::ConsumesCollector && iC)
{
    std::cout << mLegend << "initializing Test selection" << std::endl;

    debug = true;

    BaseEventSelector::BeginJob(par);

    triggersToken		= iC.consumes<edm::TriggerResults>(edm::InputTag("TriggerResults::HLT"));
    //vTargetTrigs			= iConfig.getParameter<std::vector<std::string>>("HLTpaths");
    vTargetTrigs.push_back("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
    vTargetTrigs.push_back("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");
    vTargetTrigs.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
    vTargetTrigs.push_back("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v");

    bFirstEntry = true; //in case anything needs a first entry bool.

    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    push_back("No selection");
    push_back("Trigger");
    push_back("All cuts");

    //See "PhysicsTools/SelectorUtils/interface/EventSelector.h"
    set("No selection",true);
    set("Trigger",true);
    set("All cuts",true);


} // end of BeginJob()

bool TestEventSelector::operator()( edm::EventBase const & event, pat::strbitset & ret)
{
  std::cout << "Processing Event in TestEventSelector::operator()" << std::endl;
  std::cout << "=====================================" <<std::endl;
  std::cout << "Event = " << event.id().event() << ", Lumi Block = " << event.id().luminosityBlock() << std::endl;
  std::cout << "=====================================" <<std::endl;

  while(1){ // standard infinite while loop trick to avoid nested ifs

    passCut(ret, "No selection");

    edm::Handle< edm::TriggerResults > triggersHandle;
    event.getByToken(triggersToken,triggersHandle);
    //event.getByLabel("TriggerResults::HLT", triggersHandle);
    const edm::TriggerNames trigNames = event.triggerNames(*triggersHandle);

    unsigned int nTrig = triggersHandle->size();
    bool passTrig = false;
    for(std::vector<std::string>::const_iterator targetTrig = vTargetTrigs.begin(); targetTrig!= vTargetTrigs.end(); targetTrig++){

      for(unsigned int i=0; i<nTrig;i++){

	std::string trigName = trigNames.triggerName(i);
	if(trigName.find((*targetTrig)) == std::string::npos) continue;

	if(triggersHandle->accept(trigNames.triggerIndex(trigName))){

	  if(debug)std::cout << "\tFIRED : "<< (*targetTrig) << std::endl;

	  passTrig = true;

	}

      }

    }

    if(passTrig)passCut(ret, "Trigger");
    else break;

    passCut(ret, "All cuts");
    break;

  } // end of while loop


  bFirstEntry = false;

  return (bool)ret;

  setIgnored(ret);

  return false;
}// end of operator()


void TestEventSelector::AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec )
{
  //
  // Compute analysis-specific quantities in the event,
  // return via event content
  //

  //   ec.SetValue("pi", 3.14);


  return;
}

void TestEventSelector::EndJob()
{
}
