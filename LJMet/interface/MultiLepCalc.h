#ifndef FWLJMET_LJMet_interface_MultiLepCalc_h
#define FWLJMET_LJMet_interface_MultiLepCalc_h

#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "FWLJMET/LJMet/interface/MiniIsolation.h"

#include "FWLJMET/LJMet/interface/JetMETCorrHelper.h"
#include "FWLJMET/LJMet/interface/BTagSFUtil.h"


#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/GridPDF.h"
#include "LHAPDF/Info.h"
#include "LHAPDF/Config.h"
#include "LHAPDF/PDFInfo.h"
#include "LHAPDF/PDFSet.h"
#include "LHAPDF/Factories.h"

class LjmetFactory;

class MultiLepCalc : public BaseCalc {
public:
    MultiLepCalc();
    virtual ~MultiLepCalc();
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;};

    void AnalyzeTriggers(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzePV(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzePU(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzeBadDupMu(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzeMuon(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzeElectron(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzeJets(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzeAK8Jets(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzeMET(edm::Event const & event, BaseEventSelector * selector);
    void AnalyzeGenInfo(edm::Event const & event, BaseEventSelector * selector);


private:
    bool debug;
    bool isMc;
    bool saveLooseLeps;
    bool keepFullMChistory;
    bool UseElMVA;
    bool UseElIDV1;

    bool doNewJEC;
    bool   JECup;
    bool   JECdown;
    bool   JERup;
    bool   JERdown;
    bool doAllJetSyst;
    JetMETCorrHelper JetMETCorr;

    BTagSFUtil btagSfUtil;

    bool saveGenHT;
    bool orlhew;
    std::string basePDFname;
    std::string newPDFname;
    std::vector<unsigned int> keepPDGID;
    std::vector<unsigned int> keepMomPDGID;
    std::vector<unsigned int> keepPDGIDForce;
    std::vector<unsigned int> keepStatusForce;
    bool cleanGenJets;
    std::vector< TLorentzVector > vGenLep;


    edm::EDGetTokenT<std::vector<PileupSummaryInfo>>   PupInfoToken;
    edm::EDGetTokenT<edm::TriggerResults >             muflagtagToken;
    edm::EDGetTokenT<double>                           rhoJetsNCToken;
    edm::EDGetTokenT<double>                           rhoJetsToken;
    edm::EDGetTokenT<pat::PackedCandidateCollection>   PFCandToken;
    edm::EDGetTokenT<reco::GenParticleCollection>      genParticlesToken;
    edm::EDGetTokenT<std::vector<pat::MET> >           METnoHFtoken;
    edm::EDGetTokenT<std::vector<pat::MET> >           METmodToken;
    edm::EDGetTokenT<GenEventInfoProduct>              genToken;
    edm::EDGetTokenT<LHEEventProduct>                  LHEToken;
    edm::EDGetTokenT<std::vector< reco::GenJet> >      genJetsToken;


    //helper functions
    int findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi);
    double mdeltaR(double eta1, double phi1, double eta2, double phi2);
    void fillMotherInfo(const reco::Candidate *mother,
			int i,
			std::vector <int> & momid,
			std::vector <int> & momstatus,
			std::vector<double> & mompt,
			std::vector<double> & mometa,
			std::vector<double> & momphi,
			std::vector<double> & momenergy);
    static bool SortLVByPt(const TLorentzVector a, const TLorentzVector b) {return a.Pt() > b.Pt();}


};


#endif