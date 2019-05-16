#include "FWLJMET/LJMet/interface/HOTTaggerCalc.h"



int HOTTaggerCalc::BeginJob(edm::ConsumesCollector && iC){
    
    ak4ptCut_ = mPset.getParameter<double>("ak4PtCut");
    qgTaggerKey_ = mPset.getParameter<std::string>("qgTaggerKey");
    deepCSVBJetTags_ = mPset.getParameter<std::string>("deepCSVBJetTags");
    bTagKeyString_ = mPset.getParameter<std::string>("bTagKeyString");    
    taggerCfgFile_ = mPset.getParameter<edm::FileInPath>("taggerCfgFile").fullPath();
    discriminatorCut_ = mPset.getParameter<double>("discriminatorCut");
    
    //configure the top tagger
    try{
		//For working directory use cfg file location
		size_t splitLocation = taggerCfgFile_.rfind("/");
		std::string workingDir = taggerCfgFile_.substr(0, splitLocation);
		std::string configName = taggerCfgFile_.substr(splitLocation + 1);
		tt.setWorkingDirectory(workingDir);
		tt.setCfgFile(configName);
	}
    catch(const TTException& e){
      //Convert the TTException into a cms::Exception
      throw cms::Exception(e.getFileName() + ":" + std::to_string(e.getLineNumber()) + ", in function \"" + e.getFunctionName() + "\" -- " + e.getMessage());
    }
    
    return 0;
  }


