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

	edm::EDGetTokenT<std::vector<PileupSummaryInfo>>   PupInfoToken;
	edm::EDGetTokenT<edm::TriggerResults >             muflagtagToken;
	edm::EDGetTokenT<double>                           rhoJetsNC_Token;
        edm::EDGetTokenT<pat::PackedCandidateCollection>   PFCandToken;
        edm::EDGetTokenT<reco::GenParticleCollection>      genParticlesToken;

        //helper functions
        int findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi);
        double mdeltaR(double eta1, double phi1, double eta2, double phi2);
        void fillMotherInfo(const reco::Candidate *mother, int i, std::vector <int> & momid, std::vector <int> & momstatus, std::vector<double> & mompt, std::vector<double> & mometa, std::vector<double> & momphi, std::vector<double> & momenergy);


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

	//For miniIsolation
	rhoJetsNC_Token      = iC.consumes<double>(mPset.getParameter<edm::InputTag>("rhoInputTag"));
        PFCandToken          = iC.consumes<pat::PackedCandidateCollection>(mPset.getParameter<edm::InputTag>("PFparticlesCollection"));
        genParticlesToken    = iC.consumes<reco::GenParticleCollection>(mPset.getParameter<edm::InputTag>("genParticlesCollection"));


	debug             = mPset.getParameter<bool>("debug");
	isMc              = mPset.getParameter<bool>("isMc");
	saveLooseLeps     = mPset.getParameter<bool>("saveLooseLeps");
	keepFullMChistory = mPset.getParameter<bool>("keepFullMChistory");

	return 0;
}

int MultiLepCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector)
{
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

	std::vector< TLorentzVector > vGenLep;
	TLorentzVector tmpLV;

	//packed pf candidates and rho source needed miniIso
	edm::Handle<pat::PackedCandidateCollection> packedPFCands;
	event.getByToken(PFCandToken, packedPFCands);

	edm::Handle<double> rhoJetsNC;
	event.getByToken(rhoJetsNC_Token, rhoJetsNC);
	double myRhoJetsNC = *rhoJetsNC;

	edm::Handle<reco::GenParticleCollection> genParticles;
	event.getByToken(genParticlesToken, genParticles);

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

