// -*- C++ -*-
//
// Package:    FWLJMET/LJMet
// Class:      LJMet
//
/**\class LJMet LJMet.cc FWLJMET/LJMet/plugins/LJMet.cc

 Description: [Full Framework LJMET]

 Implementation:
     [Much of what has been implemented here is based on https://github.com/cms-ljmet/Ljmet-Com/blob/CMSSW_9_4_X/bin/ljmet.cc ]
*/
//
// Original Author:  Rizki Syarif
//         Created:  Wed, 27 Feb 2019 00:47:45 GMT
//
//


// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "PhysicsTools/SelectorUtils/interface/strbitset.h"


#include "TTree.h"

// Adding TFile service stuff -- https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTFileService, https://github.com/cms-sw/cmssw/blob/CMSSW_9_4_X/FWCore/Skeletons/scripts/mkTemplates/EDAnalyzer/EDAnalyzer.cc
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWLJMET/LJMet/interface/LjmetEventContent.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/BaseEventSelector.h"


//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class LJMet : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit LJMet(const edm::ParameterSet&);
      ~LJMet();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------

      TTree * _tree;

      // harvest all parameter sets from config file
      std::map<std::string, edm::ParameterSet const> mPar;

      // internal LJMet event content
      LjmetEventContent ec;

      // The factory for event selector and calculator plugins
      LjmetFactory * factory;

      // choose event selector
      BaseEventSelector * theSelector = 0;


      bool debug;
      int verbosity;
      std::string selection;
      std::vector<std::string> vExcl;
      std::vector<std::string> vIncl;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
LJMet::LJMet(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

   debug      = iConfig.getParameter<bool>("debug"); //this is debug feature from new ljmet, only on and off.
   verbosity  = iConfig.getParameter<int>("verbosity"); // this is debug feature from old ljmet. this has levels, which is better. Need to utilize both old and new ! #TODO.
   selection  = iConfig.getParameter<std::string>("selector");
   vExcl      = iConfig.getParameter<std::vector<std::string>>("exclude_calcs");
   vIncl      = iConfig.getParameter<std::vector<std::string>>("include_calcs");


   usesResource("TFileService"); // came originally with EDAnalyzer

   edm::Service<TFileService> fs; //for purpose of creating / saving to root file

   // output tree
   std::cout << "[FWLJMet] : " << "Creating output tree" << std::endl;
   std::string const _treename = "ljmet";
   _tree = fs->make<TTree>(_treename.c_str(), _treename.c_str(), 64000000);

   // internal LJMet event content
   ec.SetVerbosity(verbosity);
   ec.SetTree(_tree);

   // The factory for event selector and calculator plugins
   factory = LjmetFactory::GetInstance();

   // choose event selector
   std::cout << "[FWLJMet] : " << "instantiating the event selector" << std::endl;
   theSelector = factory->GetEventSelector(selection);

   // sanity check histograms from the selector
   theSelector->SetEventContent(&ec);
   theSelector->Init();

   //Object to pass to eventSelector and Calculators access data - https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideEDMGetDataFromEvent#Consumes_and_Helpers
   edm::ConsumesCollector && cC = consumesCollector(); 

   theSelector->BeginJob(iConfig, (edm::ConsumesCollector &&)cC);
   
   //print out included Calculators
   for (std::vector<std::string>::const_iterator it = vIncl.begin(); it != vIncl.end(); ++it){
      std::cout << "[FWLJMet] : " << "including " << *it <<std::endl;
   }

   // send config parameters to calculators
   factory->SetAllCalcConfig(iConfig, vIncl);

   // Run BeginJob() for calculators
   factory->BeginJobAllCalc((edm::ConsumesCollector &&)cC, vIncl);

   // set excluded calculators
   factory->SetExcludedCalcs(vExcl); 
   


}


LJMet::~LJMet()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

    delete theSelector;

    delete factory;

}


//
// member functions
//

// ------------ method called for each event  ------------
void
LJMet::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	if(debug) std::cout << " " <<std::endl;
	if(debug) std::cout << "Processing Event in FWLJMet::analyze" << std::endl;

	//
	//_____ Run private begin-of-event methods ___________________
	//
	factory->RunBeginEvent(iEvent, ec);


	// run producers
	factory->RunAllProducers(iEvent, theSelector, vIncl); 

	// event selection
	pat::strbitset ret = theSelector->getBitTemplate();
	bool passed = (*theSelector)( iEvent, ret );


	if ( passed ) {

		//
		//_____ Run all variable calculators now ___________________
		//
		factory->RunAllCalculators(iEvent, theSelector, ec, vIncl);


		//
		//_____ Run selector-specific code if any___________________
		//
		theSelector->AnalyzeEvent(iEvent, ec);


		//
		//_____ Run private end-of-event methods ___________________
		//
		factory->RunEndEvent(iEvent, ec);


		//
		//_____Fill output file ____________________________________
		//
		ec.Fill();

	} // end if statement for final cut requirements

}


// ------------ method called once each job just before starting event loop  ------------
void
LJMet::beginJob()
{

}

// ------------ method called once each job just after ending the event loop  ------------
void
LJMet::endJob()
{
    if(debug) std::cout << " " <<std::endl;
    if(debug) std::cout << "[FWLJMet] : " << "Selection" << std::endl;
    theSelector->print(std::cout);


    // Run EndJob() for calculators
    factory->EndJobAllCalc(vIncl);


    // EndJob() for the selector
    theSelector->EndJob();

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
LJMet::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(LJMet);