int HOTTaggerCalc::AnalyzeEvent(edm::Event const & event, BaseEventSelector * selector){

  //Get lepton-cleaned and JEC-corrected jet collection
  std::vector<pat::Jet> const & vSelCorrJets = selector->GetSelCorrJets();

  //container holding input jet info for top tagger
  std::vector<Constituent> constituents;
  
  int nAK4 = 0; // to check against the number of jets from singleLepCalc, they should be the same
  for (std::vector<pat::Jet>::const_iterator ijet = vSelCorrJets.begin(); ijet != vSelCorrJets.end(); ijet++){
    int iJet = (int)(ijet-vSelCorrJets.begin());
      
    const pat::Jet jet = *ijet;
      
    //Apply pt cut on jets -- this should do nothing if the cut is left at 20
    if(jet.pt() < ak4ptCut_) continue;
    nAK4++;

    TLorentzVector perJetLVec(jet.p4().X(), jet.p4().Y(), jet.p4().Z(), jet.p4().T());
      
    double qgPtD = jet.userFloat("QGTagger:ptD");
    double qgAxis1 = jet.userFloat("QGTagger:axis1");
    double qgAxis2 = jet.userFloat("QGTagger:axis2");
    double qgMult = static_cast<double>(jet.userInt("QGTagger:mult"));
    double deepCSVb = jet.bDiscriminator((deepCSVBJetTags_+":probb").c_str());
    double deepCSVc = jet.bDiscriminator((deepCSVBJetTags_+":probc").c_str());
    double deepCSVl = jet.bDiscriminator((deepCSVBJetTags_+":probudsg").c_str());
    double deepCSVbb = jet.bDiscriminator((deepCSVBJetTags_+":probbb").c_str());
    double deepCSVcc = jet.bDiscriminator((deepCSVBJetTags_+":probcc").c_str());
    double btag = jet.bDiscriminator(bTagKeyString_.c_str());
    double chargedHadronEnergyFraction = jet.chargedHadronEnergyFraction();
    double neutralHadronEnergyFraction = jet.neutralHadronEnergyFraction();
    double chargedEmEnergyFraction = jet.chargedEmEnergyFraction();
    double neutralEmEnergyFraction = jet.neutralEmEnergyFraction();
    double muonEnergyFraction = jet.muonEnergyFraction();
    double photonEnergyFraction = jet.photonEnergyFraction();
    double electronEnergyFraction = jet.electronEnergyFraction();
    double recoJetsHFHadronEnergyFraction = jet.HFHadronEnergyFraction();
    double recoJetsHFEMEnergyFraction = jet.HFEMEnergyFraction();
    double chargedHadronMultiplicity = jet.chargedHadronMultiplicity();
    double neutralHadronMultiplicity = jet.neutralHadronMultiplicity();
    double photonMultiplicity = jet.photonMultiplicity();
    double electronMultiplicity = jet.electronMultiplicity();
    double muonMultiplicity = jet.muonMultiplicity();
    
    constituents.emplace_back(perJetLVec, btag, 0.0);
    constituents.back().setIndex(iJet);
    constituents.back().setExtraVar("qgMult"                              , qgMult);
    constituents.back().setExtraVar("qgPtD"                               , qgPtD);
    constituents.back().setExtraVar("qgAxis1"                             , qgAxis1);
    constituents.back().setExtraVar("qgAxis2"                             , qgAxis2);
    constituents.back().setExtraVar("recoJetschargedHadronEnergyFraction" , chargedHadronEnergyFraction);
    constituents.back().setExtraVar("recoJetschargedEmEnergyFraction"     , chargedEmEnergyFraction);
    constituents.back().setExtraVar("recoJetsneutralEmEnergyFraction"     , neutralEmEnergyFraction);
    constituents.back().setExtraVar("recoJetsmuonEnergyFraction"          , muonEnergyFraction);
    constituents.back().setExtraVar("recoJetsHFHadronEnergyFraction"      , recoJetsHFHadronEnergyFraction);
    constituents.back().setExtraVar("recoJetsHFEMEnergyFraction"          , recoJetsHFEMEnergyFraction);
    constituents.back().setExtraVar("recoJetsneutralEnergyFraction"       , neutralHadronEnergyFraction);
    constituents.back().setExtraVar("PhotonEnergyFraction"                , photonEnergyFraction);
    constituents.back().setExtraVar("ElectronEnergyFraction"              , electronEnergyFraction);
    constituents.back().setExtraVar("ChargedHadronMultiplicity"           , chargedHadronMultiplicity);
    constituents.back().setExtraVar("NeutralHadronMultiplicity"           , neutralHadronMultiplicity);
    constituents.back().setExtraVar("PhotonMultiplicity"                  , photonMultiplicity);
    constituents.back().setExtraVar("ElectronMultiplicity"                , electronMultiplicity);
    constituents.back().setExtraVar("MuonMultiplicity"                    , muonMultiplicity);
    constituents.back().setExtraVar("DeepCSVb"                            , deepCSVb);
    constituents.back().setExtraVar("DeepCSVc"                            , deepCSVc);
    constituents.back().setExtraVar("DeepCSVl"                            , deepCSVl);
    constituents.back().setExtraVar("DeepCSVbb"                           , deepCSVbb);
    constituents.back().setExtraVar("DeepCSVcc"                           , deepCSVcc);
  }
  
  //run top tagger
  try{
    tt.runTagger(std::move(constituents));
  }
  catch(const TTException& e){
    //Convert the TTException into a cms::Exception
    throw cms::Exception(e.getFileName() + ":" + std::to_string(e.getLineNumber()) + ", in function \"" + e.getFunctionName() + "\" -- " + e.getMessage());
  }
  
  //retrieve the top tagger results object
  const TopTaggerResults& ttr = tt.getResults();

  //get reconstructed top
  const std::vector<TopObject*>& tops = ttr.getTops();
  
  std::vector<double> topPt;
  std::vector<double> topPhi;
  std::vector<double> topEta;
  std::vector<double> topMass;
  std::vector<double> topDRmax;
  std::vector<double> topDThetaMin;
  std::vector<double> topDThetaMax;
  std::vector<double> topDiscriminator;
  std::vector<double> topType;
  std::vector<double> topNconstituents;
  std::vector<int>   topJet1Index;
  std::vector<int>   topJet2Index;
  std::vector<int>   topJet3Index;
  std::vector<double> topBestGenPt;
  std::vector<double> topBestGenPhi;
  std::vector<double> topBestGenEta;
  std::vector<double> topBestGenEnergy;

  int topNtops = tops.size();
  for (unsigned int itop = 0; itop < tops.size(); itop++){
    TopObject *thistop = tops.at(itop);
    topPt.push_back(thistop->p().Pt());
    topPhi.push_back(thistop->p().Phi());
    topEta.push_back(thistop->p().Eta());
    topMass.push_back(thistop->p().M());
    topDRmax.push_back(thistop->getDRmax());
    topDThetaMin.push_back(thistop->getDThetaMin());
    topDThetaMax.push_back(thistop->getDThetaMax());
    topDiscriminator.push_back(thistop->getDiscriminator());
    topType.push_back(thistop->getType());
    topNconstituents.push_back(thistop->getNConstituents());
  
    std::vector<Constituent const*> thistopConsts = thistop->getConstituents();
    topJet1Index.push_back(thistopConsts.at(0)->getIndex());
    topJet2Index.push_back(thistopConsts.at(1)->getIndex());
    topJet3Index.push_back(thistopConsts.at(2)->getIndex());
  
    double pt = -999;
    double phi = -999;
    double eta = -999;
    double energy = -999;
    const TLorentzVector *bestGen = thistop->getBestGenTopMatch();
    if(bestGen){
      pt = bestGen->Pt();
      phi = bestGen->Phi();
      eta = bestGen->Eta();
      energy = bestGen->Energy();
    }
    topBestGenPt.push_back(pt);
    topBestGenPhi.push_back(phi);
    topBestGenEta.push_back(eta);
    topBestGenEnergy.push_back(energy);
  }

  SetValue("topNAK4",     nAK4);
  SetValue("topNtops",        topNtops);
  SetValue("topPt",		  topPt);		  
  SetValue("topPhi",	  topPhi);	  
  SetValue("topEta",	  topEta);	  
  SetValue("topMass",	  topMass);	  
  SetValue("topDRmax",	  topDRmax);	  
  SetValue("topDThetaMin",	  topDThetaMin);	  
  SetValue("topDThetaMax",	  topDThetaMax);	  
  SetValue("topDiscriminator",topDiscriminator);
  SetValue("topType",	  topType);	  
  SetValue("topNconstituents",topNconstituents);
  SetValue("topJet1Index",	  topJet1Index);	  
  SetValue("topJet2Index",	  topJet2Index);	  
  SetValue("topJet3Index",	  topJet3Index);	  
  SetValue("topBestGenPt",	  topBestGenPt);	  
  SetValue("topBestGenPhi",	  topBestGenPhi);	  
  SetValue("topBestGenEta",	  topBestGenEta);	  
  SetValue("topBestGenEnergy",topBestGenEnergy);
  
  return 0;
}

