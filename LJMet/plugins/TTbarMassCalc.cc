/*
  Calculator for the Run 2 inclusive TprimeTprime analysis
  Finds Tprime or Bprime particles and tags the decays chain so branching ratios can be scanned

  Author: Julie Hogan, 2015
*/


#include <iostream>
#include <algorithm>
#include "FWLJMET/LJMet/interface/BaseCalc.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"
#include "FWLJMET/LJMet/interface/LjmetEventContent.h"
#include "TLorentzVector.h" 
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfo.h"
#include "DataFormats/JetReco/interface/GenJet.h"

using namespace std;


class TTbarMassCalc : public BaseCalc{
  
public:
  
    TTbarMassCalc();
    virtual ~TTbarMassCalc(){}
    virtual int BeginJob(edm::ConsumesCollector && iC);
    virtual int AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;}

  
private:
  

    edm::EDGetTokenT<reco::GenParticleCollection>      genParticlesToken;
    edm::EDGetTokenT<int>                              genTtbarIdToken;


 
};



static int reg = LjmetFactory::GetInstance()->Register(new TTbarMassCalc(), "TTbarMassCalc");



TTbarMassCalc::TTbarMassCalc(){
}

int TTbarMassCalc::BeginJob(edm::ConsumesCollector && iC)
{

      genParticlesToken = iC.consumes<reco::GenParticleCollection>(mPset.getParameter<edm::InputTag>("genParticles"));
      genTtbarIdToken   = iC.consumes<int>(mPset.getParameter<edm::InputTag>("genTtbarId"));
      
      return 0;


}

int TTbarMassCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector){
    //
    // compute event variables here
    //

    std::vector<int> genTtbarId;
    std::vector<int> genTtbarIdCategory;
    int id = -1;
    int category = -1;

    edm::Handle<int> ttbarId;
    if(event.getByToken(genTtbarIdToken, ttbarId)){  
      const int* ID = ttbarId.product();
      id = *ID;
    }
    // save the actual full ID value for checks or changes
    genTtbarId.push_back(id);

    // Id's like xxx00 are light
    // xxx55, xxx54 = 2B 
    // xxx53 = 2b
    // xxx52, xxx51 = 1b
    // xxx4x = charm
    if(id == -1) category = -1;
    else if(id == 0 || id%100 == 0) category = 0; // light
    else if((id+45)%100 == 0 || (id+46)%100 == 0) category = 4; // 2B
    else if((id+47)%100 == 0) category = 3; // 2b
    else if((id+48)%100 == 0 || (id+49)%100 == 0) category = 2; // 1b
    else category = 1; // c
    genTtbarIdCategory.push_back(category);

    SetValue("genTtbarId",genTtbarId);
    SetValue("genTtbarIdCategory",genTtbarIdCategory);  
      
    std::vector<int>    topID;
    std::vector<int>    topMotherID;
    std::vector<double> topPt;
    std::vector<double> topEta;
    std::vector<double> topPhi;
    std::vector<double> topEnergy;
    std::vector<double> topMass;

    std::vector<int>    topbID;
    std::vector<double> topbPt;
    std::vector<double> topbEta;
    std::vector<double> topbPhi;
    std::vector<double> topbEnergy;

    std::vector<int>    topWID;
    std::vector<double> topWPt;
    std::vector<double> topWEta;
    std::vector<double> topWPhi;
    std::vector<double> topWEnergy;

    std::vector<int>    allTopsID;
    std::vector<int>    allTopsStatus;
    std::vector<double> allTopsPt;
    std::vector<double> allTopsEta;
    std::vector<double> allTopsPhi;
    std::vector<double> allTopsEnergy;

    double ttbarMass = -999.9;
    TLorentzVector top1;
    TLorentzVector top2;

    // Get the generated particle collection
    edm::Handle<reco::GenParticleCollection> genParticles;
    if(event.getByToken(genParticlesToken, genParticles)){

      // loop over all gen particles in event
      for(size_t i = 0; i < genParticles->size(); i++){
	const reco::GenParticle &p = (*genParticles).at(i);
	int id = p.pdgId();
	
	// find tops 
	if(abs(id) != 6) continue;

	size_t nDs = p.numberOfDaughters();
	if(nDs == 2){
	  int d1Id = p.daughter(0)->pdgId();
	  int d2Id = p.daughter(1)->pdgId();
	  
	  if(abs(d1Id)==6 || abs(d2Id)==6) continue;

	  if((abs(d1Id)==5 && abs(d2Id)==24) || (abs(d1Id)==24 && abs(d2Id)==5)){
	    const reco::Candidate *d = p.daughter(0);
	    if(abs(d1Id)==24) d = p.daughter(1);

	    topbID.push_back(d->pdgId());
	    topbPt.push_back(d->pt());
	    topbEta.push_back(d->eta());
	    topbPhi.push_back(d->phi());
	    topbEnergy.push_back(d->energy());

	    const reco::Candidate *W = p.daughter(1);
	    if(abs(d2Id)==5) W = p.daughter(0);

	    while(W->numberOfDaughters() == 1) W = W->daughter(0);	    
	    if(W->daughter(1)->pdgId() == 22) W = W->daughter(0);	    
	    while(W->numberOfDaughters() == 1) W = W->daughter(0);	    

	    size_t nWDs = W->numberOfDaughters();
	    if(nWDs > 2) cout << "W daughters: " << nWDs << endl;
	    int Wd1Id = abs(W->daughter(0)->pdgId());
	    int Wd2Id = abs(W->daughter(1)->pdgId());

	    if((Wd1Id > 10 && Wd1Id < 17 && Wd2Id > 10 && Wd2Id < 17) || (Wd1Id < 6 && Wd2Id < 6)){
	      topWID.push_back(W->daughter(0)->pdgId());
	      topWID.push_back(W->daughter(1)->pdgId());
	      topWPt.push_back(W->daughter(0)->pt());
	      topWPt.push_back(W->daughter(1)->pt());
	      topWEta.push_back(W->daughter(0)->eta());
	      topWEta.push_back(W->daughter(1)->eta());
	      topWPhi.push_back(W->daughter(0)->phi());
	      topWPhi.push_back(W->daughter(1)->phi());
	      topWEnergy.push_back(W->daughter(0)->energy());
	      topWEnergy.push_back(W->daughter(1)->energy());
	    }else{	      
	      cout << "Weird W decay: " << Wd1Id << ", " << Wd2Id << endl;
	    }
	  }else{
	    cout << "2 top daughters are: " << d1Id << ", " << d2Id << endl;
	  }
	}

	// Find original tops, before t -> t -> t chains
	if(abs(p.mother()->pdgId()) == 6) continue;      
	
	topID.push_back(id);
	topPt.push_back(p.pt());
	topEta.push_back(p.eta());
	topPhi.push_back(p.phi());
	topMass.push_back(p.mass());
	topEnergy.push_back(p.energy());
	topMotherID.push_back(p.mother()->pdgId());

      }
            
      // Now time for some checking -- do we have the right particles? 
      if(topID.size() == 0) ttbarMass = -999;
      else if(topID.size() != 2){
	//	std::cout << "TTbarMassCalc: More/less than 2 quarks stored: " << topID.size() << std::endl;
	//	for(size_t i = 0; i < topID.size(); i++){	
	//	  std::cout << "quark " << i << " = " << topID[i] << ", mother ID = " << topMotherID[i] << std::endl;
	//	}
      }
      else{
	if(topID[0]*topID[1] > 0){
	  cout << "tops have same ID sign!" << endl;
	}
	else{
	  /*
	  cout << "----------------------------------" << endl;
	  cout << "particle " << topID[0] << " pt = " << topPt[0] << endl;
	  cout << "particle " << topID[1] << " pt = " << topPt[1] << endl;
	  */
	  top1.SetPtEtaPhiE(topPt[0],topEta[0],topPhi[0],topEnergy[0]);
	  top2.SetPtEtaPhiE(topPt[1],topEta[1],topPhi[1],topEnergy[1]);
	  
	  ttbarMass = (top1+top2).M();
	}
      }

      // loop over all gen particles in event
      for(size_t i = 0; i < genParticles->size(); i++){
	const reco::GenParticle &p = (*genParticles).at(i);
	int id = p.pdgId();
	
	// find tops 
	if(abs(id) != 6) continue;
	
	allTopsID.push_back(id);
	allTopsPt.push_back(p.pt());
	allTopsEta.push_back(p.eta());
	allTopsPhi.push_back(p.phi());
	allTopsEnergy.push_back(p.energy());
	allTopsStatus.push_back(p.status());
	
      }
    }
    SetValue("topID",topID);
    SetValue("topPt",topPt);
    SetValue("topEta",topEta);
    SetValue("topPhi",topPhi);
    SetValue("topMass",topMass);
    SetValue("topEnergy",topEnergy);

    SetValue("topbID",topbID);
    SetValue("topbPt",topbPt);
    SetValue("topbEta",topbEta);
    SetValue("topbPhi",topbPhi);
    SetValue("topbEnergy",topbEnergy);

    SetValue("topWID",topWID);
    SetValue("topWPt",topWPt);
    SetValue("topWEta",topWEta);
    SetValue("topWPhi",topWPhi);
    SetValue("topWEnergy",topWEnergy);
    
    SetValue("ttbarMass",ttbarMass);

    SetValue("allTopsID",allTopsID);
    SetValue("allTopsPt",allTopsPt);
    SetValue("allTopsEta",allTopsEta);
    SetValue("allTopsPhi",allTopsPhi);
    SetValue("allTopsStatus",allTopsStatus);
    SetValue("allTopsEnergy",allTopsEnergy);
     
    return 0;
}

