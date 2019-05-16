// -*- C++ -*-
//
// Package:    FWLJMET/WeightAnalyzer
// Class:      WeightAnalyzer
//
/**\class WeightAnalyzer WeightAnalyzer.cc FWLJMET/WeightAnalyzer/plugins/WeightAnalyzer.cc

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
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/GridPDF.h"
#include "LHAPDF/Info.h"
#include "LHAPDF/Config.h"
#include "LHAPDF/PDFInfo.h"
#include "LHAPDF/PDFSet.h"
#include "LHAPDF/Factories.h"

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
  
  TH1D* weightHist;
  TH1D* NumTrueHist;

  bool orlhew = false;
  int adjustedcount = 0;
  double pdfweightsum = 0;
  std::string basePDFname = "NNPDF31_nnlo_as_0118_nf_4";
  std::string newPDFname = "NNPDF31_nnlo_as_0118_nf_4_mc_hessian";
  
  // ----------member data ---------------------------
  edm::EDGetTokenT<GenEventInfoProduct> GEIPtoken;
  edm::EDGetTokenT<LHEEventProduct> LHEEPtoken;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo>> APItoken;

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
WeightAnalyzer::WeightAnalyzer(const edm::ParameterSet& iConfig):
  orlhew(iConfig.getParameter<bool>("overrideLHEweight")),
  basePDFname(iConfig.getParameter<std::string>("basePDFname")),
  newPDFname(iConfig.getParameter<std::string>("newPDFname"))
{
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;  
  
  edm::InputTag GEIPtag("generator");
  GEIPtoken = consumes<GenEventInfoProduct>(GEIPtag);

  edm::InputTag LHEEPtag("externalLHEProducer");
  LHEEPtoken = consumes<LHEEventProduct>(LHEEPtag);

  edm::InputTag APItag("slimmedAddPileupInfo");
  APItoken = consumes<std::vector<PileupSummaryInfo>>(APItag);

  //Make histograms to save counts
  std::cout << "[WeightAnalyzer] : Creating histograms " << std::endl;
  NumTrueHist = fs->make<TH1D>("NumTrueHist","N True Interactions",100,0,100); NumTrueHist->Sumw2();
  weightHist = fs->make<TH1D>("weightHist","MC Weight",10,-5,5); weightHist->Sumw2();

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

  // Basic +/- event weight
  edm::Handle<GenEventInfoProduct> genEvtInfo;
  iEvent.getByToken(GEIPtoken, genEvtInfo);
  
  double weight = 1.0;
  if(genEvtInfo->weight() < 0) weight = -1;
  weightHist->Fill(weight); // bin at -1 filled with neg, bin at +1 filled with pos. 

  adjustedcount += weight; // sum up the +/- weights

  // Pileup hist
  edm::Handle<std::vector<PileupSummaryInfo>> PupInfo;
  iEvent.getByToken(APItoken, PupInfo);

  int NumTrueInts = -1;
  for(std::vector<PileupSummaryInfo>::const_iterator PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI){
    int BX = PVI->getBunchCrossing();
    if(BX == 0) NumTrueInts = PVI->getTrueNumInteractions();
  }
  NumTrueHist->Fill(NumTrueInts,weight);

  // PDF adjustment for signals     
  if (orlhew) {
    
    float x1 = genEvtInfo->pdf()->x.first;
    float x2 = genEvtInfo->pdf()->x.second;
    double Q = genEvtInfo->pdf()->scalePDF;
    int id1 = genEvtInfo->pdf()->id.first;
    int id2 = genEvtInfo->pdf()->id.second;

    //Initialize PDF sets
    LHAPDF::PDF* basepdf = LHAPDF::mkPDF(basePDFname,0);
    const LHAPDF::GridPDF& pdf1 = * dynamic_cast<const LHAPDF::GridPDF*>(basepdf);
    LHAPDF::PDF* newpdf = LHAPDF::mkPDF(newPDFname,0);
    const LHAPDF::GridPDF& pdf2 = * dynamic_cast<const LHAPDF::GridPDF*>(newpdf);
    
    // calculate central PDFs for generator set,
    double pdf1_gen = pdf1.xfxQ(id1, x1, Q);
    double pdf2_gen = pdf1.xfxQ(id2, x2, Q);
    delete basepdf;
  
    // calculate central PDFs for new set,
    double pdf1_new = pdf2.xfxQ(id1, x1, Q);
    double pdf2_new = pdf2.xfxQ(id2, x2, Q);
    delete newpdf;

    double newweight = (pdf1_new*pdf2_new)/(pdf1_gen*pdf2_gen); // new/old
    newweight *= genEvtInfo->weight(); // (new/old)*old = new, for consistency with singleLepCalc
    
    pdfweightsum += newweight;

  }else{
    pdfweightsum += genEvtInfo->weight();
  }
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
  printf("Total events processed = %f\n",weightHist->Integral());
  printf("Adjusted count = %i\n",adjustedcount);
  printf("New PDF sum-of-weights = %f\n",pdfweightsum);
  printf(  "------------------------------------\n");
  
  //Save values in histogram
  weightHist->SetBinContent(1,adjustedcount); // bin at -5 filled with basic adjusted count
  weightHist->SetBinContent(2,pdfweightsum); // bin at -4 filled with pdf adjusted sum

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
