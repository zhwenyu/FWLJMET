// -*- C++ -*-
//
// Package:    MCWeightInfo/WeightAnalyzer
// Class:      WeightAnalyzer
//
/**\class WeightAnalyzer WeightAnalyzer.cc MCWeightInfo/WeightAnalyzer/plugins/WeightAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Rizki Syarif
//         Created:  Fri, 10 May 2019 18:16:01 GMT
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
#include "FWCore/Utilities/interface/InputTag.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class WeightAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit WeightAnalyzer(const edm::ParameterSet&);
      ~WeightAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      
      std::vector<double> muRFvar;
      std::vector<double> pdfvar;
      
      int posweightsum = 0;
      int negweightsum = 0;
      int totalcount = 0;
      int zeroweightsum = 0;
      
      double weight = 1;
      double muRup = 1;
      double muRdn = 1;
      double muFup = 1;
      double muFdn = 1;
      double muRFup = 1;
      double muRFdn = 1;
      double Pdfup = 1;
      double Pdfdown = 1;


      // ----------member data ---------------------------
      edm::EDGetTokenT<GenEventInfoProduct> GEIPtoken;  
      edm::EDGetTokenT<LHEEventProduct> LHEEPtoken;
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
WeightAnalyzer::WeightAnalyzer(const edm::ParameterSet& iConfig) {
   //now do what ever initialization is needed
   edm::Service<TFileService> fs;

//   for(int i = 0; i < 7; i++){
//     muRFvar.push_back(0);
//   }
//   for(int i = 0; i < 101; i++){
//     pdfvar.push_back(0);
//   }

  edm::InputTag GEIPtag("generator");
  GEIPtoken = consumes<GenEventInfoProduct>(GEIPtag);

  edm::InputTag LHEEPtag("externalLHEProducer");
  LHEEPtoken = consumes<LHEEventProduct>(LHEEPtag);


}


WeightAnalyzer::~WeightAnalyzer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
WeightAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

  using namespace std;

  edm::Handle<GenEventInfoProduct> genEvtInfo;
  iEvent.getByToken(GEIPtoken,genEvtInfo);

  weight = 1.0;
  weight = genEvtInfo->weight()/fabs(genEvtInfo->weight());

  // Count
  double countweight = genEvtInfo->weight();
  if(countweight < 0) negweightsum++;
  else if(countweight > 0) posweightsum++;
  else zeroweightsum++;

  totalcount++;

//   std::vector<double> LHEweights;
//   std::vector<int> LHEweightids;
// 
//   edm::Handle<LHEEventProduct> EvtHandle;
//   if(iEvent.getByToken(LHEEPtoken,EvtHandle)){
//     // Storing LHE weights https://twiki.cern.ch/twiki/bin/viewauth/CMS/LHEReaderCMSSW
//     // for MC@NLO renormalization and factorization scale. 
//     // ID numbers 1001 - 1009. (muR,muF) = 
//     // 0 = 1001: (1,1)    3 = 1004: (2,1)    6 = 1007: (0.5,1)  
//     // 1 = 1002: (1,2)    4 = 1005: (2,2)  	 7 = 1008: (0.5,2)  
//     // 2 = 1003: (1,0.5)  5 = 1006: (2,0.5)	 8 = 1009: (0.5,0.5)
//     // for PDF variations: ID numbers > 2000
//     
//     std::string weightidstr;
//     int weightid;
//     if(EvtHandle->weights().size() > 0){	  
//       for(unsigned int i = 0; i < EvtHandle->weights().size(); i++){
//       	weightidstr = EvtHandle->weights()[i].id;
//       	weightid = std::stoi(weightidstr);
//       	LHEweights.push_back(EvtHandle->weights()[i].wgt/EvtHandle->originalXWGTUP());
//       	LHEweightids.push_back(weightid);
//       }
//     }
//   }

  //PDF and RENORM weights
//   std::vector<double> renorm;
//   std::vector<double> pdf;
//   if(isX53){
//     for(unsigned int i = 0; i < LHEweightids.size(); i++){
//       if(LHEweightids.at(i) > 1 && LHEweightids.at(i) < 10){
// 	if(LHEweightids.at(i) == 6 || LHEweightids.at(i) == 8) continue;
// 	renorm.push_back(LHEweights.at(i));
//       }
//       if(LHEweightids.at(i) > 111 && LHEweightids.at(i) < 212){
// 	pdf.push_back(LHEweights.at(i));
//       }
//     }
//   }
//   else if(isWJ){
//     for(unsigned int i = 0; i < LHEweightids.size(); i++){
//       if(LHEweightids.at(i) > 1 && LHEweightids.at(i) < 10){
// 	if(LHEweightids.at(i) == 6 || LHEweightids.at(i) == 8) continue;
// 	renorm.push_back(LHEweights.at(i));
//       }
//       if(LHEweightids.at(i) > 10 && LHEweightids.at(i) < 111){
// 	pdf.push_back(LHEweights.at(i));
//       }
//     }
//   }
//   else{
//     for(unsigned int i = 0; i < LHEweightids.size(); i++){
//       if(LHEweightids.at(i) > 1001 && LHEweightids.at(i) < 1010){
//       	if(LHEweightids.at(i) == 1006 || LHEweightids.at(i) == 1008) continue;
//       	renorm.push_back(LHEweights.at(i));
//       }
//       if(LHEweightids.at(i) > 2000 && LHEweightids.at(i) < 2101){
//       	pdf.push_back(LHEweights.at(i));
//       }
//     }
//   }

//   muRFvar.at(0) += weight;
//   if(renorm.size() < 6) cout << "Didn't get all the renorm weights!" << endl;
//   else{
//     muRFvar.at(1) += weight*renorm.at(2);
//     muRFvar.at(2) += weight*renorm.at(4);
//     muRFvar.at(3) += weight*renorm.at(0);
//     muRFvar.at(4) += weight*renorm.at(1);
//     muRFvar.at(5) += weight*renorm.at(3);
//     muRFvar.at(6) += weight*renorm.at(5);
//   }
// 
//   pdfvar.at(0) += weight;
//   if(pdf.size() < 100) cout << "Didn't get all the PDF weights!" << endl;
//   else{
//     for(int i = 0; i < 100; i++){
//       pdfvar.at(i+1) += weight*pdf.at(i);
//     }
//   }



}


// ------------ method called once each job just before starting event loop  ------------
void
WeightAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
WeightAnalyzer::endJob()
{
  printf("\n-------- MC Weight Analyzer --------\n");
  printf("Total events processed = %i\n",totalcount);
  printf("Positive weight = %i\n",posweightsum);
  printf("Negative weight = %i\n",negweightsum);
  printf("Zero weight = %i\n",zeroweightsum);
  printf("Adjusted count = %i\n",posweightsum - negweightsum);
  printf(  "----------------------------------------\n");

//   double nominal = muRFvar.at(0);
//   
//   printf("Nominal yield = %f\n",nominal);
// 
//   std::sort(muRFvar.begin(),muRFvar.end());
//   std::sort(pdfvar.begin(),pdfvar.end());
// 
//   double muRFup = muRFvar.at(6);
//   double muRFdn = muRFvar.at(0);
//   double pdfup = pdfvar.at(83);
//   double pdfdn = pdfvar.at(15);
// 
//   printf("MUup yield = %f\n",muRFup);
//   printf("MUdn yield = %f\n",muRFdn);
//   printf("PDFup yield = %f\n",pdfup);
//   printf("PDFdn yield = %f\n",pdfdn);
// 
//   printf("\n --- Yield Quantiles method --- \n");
// 
//   printf("MUup scale factor = %f\n",nominal/muRFup);
//   printf("MUdn scale factor = %f\n",nominal/muRFdn);
//   printf("PDFup scale factor = %f\n",nominal/pdfup);
//   printf("PDFdn scale factor = %f\n",nominal/pdfdn);


}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
WeightAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(WeightAnalyzer);
