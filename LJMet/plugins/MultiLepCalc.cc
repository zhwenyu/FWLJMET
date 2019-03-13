#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"


#include "FWLJMET/LJMet/interface/MiniIsolation.h"


class LjmetFactory;

class MultiLepCalc : public BaseCalc {
public:
    MultiLepCalc();
    virtual ~MultiLepCalc();
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;};

private:
	bool debug;
	bool isMc;
	bool saveLooseLeps;
	bool keepFullMChistory;
	bool UseElMVA;
	bool UseElIDV1;

	edm::EDGetTokenT<std::vector<PileupSummaryInfo>>   PupInfoToken;
	edm::EDGetTokenT<edm::TriggerResults >             muflagtagToken;
	edm::EDGetTokenT<double>                           rhoJetsNCToken;
	edm::EDGetTokenT<double>                           rhoJetsToken;
	edm::EDGetTokenT<pat::PackedCandidateCollection>   PFCandToken;
	edm::EDGetTokenT<reco::GenParticleCollection>      genParticlesToken;
	
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
        

};

static int reg = LjmetFactory::GetInstance()->Register(new MultiLepCalc(), "MultiLepCalc");


MultiLepCalc::MultiLepCalc()
{
}

MultiLepCalc::~MultiLepCalc()
{
}

int MultiLepCalc::BeginJob(edm::ConsumesCollector && iC)
{
	//do consumes here if need to access input file directly

	//PU info
	PupInfoToken 		= iC.consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("slimmedAddPileupInfo"));

	//Bad, dup, mu flag
	muflagtagToken 		= iC.consumes<edm::TriggerResults >(edm::InputTag("TriggerResults::RECO"));

	//Misc
	rhoJetsNCToken      = iC.consumes<double>(mPset.getParameter<edm::InputTag>("rhoJetsNCInputTag"));    
	PFCandToken         = iC.consumes<pat::PackedCandidateCollection>(mPset.getParameter<edm::InputTag>("PFparticlesCollection"));
	genParticlesToken   = iC.consumes<reco::GenParticleCollection>(mPset.getParameter<edm::InputTag>("genParticlesCollection"));
	rhoJetsToken        = iC.consumes<double>(mPset.getParameter<edm::InputTag>("rhoJetsInputTag"));

	//Electron
	UseElMVA          = mPset.getParameter<bool>("UseElMVA");
	UseElIDV1         = mPset.getParameter<bool>("UseElIDV1");

	debug             = mPset.getParameter<bool>("debug");
	isMc              = mPset.getParameter<bool>("isMc");
	saveLooseLeps     = mPset.getParameter<bool>("saveLooseLeps");
	keepFullMChistory = mPset.getParameter<bool>("keepFullMChistory");

	return 0;
}

int MultiLepCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector)
{
	//NOTE: THIS IS ALL BECOMING TOO LONG. NEEDS TO BE BROKEN DOWN SOMEHOW. IDEALLY PER OBJECTS. modularize! 

	if(debug)std::cout << "Processing Event in MultiLepCalc::AnalyzeEvent" << std::endl;

	// ----- Get objects from the selector -----
	std::map<std::string, unsigned int>         const & mSelMCTriggersEl = selector->GetSelectedMCTriggersEl();
	std::map<std::string, unsigned int>         const & mSelTriggersEl   = selector->GetSelectedTriggersEl();
	std::map<std::string, unsigned int>         const & mSelMCTriggersMu = selector->GetSelectedMCTriggersMu();
	std::map<std::string, unsigned int>         const & mSelTriggersMu   = selector->GetSelectedTriggersMu();

	std::vector<edm::Ptr<reco::Vertex>>         const & vSelPVs       = selector->GetSelPVs();

	std::vector<edm::Ptr<pat::Muon> >           const & vSelLooseMuons     = selector->GetSelLooseMuons();
	std::vector<edm::Ptr<pat::Electron> >       const & vSelLooseElectrons = selector->GetSelLooseElectrons();
	std::vector<edm::Ptr<pat::Muon> >                   vSelMuons          = selector->GetSelMuons();
	std::vector<edm::Ptr<pat::Electron> >               vSelElectrons      = selector->GetSelElectrons();

	//
	//_____Triggers______
	//
	std::vector<std::string> vsSelMCTriggersEl, vsSelTriggersEl, vsSelMCTriggersMu, vsSelTriggersMu;
	std::vector<int> viSelMCTriggersEl, viSelTriggersEl, viSelMCTriggersMu, viSelTriggersMu;
	for(std::map<std::string, unsigned int>::const_iterator j = mSelMCTriggersEl.begin(); j != mSelMCTriggersEl.end();j++) {
	  vsSelMCTriggersEl.push_back(j->first);
	  viSelMCTriggersEl.push_back((int)(j->second));
	}
	for(std::map<std::string, unsigned int>::const_iterator j = mSelTriggersEl.begin(); j != mSelTriggersEl.end();j++) {
	  vsSelTriggersEl.push_back(j->first);
	  viSelTriggersEl.push_back((int)(j->second));
	}
	for(std::map<std::string, unsigned int>::const_iterator j = mSelMCTriggersMu.begin(); j != mSelMCTriggersMu.end();j++) {
	  vsSelMCTriggersMu.push_back(j->first);
	  viSelMCTriggersMu.push_back((int)(j->second));
	}
	for(std::map<std::string, unsigned int>::const_iterator j = mSelTriggersMu.begin(); j != mSelTriggersMu.end();j++) {
	  vsSelTriggersMu.push_back(j->first);
	  viSelTriggersMu.push_back((int)(j->second));
	}
	SetValue("vsSelMCTriggersEl", vsSelMCTriggersEl);
	SetValue("vsSelTriggersEl", vsSelTriggersEl);
	SetValue("vsSelMCTriggersMu", vsSelMCTriggersMu);
	SetValue("vsSelTriggersMu", vsSelTriggersMu);
	SetValue("viSelMCTriggersEl", viSelMCTriggersEl);
	SetValue("viSelTriggersEl", viSelTriggersEl);
	SetValue("viSelMCTriggersMu", viSelMCTriggersMu);
	SetValue("viSelTriggersMu", viSelTriggersMu);


	//
	//_____Primary Vertex______
	//
	SetValue("nPV", (int)vSelPVs.size()); //This is now PV from PV selector not all PV in MiniAOD as how it was in old singleLepCalc.cc -- Rizki, Mar 12, 1019.

	//
	//_____PU______
	//
	int NumTrueInts = -1;
	int NumPUInts = -1;
	if(isMc){
	  edm::Handle<std::vector<PileupSummaryInfo>> PupInfo;
	  event.getByToken(PupInfoToken, PupInfo);

	  for(std::vector<PileupSummaryInfo>::const_iterator PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI){
	    int BX = PVI->getBunchCrossing();
	    if(BX == 0){
	      NumTrueInts = PVI->getTrueNumInteractions();
	      NumPUInts = PVI->getPU_NumInteractions();
	    }
	  }
	}
	SetValue("nTrueInteractions",NumTrueInts);
	SetValue("nPileupInteractions",NumPUInts);

	//
	//_____Bad & duplicate muon flags______
	//
	bool badmuonflag = false;
	bool dupmuonflag = false;

	if(!isMc){
	  edm::Handle<edm::TriggerResults > PatTriggerResults;
	  event.getByToken( muflagtagToken, PatTriggerResults );
	  const edm::TriggerNames patTrigNames = event.triggerNames(*PatTriggerResults);

	  for (unsigned int i=0; i<PatTriggerResults->size(); i++){
	    if (patTrigNames.triggerName(i) == "Flag_badMuons") badmuonflag = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_duplicateMuons") dupmuonflag = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	  }
	}
	SetValue("flagBadMu",badmuonflag);
	SetValue("flagDupMu",dupmuonflag);



	//option to override vSelMuons,vSelElectrons
	if(saveLooseLeps){
	  vSelMuons = vSelLooseMuons;
	  vSelElectrons = vSelLooseElectrons;
	}


	//
	//_____Misc______
	//

	std::vector< TLorentzVector > vGenLep;
	TLorentzVector tmpLV;

	//packed pf candidates and rho source needed miniIso
	edm::Handle<pat::PackedCandidateCollection> packedPFCands;
	event.getByToken(PFCandToken, packedPFCands);

	edm::Handle<double> rhoJetsNC;
	event.getByToken(rhoJetsNCToken, rhoJetsNC);
	double myRhoJetsNC = *rhoJetsNC;
	
	edm::Handle<reco::GenParticleCollection> genParticles;
	event.getByToken(genParticlesToken, genParticles);
	
	edm::Handle<double> rhoHandle;
	event.getByToken(rhoJetsToken, rhoHandle);
	double rhoIso = std::max(*(rhoHandle.product()), 0.0);


	//
	//_____Muons______
	//

	std::vector <int> muCharge;
	std::vector <int> muGlobal;
	//Four std::vector
	std::vector <double> muPt;
	std::vector <double> muEta;
	std::vector <double> muPhi;
	std::vector <double> muEnergy;
	//Three InnerTrack std::vector
	std::vector <double> muInnerPt;
	std::vector <double> muInnerEta;
	std::vector <double> muInnerPhi;
	//Quality criteria
	std::vector <double> muChi2;
	std::vector <double> muDxy;
	std::vector <double> muDz;
	std::vector <double> muRelIso;
	std::vector <double> muMiniIso;
	std::vector <double> muMiniIsoDB;

	std::vector <int> muNValMuHits;
	std::vector <int> muNMatchedStations;
	std::vector <int> muNValPixelHits;
	std::vector <int> muNTrackerLayers;
	//Extra info about isolation
	std::vector <double> muChIso;
	std::vector <double> muNhIso;
	std::vector <double> muGIso;
	std::vector <double> muPuIso;
	//ID info
	std::vector <int> muIsTight;
	std::vector <int> muIsMedium;
	std::vector<int> muIsLoose;
	std::vector<int> muIsMediumPrompt;
	std::vector<int> muIsGlobalHighPt;
	std::vector<int> muIsTrkHighPt;
	std::vector<int> muIsMvaLoose;
	std::vector<int> muIsMvaMedium;
	std::vector<int> muIsMvaTight;
	std::vector<int> muIsMiniIsoLoose;
	std::vector<int> muIsMiniIsoMedium;
	std::vector<int> muIsMiniIsoTight;
	std::vector<int> muIsMiniIsoVeryTight;

	//Generator level information -- MC matching
	std::vector<double> muGen_Reco_dr;
	std::vector<int> muPdgId;
	std::vector<int> muStatus;
	std::vector<int> muMatched;
	std::vector<int> muNumberOfMothers;
	std::vector<double> muMother_pt;
	std::vector<double> muMother_eta;
	std::vector<double> muMother_phi;
	std::vector<double> muMother_energy;
	std::vector<int> muMother_id;
	std::vector<int> muMother_status;
	//Matched gen muon information:
	std::vector<double> muMatchedPt;
	std::vector<double> muMatchedEta;
	std::vector<double> muMatchedPhi;
	std::vector<double> muMatchedEnergy;

	for (std::vector<edm::Ptr<pat::Muon> >::const_iterator imu = vSelMuons.begin(); imu != vSelMuons.end(); imu++) {
	  //Protect against muons without tracks (should never happen, but just in case)
	  if ((*imu)->globalTrack().isNonnull()   and 
	      (*imu)->globalTrack().isAvailable() and
	      (*imu)->innerTrack().isNonnull()    and 
	      (*imu)->innerTrack().isAvailable())
	    {

	    if ((*imu)->genParticle()!=0) {
	      tmpLV.SetPtEtaPhiE((*imu)->genParticle()->pt(),(*imu)->genParticle()->eta(),(*imu)->genParticle()->phi(),(*imu)->genParticle()->energy());
	      vGenLep.push_back(tmpLV);
	    }

            //charge
            muCharge.push_back((*imu)->charge());
            // 4-std::vector
            muPt     . push_back((*imu)->pt());
            muEta    . push_back((*imu)->eta());
            muPhi    . push_back((*imu)->phi());
            muEnergy . push_back((*imu)->energy());

            muInnerPt     . push_back((*imu)->innerTrack()->pt());
            muInnerEta    . push_back((*imu)->innerTrack()->eta());
            muInnerPhi    . push_back((*imu)->innerTrack()->phi());

            muIsTight.push_back((*imu)->passed(reco::Muon::CutBasedIdTight));
            muIsMedium.push_back((*imu)->passed(reco::Muon::CutBasedIdMedium));
            muIsMediumPrompt.push_back((*imu)->passed(reco::Muon::CutBasedIdMediumPrompt));
            muIsLoose.push_back((*imu)->passed(reco::Muon::CutBasedIdLoose));
            muIsGlobalHighPt.push_back((*imu)->passed(reco::Muon::CutBasedIdGlobalHighPt));
            muIsTrkHighPt.push_back((*imu)->passed(reco::Muon::CutBasedIdTrkHighPt));
            muIsMvaLoose.push_back((*imu)->passed(reco::Muon::MvaLoose));
            muIsMvaMedium.push_back((*imu)->passed(reco::Muon::MvaMedium));
            muIsMvaTight.push_back((*imu)->passed(reco::Muon::MvaTight));
            muIsMiniIsoLoose.push_back((*imu)->passed(reco::Muon::MiniIsoLoose));
            muIsMiniIsoMedium.push_back((*imu)->passed(reco::Muon::MiniIsoMedium));
            muIsMiniIsoTight.push_back((*imu)->passed(reco::Muon::MiniIsoTight));
            muIsMiniIsoVeryTight.push_back((*imu)->passed(reco::Muon::MiniIsoVeryTight));

            muGlobal.push_back(((*imu)->isGlobalMuon()<<2)+(*imu)->isTrackerMuon());
            //chi2
            muChi2 . push_back((*imu)->globalTrack()->normalizedChi2());
            //Isolation
            double chIso  = (*imu)->userIsolation(pat::PfChargedHadronIso);
            double nhIso  = (*imu)->userIsolation(pat::PfNeutralHadronIso);
            double gIso   = (*imu)->userIsolation(pat::PfGammaIso);
            double puIso  = (*imu)->userIsolation(pat::PfPUChargedHadronIso);
            double relIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso)) / (*imu)->pt();

			//Do we need two of these? And don't we need to update to the official CMSSW MiniIsolation.cc rather than some old file --Rizki Mar 12, 2019.
            double miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate *>(imu->get()), 0.05, 0.2, 10., false, false,myRhoJetsNC);
            double miniIsoDB = getPFMiniIsolation_DeltaBeta(packedPFCands, dynamic_cast<const reco::Candidate *>(imu->get()), 0.05, 0.2, 10., false);

            muRelIso . push_back(relIso);
            muMiniIso . push_back(miniIso);
            muMiniIsoDB . push_back(miniIsoDB);
            muRelIso . push_back(relIso);

            muChIso . push_back(chIso);
            muNhIso . push_back(nhIso);
            muGIso  . push_back(gIso);
            muPuIso . push_back(puIso);
            //IP: for some reason this is with respect to the first vertex in the collection
            if (vSelPVs.size() > 0){ //This is now PV from PV selector not all PV in MiniAOD as how it was in old singleLepCalc.cc -- Rizki, Mar 12, 1019.
              muDxy . push_back((*imu)->muonBestTrack()->dxy((*vSelPVs[0]).position()));
              muDz  . push_back((*imu)->muonBestTrack()->dz((*vSelPVs[0]).position()));
            } else {
              muDxy . push_back(-999);
              muDz  . push_back(-999);
            }
            //Numbers of hits
            muNValMuHits       . push_back((*imu)->globalTrack()->hitPattern().numberOfValidMuonHits());
            muNMatchedStations . push_back((*imu)->numberOfMatchedStations());
            muNValPixelHits    . push_back((*imu)->innerTrack()->hitPattern().numberOfValidPixelHits());
            muNTrackerLayers   . push_back((*imu)->innerTrack()->hitPattern().trackerLayersWithMeasurement());
            if(isMc && keepFullMChistory){
                int matchId = findMatch(*genParticles, 13, (*imu)->eta(), (*imu)->phi());
                double closestDR = 10000.;
                if (matchId>=0) {
                    const reco::GenParticle & p = (*genParticles).at(matchId);
                    closestDR = mdeltaR( (*imu)->eta(), (*imu)->phi(), p.eta(), p.phi());
                    if(closestDR < 0.3){
                        muGen_Reco_dr.push_back(closestDR);
                        muPdgId.push_back(p.pdgId());
                        muStatus.push_back(p.status());
                        muMatched.push_back(1);
                        muMatchedPt.push_back( p.pt());
                        muMatchedEta.push_back(p.eta());
                        muMatchedPhi.push_back(p.phi());
                        muMatchedEnergy.push_back(p.energy());
                        int oldSize = muMother_id.size();
                        fillMotherInfo(p.mother(), 0, muMother_id, muMother_status, muMother_pt, muMother_eta, muMother_phi, muMother_energy);
                        muNumberOfMothers.push_back(muMother_id.size()-oldSize);
                    }
                }
                if(closestDR >= 0.3){
                    muNumberOfMothers.push_back(-1);
                    muGen_Reco_dr.push_back(-1.0);
                    muPdgId.push_back(-1);
                    muStatus.push_back(-1);
                    muMatched.push_back(0);
                    muMatchedPt.push_back(-1000.0);
                    muMatchedEta.push_back(-1000.0);
                    muMatchedPhi.push_back(-1000.0);
                    muMatchedEnergy.push_back(-1000.0);
                }
            }
	  }
	}
	SetValue("muCharge", muCharge);
	SetValue("muGlobal", muGlobal);
	SetValue("muPt"     , muPt);
	SetValue("muEta"    , muEta);
	SetValue("muPhi"    , muPhi);
	SetValue("muInnerPt"     , muInnerPt);
	SetValue("muInnerEta"    , muInnerEta);
	SetValue("muInnerPhi"    , muInnerPhi);
	SetValue("muEnergy" , muEnergy);
	SetValue("muIsTight", muIsTight);
	SetValue("muIsMedium", muIsMedium);
	SetValue("muIsMediumPrompt",muIsMediumPrompt);
	SetValue("muIsLoose",muIsLoose);
	SetValue("muIsGlobalHighPt",muIsGlobalHighPt);
	SetValue("muIsTrkHighPt",muIsTrkHighPt);
	SetValue("muIsMvaLoose",muIsMvaLoose);
	SetValue("muIsMvaMedium",muIsMvaMedium);
	SetValue("muIsMvaTight",muIsMvaTight);
	SetValue("muIsMiniIsoLoose",muIsMiniIsoLoose);
	SetValue("muIsMiniIsoMedium",muIsMiniIsoMedium);
	SetValue("muIsMiniIsoTight",muIsMiniIsoTight);
	SetValue("muIsMiniIsoVeryTight",muIsMiniIsoVeryTight);
	//Quality criteria
	SetValue("muChi2"   , muChi2);
	SetValue("muDxy"    , muDxy);
	SetValue("muDz"     , muDz);
	SetValue("muRelIso" , muRelIso);
	SetValue("muMiniIso", muMiniIso);
	SetValue("muMiniIsoDB", muMiniIsoDB);

	SetValue("muNValMuHits"       , muNValMuHits);
	SetValue("muNMatchedStations" , muNMatchedStations);
	SetValue("muNValPixelHits"    , muNValPixelHits);
	SetValue("muNTrackerLayers"   , muNTrackerLayers);
	SetValue("muChIso", muChIso);
	SetValue("muNhIso", muNhIso);
	SetValue("muGIso" , muGIso);
	SetValue("muPuIso", muPuIso);
	//MC matching -- mother information
	SetValue("muGen_Reco_dr", muGen_Reco_dr);
	SetValue("muPdgId", muPdgId);
	SetValue("muStatus", muStatus);
	SetValue("muMatched",muMatched);
	SetValue("muMother_pt", muMother_pt);
	SetValue("muMother_eta", muMother_eta);
	SetValue("muMother_phi", muMother_phi);
	SetValue("muMother_energy", muMother_energy);
	SetValue("muMother_status", muMother_status);
	SetValue("muMother_id", muMother_id);
	SetValue("muNumberOfMothers", muNumberOfMothers);
	//Matched gen muon information:
	SetValue("muMatchedPt", muMatchedPt);
	SetValue("muMatchedEta", muMatchedEta);
	SetValue("muMatchedPhi", muMatchedPhi);
	SetValue("muMatchedEnergy", muMatchedEnergy);


    //
    //_____Electrons______
    //

    //Four std::vector
    std::vector <double> elPt;
    std::vector <double> elEta;
    std::vector <double> elPFEta;
    std::vector <double> elPhi;
    std::vector <double> elSCE;
    std::vector <double> elPFPhi;
    std::vector <double> elEnergy;

    std::vector <double> elEtaVtx;
    std::vector <double> elPhiVtx;
    std::vector <double> elDEtaSCTkAtVtx;
    std::vector <double> elDPhiSCTkAtVtx;

    //Quality criteria
    std::vector <double> elRelIso;
    std::vector <double> elMiniIso;
    std::vector <double> elDxy;
    std::vector <int>    elNotConversion;
    std::vector <int>    elChargeConsistent;
    std::vector <int>    elIsEBEE;
    std::vector <int>    elCharge;
    std::vector <int>    elGsfCharge;
    std::vector <int>    elCtfCharge;
    std::vector <int>    elScPixCharge;

    //ID requirement
    std::vector <double> elDeta;
    std::vector <double> elDphi;
    std::vector <double> elSihih;
    std::vector <double> elHoE;
    std::vector <double> elD0;
    std::vector <double> elDZ;
    std::vector <double> elOoemoop;
    std::vector <int>    elMHits;
    std::vector <int>    elVtxFitConv;    

    std::vector <double> elMVAValue;
    std::vector <double> elMVAValue_iso;
    std::vector <int>    elIsMVATight80;
    std::vector <int>    elIsMVATight90;
    std::vector <int>    elIsMVALoose;
    std::vector <int>    elIsMVATightIso80;
    std::vector <int>    elIsMVATightIso90;
    std::vector <int>    elIsMVALooseIso;

    //Extra info about isolation
    std::vector <double> elChIso;
    std::vector <double> elNhIso;
    std::vector <double> elPhIso;
    std::vector <double> elAEff;
    std::vector <double> elRhoIso;
    std::vector <double> elEcalPFClusterIso;
    std::vector <double> elHcalPFClusterIso;
    std::vector <double> elDR03TkSumPt;

    //mother-information
    //Generator level information -- MC matching
    std::vector<double> elGen_Reco_dr;
    std::vector<int> elPdgId;
    std::vector<int> elStatus;
    std::vector<int> elMatched;
    std::vector<int> elNumberOfMothers;
    std::vector<double> elMother_pt;
    std::vector<double> elMother_eta;
    std::vector<double> elMother_phi;
    std::vector<double> elMother_energy;
    std::vector<int> elMother_id;
    std::vector<int> elMother_status;

    //Matched gen electron information:
    std::vector<double> elMatchedPt;
    std::vector<double> elMatchedEta;
    std::vector<double> elMatchedPhi;
    std::vector<double> elMatchedEnergy;

    std::vector<double> elIsTight;
    std::vector<double> elIsMedium;
    std::vector<double> elIsLoose;
    std::vector<double> elIsVeto;


    for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iel = vSelElectrons.begin(); iel != vSelElectrons.end(); iel++){
        //Protect against electrons without tracks (should never happen, but just in case)
        if ((*iel)->gsfTrack().isNonnull() and (*iel)->gsfTrack().isAvailable()){

	    if ((*iel)->genParticle()!=0) {
                tmpLV.SetPtEtaPhiE((*iel)->genParticle()->pt(),(*iel)->genParticle()->eta(),(*iel)->genParticle()->phi(),(*iel)->genParticle()->energy());
                vGenLep.push_back(tmpLV);
            }

            //Four std::vector
            elPt     . push_back((*iel)->pt()); //Must check: why ecalDrivenMomentum?
            elEta    . push_back((*iel)->superCluster()->eta());
            elPFEta  . push_back((*iel)->eta());
            elPhi    . push_back((*iel)->superCluster()->phi());
            elSCE    . push_back((*iel)->superCluster()->energy());
            elPFPhi  . push_back((*iel)->phi());
            elEnergy . push_back((*iel)->energy());
            
            elEtaVtx.push_back((*iel)->trackMomentumAtVtxWithConstraint().Eta());
            elPhiVtx.push_back((*iel)->trackMomentumAtVtxWithConstraint().Phi());
            elDEtaSCTkAtVtx.push_back((*iel)->deltaEtaSuperClusterTrackAtVtx());
            elDPhiSCTkAtVtx.push_back((*iel)->deltaPhiSuperClusterTrackAtVtx());

            //Isolation
            double scEta = (*iel)->superCluster()->eta();
            double AEff;
            if(fabs(scEta) >2.4) AEff = 0.1524;
            else if(fabs(scEta) >2.3) AEff = 0.1204;
            else if(fabs(scEta) >2.2) AEff = 0.1051;
            else if(fabs(scEta) >2.0) AEff = 0.0854;
            else if(fabs(scEta) >1.479) AEff = 0.1073;
            else if(fabs(scEta) >1.0) AEff = 0.1626;
            else AEff = 0.1566;
  
            double chIso = ((*iel)->pfIsolationVariables()).sumChargedHadronPt;
            double nhIso = ((*iel)->pfIsolationVariables()).sumNeutralHadronEt;
            double phIso = ((*iel)->pfIsolationVariables()).sumPhotonEt;
            double relIso = ( chIso + std::max(0.0, nhIso + phIso - rhoIso*AEff) ) / (*iel)->pt();

            elChIso  . push_back(chIso);
            elNhIso  . push_back(nhIso);
            elPhIso  . push_back(phIso);
            elAEff   . push_back(AEff);
            elRhoIso . push_back(rhoIso);
            
            elEcalPFClusterIso.push_back((*iel)->ecalPFClusterIso());
            elHcalPFClusterIso.push_back((*iel)->hcalPFClusterIso());
            elDR03TkSumPt.push_back((*iel)->dr03TkSumPt());

            double miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate *>(iel->get()), 0.05, 0.2, 10., false, false,myRhoJetsNC);

            elRelIso . push_back(relIso);
            elMiniIso . push_back(miniIso);
            
            //get three different charges
            elGsfCharge.push_back( (*iel)->gsfTrack()->charge());
            if( (*iel)->closestCtfTrackRef().isNonnull()) elCtfCharge.push_back((*iel)->closestCtfTrackRef()->charge());
            else elCtfCharge.push_back(-999);
            elScPixCharge.push_back((*iel)->scPixCharge());
            elCharge.push_back((*iel)->charge());

            //IP: for some reason this is with respect to the first vertex in the collection
            if(vSelPVs.size() > 0){ //This is now PV from PV selector not all PV in MiniAOD as how it was in old singleLepCalc.cc -- Rizki, Mar 12, 1019.
                elDxy.push_back((-1.0)*(*iel)->gsfTrack()->dxy((*vSelPVs[0]).position()));
                elD0.push_back((-1.0)*(*iel)->gsfTrack()->dxy((*vSelPVs[0]).position()));
                elDZ.push_back((*iel)->gsfTrack()->dz((*vSelPVs[0]).position()));
            } else {
                elDxy.push_back(-999);
                elD0.push_back(-999);
                elDZ.push_back(-999);
            }
            elChargeConsistent.push_back((*iel)->isGsfCtfScPixChargeConsistent());
            elIsEBEE.push_back(((*iel)->isEBEEGap()<<2) + ((*iel)->isEE()<<1) + (*iel)->isEB());
            elDeta.push_back((*iel)->deltaEtaSuperClusterTrackAtVtx());
            elDphi.push_back((*iel)->deltaPhiSuperClusterTrackAtVtx());
            elSihih.push_back((*iel)->full5x5_sigmaIetaIeta());
            elHoE.push_back((*iel)->hcalOverEcal());
            elOoemoop.push_back(fabs(1.0/(*iel)->ecalEnergy() - (*iel)->eSuperClusterOverP()/(*iel)->ecalEnergy()));
            elMHits.push_back((*iel)->gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS));
            elVtxFitConv.push_back((*iel)->passConversionVeto());
            elNotConversion.push_back((*iel)->passConversionVeto());
	    

	    if(UseElIDV1){
	      elIsTight.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V1-tight"));
	      elIsMedium.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V1-medium"));
	      elIsLoose.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V1-loose"));
	      elIsVeto.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V1-veto"));

	      if (UseElMVA) {
			elIsMVATight80.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V1-wp80"));
			elIsMVATight90.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V1-wp90"));
			elIsMVALoose.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V1-wpLoose"));
			elMVAValue.push_back((*iel)->userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV1Values"));

			elIsMVATightIso80.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V1-wp80"));
			elIsMVATightIso90.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V1-wp90"));
			elIsMVALooseIso.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V1-wpLoose"));
			elMVAValue_iso.push_back((*iel)->userFloat("ElectronMVAEstimatorRun2Fall17IsoV1Values"));
	      }
	    }
	    else{
	      elIsTight.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-tight"));
	      elIsMedium.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-medium"));
	      elIsLoose.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-loose"));
	      elIsVeto.push_back((*iel)->electronID("cutBasedElectronID-Fall17-94X-V2-veto"));

	      if (UseElMVA) {
			elIsMVATight80.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V2-wp80"));
			elIsMVATight90.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V2-wp90"));
			elIsMVALoose.push_back((*iel)->electronID("mvaEleID-Fall17-noIso-V2-wpLoose"));
			elMVAValue.push_back((*iel)->userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV2Values"));

			elIsMVATightIso80.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V2-wp80"));
			elIsMVATightIso90.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V2-wp90"));
			elIsMVALooseIso.push_back((*iel)->electronID("mvaEleID-Fall17-iso-V2-wpLoose"));
			elMVAValue_iso.push_back((*iel)->userFloat("ElectronMVAEstimatorRun2Fall17IsoV2Values"));
	      }
	    }

            if(isMc && keepFullMChistory){
                //cout << "start\n";
                int matchId = findMatch(*genParticles, 11, (*iel)->eta(), (*iel)->phi());
                double closestDR = 10000.;
                //cout << "matchId "<<matchId <<endl;
                if (matchId>=0) {
                    const reco::GenParticle & p = (*genParticles).at(matchId);
                    closestDR = mdeltaR( (*iel)->eta(), (*iel)->phi(), p.eta(), p.phi());
                    //cout << "closestDR "<<closestDR <<endl;
                    if(closestDR < 0.3){
                        elGen_Reco_dr.push_back(closestDR);
                        elPdgId.push_back(p.pdgId());
                        elStatus.push_back(p.status());
                        elMatched.push_back(1);
                        elMatchedPt.push_back( p.pt());
                        elMatchedEta.push_back(p.eta());
                        elMatchedPhi.push_back(p.phi());
                        elMatchedEnergy.push_back(p.energy());
                        int oldSize = elMother_id.size();
                        fillMotherInfo(p.mother(), 0, elMother_id, elMother_status, elMother_pt, elMother_eta, elMother_phi, elMother_energy);
                        elNumberOfMothers.push_back(elMother_id.size()-oldSize);
                    }
                }
                if(closestDR >= 0.3){
                    elNumberOfMothers.push_back(-1);
                    elGen_Reco_dr.push_back(-1.0);
                    elPdgId.push_back(-1);
                    elStatus.push_back(-1);
                    elMatched.push_back(0);
                    elMatchedPt.push_back(-1000.0);
                    elMatchedEta.push_back(-1000.0);
                    elMatchedPhi.push_back(-1000.0);
                    elMatchedEnergy.push_back(-1000.0);

                }
            }//closing the isMC checking criteria
        }
    }

    //Four std::vector
    SetValue("elPt"     , elPt);
    SetValue("elEta"    , elEta);
    SetValue("elPFEta"  , elPFEta);
    SetValue("elPhi"    , elPhi);
    SetValue("elSCE"    , elSCE);
    SetValue("elPFPhi"  , elPFPhi);
    SetValue("elEnergy" , elEnergy);

    SetValue("elEtaVtx" , elEtaVtx);
    SetValue("elPhiVtx" , elPhiVtx);
    SetValue("elDEtaSCTkAtVtx" , elDEtaSCTkAtVtx);
    SetValue("elDPhiSCTkAtVtx" , elDPhiSCTkAtVtx);

    SetValue("elCharge", elCharge);
    SetValue("elGsfCharge", elGsfCharge);
    SetValue("elCtfCharge", elCtfCharge);
    SetValue("elScPixCharge", elScPixCharge);

    SetValue("elIsTight", elIsTight);
    SetValue("elIsMedium",elIsMedium);
    SetValue("elIsLoose",elIsLoose);
    SetValue("elIsVeto",elIsVeto);

    //Quality requirements
    SetValue("elRelIso" , elRelIso); //Isolation
    SetValue("elMiniIso" , elMiniIso); //Mini Isolation
    SetValue("elDxy"    , elDxy);    //Dxy
    SetValue("elNotConversion" , elNotConversion);  //Conversion rejection
    SetValue("elChargeConsistent", elChargeConsistent);
    SetValue("elIsEBEE", elIsEBEE);

    //ID cuts
    SetValue("elDeta", elDeta);
    SetValue("elDphi", elDphi);
    SetValue("elSihih", elSihih);
    SetValue("elHoE", elHoE);
    SetValue("elD0", elD0);
    SetValue("elDZ", elDZ);
    SetValue("elOoemoop", elOoemoop);
    SetValue("elMHits", elMHits);
    SetValue("elVtxFitConv", elVtxFitConv);

    SetValue("elMVAValue", elMVAValue);
    SetValue("elMVAValue_iso", elMVAValue_iso);
    SetValue("elIsMVATight80", elIsMVATight80);
    SetValue("elIsMVATight90", elIsMVATight90);
    SetValue("elIsMVALoose", elIsMVALoose);
    SetValue("elIsMVATightIso80",elIsMVATightIso80);
    SetValue("elIsMVATightIso90",elIsMVATightIso90);
    SetValue("elIsMVALooseIso",elIsMVALooseIso);

    //Extra info about isolation
    SetValue("elChIso" , elChIso);
    SetValue("elNhIso" , elNhIso);
    SetValue("elPhIso" , elPhIso);
    SetValue("elAEff"  , elAEff);
    SetValue("elRhoIso", elRhoIso);
    SetValue("elEcalPFClusterIso", elEcalPFClusterIso);
    SetValue("elHcalPFClusterIso", elHcalPFClusterIso);
    SetValue("elDR03TkSumPt", elDR03TkSumPt);

    //MC matching -- mother information
    SetValue("elNumberOfMothers", elNumberOfMothers);
    SetValue("elGen_Reco_dr", elGen_Reco_dr);
    SetValue("elPdgId", elPdgId);
    SetValue("elStatus", elStatus);
    SetValue("elMatched",elMatched);
    SetValue("elMother_pt", elMother_pt);
    SetValue("elMother_eta", elMother_eta);
    SetValue("elMother_phi", elMother_phi);
    SetValue("elMother_energy", elMother_energy);
    SetValue("elMother_status", elMother_status);
    SetValue("elMother_id", elMother_id);
    //Matched gen muon information:
    SetValue("elMatchedPt", elMatchedPt);
    SetValue("elMatchedEta", elMatchedEta);
    SetValue("elMatchedPhi", elMatchedPhi);
    SetValue("elMatchedEnergy", elMatchedEnergy);


	return 0;
}

