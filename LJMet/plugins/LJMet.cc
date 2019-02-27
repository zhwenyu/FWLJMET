// -*- C++ -*-
//
// Package:    FWLJMET/LJMet
// Class:      LJMet
// 
/**\class LJMet LJMet.cc FWLJMET/LJMet/plugins/LJMet.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
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


#include "TTree.h"

// Adding TFile service stuff -- https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTFileService, https://github.com/cms-sw/cmssw/blob/CMSSW_9_4_X/FWCore/Skeletons/scripts/mkTemplates/EDAnalyzer/EDAnalyzer.cc
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWLJMET/LJMet/interface/LjmetEventContent.h"
// #include "FWLJMET/LJMet/interface/LjmetFactory.h"
// #include "FWLJMET/LJMet/interface/BaseEventSelector.h"


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
   usesResource("TFileService");
   
   edm::Service<TFileService> fs;  
   
   // output tree
   std::cout << "FWLJMet : " << "Creating output tree" << std::endl;
   std::string const _treename = "ljmet";
   _tree = fs->make<TTree>(_treename.c_str(), _treename.c_str(), 64000000);

}


LJMet::~LJMet()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
LJMet::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   
   std::cout << "Processing Event in FWLJMet::analyze" << std::endl;
   
   // internal LJMet event content
   LjmetEventContent ec;
   ec.SetTree(_tree);

   //_tree->Fill();
   ec.Fill();
   

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
