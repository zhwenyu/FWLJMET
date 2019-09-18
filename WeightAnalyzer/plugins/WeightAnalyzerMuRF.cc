// -*- C++ -*-
//
// Package:    FWLJMET/WeightAnalyzerMuRF
// Class:      WeightAnalyzerMuRF
//
/**\class WeightAnalyzerMuRF WeightAnalyzerMuRF.cc FWLJMET/WeightAnalyzerMuRF/plugins/WeightAnalyzerMuRF.cc

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


class WeightAnalyzerMuRF : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit WeightAnalyzerMuRF(const edm::ParameterSet&);
  ~WeightAnalyzerMuRF();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  
  TH1D* weightHist;
  TH1D* NumTrueHist;
  TH1D* pdfcounts;
  TH1D* muRFcounts;

  bool orlhew = false;
  int adjustedcount = 0;
  double pdfweightsum = 0;
  std::string basePDFname = "NNPDF31_nnlo_as_0118_nf_4";
  std::string newPDFname = "NNPDF31_nnlo_as_0118_nf_4_mc_hessian";

  LHAPDF::PDF* basepdf1;
  LHAPDF::PDFSet newset;
  std::vector<LHAPDF::PDF*> newpdfs;
  
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
WeightAnalyzerMuRF::WeightAnalyzerMuRF(const edm::ParameterSet& iConfig):
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

  //Make histograms to save counts
  std::cout << "[WeightAnalyzerMuRF] : Creating histograms " << std::endl;
  weightHist = fs->make<TH1D>("weightHist","MC Weight",10,-5,5); weightHist->Sumw2();
  pdfcounts = fs->make<TH1D>("pdfcounts","pdfcounts",101,0,101);
  muRFcounts = fs->make<TH1D>("muRFcounts","muRFcounts",7,0,7);

  //Initialize PDF sets
  basepdf1 = LHAPDF::mkPDF(basePDFname,0);
  newset = LHAPDF::PDFSet(newPDFname);
  newpdfs = newset.mkPDFs();

}


WeightAnalyzerMuRF::~WeightAnalyzerMuRF()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
WeightAnalyzerMuRF::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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

  std::vector<double> LHEweights;
  std::vector<int> LHEweightids;
  
  edm::Handle<LHEEventProduct> EvtHandle;
  if(iEvent.getByToken(LHEEPtoken,EvtHandle)){

    // Storing LHE weights https://twiki.cern.ch/twiki/bin/viewauth/CMS/LHEReaderCMSSW
    // for MC@NLO renormalization and factorization scale. 
    // ID numbers 1001 - 1009. (muR,muF) = 
    // 0 = 1001: (1,1)    3 = 1004: (2,1)    6 = 1007: (0.5,1)  
    // 1 = 1002: (1,2)    4 = 1005: (2,2)  	 7 = 1008: (0.5,2)  
    // 2 = 1003: (1,0.5)  5 = 1006: (2,0.5)	 8 = 1009: (0.5,0.5)
    // for PDF variations: ID numbers > 2000
    
    std::string weightidstr;
    int weightid;
    if(EvtHandle->weights().size() > 0){	  
      for(unsigned int i = 0; i < EvtHandle->weights().size(); i++){
	weightidstr = EvtHandle->weights()[i].id;
	weightid = std::stoi(weightidstr);
	LHEweights.push_back(EvtHandle->weights()[i].wgt/EvtHandle->originalXWGTUP());
	LHEweightids.push_back(weightid);
      }
    }
  }

  if (orlhew) {

    float newweight = 1;    

    // PDF adjustment for signals     
    float x1 = genEvtInfo->pdf()->x.first;
    float x2 = genEvtInfo->pdf()->x.second;
    double Q = genEvtInfo->pdf()->scalePDF;
    int id1 = genEvtInfo->pdf()->id.first;
    int id2 = genEvtInfo->pdf()->id.second;
        
    // calculate central PDFs for generator set,
    const LHAPDF::GridPDF& pdf1 = * dynamic_cast<const LHAPDF::GridPDF*>(basepdf1);

    double pdf1_gen = pdf1.xfxQ(id1, x1, Q);
    double pdf2_gen = pdf1.xfxQ(id2, x2, Q);
    //std::cout<<"pdf1_gen = "<<pdf1_gen<<" pdf2_gen = "<<pdf2_gen<<std::endl;

    for (size_t i = 0; i<newset.size(); i++) {
      const LHAPDF::GridPDF& pdf2 = * dynamic_cast<const LHAPDF::GridPDF*>(newpdfs[i]);
            
      double pdf1_new = pdf2.xfxQ(id1, x1, Q);
      double pdf2_new = pdf2.xfxQ(id2, x2, Q);
      LHEweights.push_back((pdf1_new*pdf2_new)/(pdf1_gen*pdf2_gen));
      LHEweightids.push_back(315000+i);

      if(i == 0){
	newweight = (pdf1_new*pdf2_new)/(pdf1_gen*pdf2_gen);
	newweight *= genEvtInfo->weight(); // (new/old)*old = new. For signal genEvtInfo->weight() is +/- 1
	pdfweightsum += newweight;	  
      }      
    }  

    //PDF and RENORM weights
    std::vector<double> renorm;
    std::vector<double> pdf;

    for(unsigned int i = 0; i < LHEweightids.size(); i++){
      if(LHEweightids.at(i) > 1 && LHEweightids.at(i) < 10){
	if(LHEweightids.at(i) == 6 || LHEweightids.at(i) == 8) continue;
	renorm.push_back(LHEweights.at(i));
      }
      if(LHEweightids.at(i) > 315000 && LHEweightids.at(i) < 315101){
	pdf.push_back(LHEweights.at(i));
      }
    }

    // positive definite == (old weight)*(new pdf0 / old pdf)
    // for renorm, treat like multiplier on new weight
    // for PDF shifts, it appears that (old weight)*(new pdfN / old pdf) is the right weight
    muRFcounts->Fill(0.5,newweight);  
    if(renorm.size() < 6) cout << "Didn't get all the renorm weights!" << endl;
    else{
      muRFcounts->Fill(1.5,newweight*renorm.at(2));
      muRFcounts->Fill(2.5,newweight*renorm.at(4));
      muRFcounts->Fill(3.5,newweight*renorm.at(0));
      muRFcounts->Fill(4.5,newweight*renorm.at(1));
      muRFcounts->Fill(5.5,newweight*renorm.at(3));
      muRFcounts->Fill(6.5,newweight*renorm.at(5));
    }

    pdfcounts->Fill(0.5,newweight);
    if(pdf.size() < 100) cout << "Didn't get all the PDF weights!" << endl;
    else{
      for(int i = 0; i < 100; i++) pdfcounts->Fill(i+1.5,genEvtInfo->weight()*pdf.at(i));
    }
  }
  else pdfweightsum += genEvtInfo->weight();
}


// ------------ method called once each job just before starting event loop  ------------
void
WeightAnalyzerMuRF::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
WeightAnalyzerMuRF::endJob()
{
  delete basepdf1;
  for (size_t i = 0; i<newset.size(); i++) {
    delete newpdfs[i];
  }

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
WeightAnalyzerMuRF::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
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
DEFINE_FWK_MODULE(WeightAnalyzerMuRF);