int MultiLepCalc::findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi)
{
    float dRtmp = 1000;
    float closestDR = 10000.;
    int closestGenPart = -1;

    for(size_t j = 0; j < genParticles.size(); ++ j) {
        const reco::GenParticle & p = (genParticles).at(j);
        dRtmp = mdeltaR( eta, phi, p.eta(), p.phi());
        if ( dRtmp < closestDR && abs(p.pdgId()) == idToMatch){// && dRtmp < 0.3) {
            closestDR = dRtmp;
            closestGenPart = j;
        }//end of requirement for matching
    }//end of gen particle loop 
    return closestGenPart;
}


double MultiLepCalc::mdeltaR(double eta1, double phi1, double eta2, double phi2) {
    return std::sqrt(deltaR2 (eta1, phi1, eta2, phi2));
}

void MultiLepCalc::fillMotherInfo(const reco::Candidate *mother, int i, std::vector <int> & momid, std::vector <int> & momstatus, std::vector<double> & mompt, std::vector<double> & mometa, std::vector<double> & momphi, std::vector<double> & momenergy)
{
    if(mother) {
        momid.push_back(mother->pdgId());
        momstatus.push_back(mother->status());
        mompt.push_back(mother->pt());
        mometa.push_back(mother->eta());
        momphi.push_back(mother->phi());
        momenergy.push_back(mother->energy());
        if(i<10)fillMotherInfo(mother->mother(), i+1, momid, momstatus, mompt, mometa, momphi, momenergy);
    }


}

