#include "FWLJMET/LJMet/interface/MiniIsolation.h"
#include "DataFormats/Math/interface/deltaR.h"

// Source:
// https://hypernews.cern.ch/HyperNews/CMS/get/susy/1991.html
// https://github.com/manuelfs/CfANtupler/blob/master/minicfa/interface/miniAdHocNTupler.h#L54


double getPFMiniIsolation_DeltaBeta(edm::Handle<pat::PackedCandidateCollection> pfcands,
		      const reco::Candidate* ptcl,  
		      double r_iso_min, double r_iso_max, double kt_scale,
		      bool charged_only) {

  if (ptcl->pt()<5.) return 99999.;

  double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.), deadcone_pu(0.);
  if(ptcl->isElectron()) {
    if (fabs(ptcl->eta())>1.479) {deadcone_ch = 0.015; deadcone_pu = 0.015; deadcone_ph = 0.08;}
  } else if(ptcl->isMuon()) {
    deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01;  
  } else {
    //deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01; // maybe use muon cones??
  }

  double iso_nh(0.); double iso_ch(0.); 
  double iso_ph(0.); double iso_pu(0.);
  double ptThresh(0.5);
  if(ptcl->isElectron()) ptThresh = 0;
  double r_iso = std::max(r_iso_min,std::min(r_iso_max, kt_scale/ptcl->pt()));
  for (const pat::PackedCandidate &pfc : *pfcands) {
    if (abs(pfc.pdgId())<7) continue;

    double dr = reco::deltaR(pfc, *ptcl);
    if (dr > r_iso) continue;
      
    //////////////////  NEUTRALS  /////////////////////////
    if (pfc.charge()==0){
      if (pfc.pt()>ptThresh) {
	/////////// PHOTONS ////////////
	if (abs(pfc.pdgId())==22) {
	  if(dr < deadcone_ph) continue;
	  iso_ph += pfc.pt();
	  /////////// NEUTRAL HADRONS ////////////
	    } else if (abs(pfc.pdgId())==130) {
	  if(dr < deadcone_nh) continue;
	  iso_nh += pfc.pt();
	}
      }
      //////////////////  CHARGED from PV  /////////////////////////
    } else if (pfc.fromPV()>1){
      if (abs(pfc.pdgId())==211) {
	if(dr < deadcone_ch) continue;
	iso_ch += pfc.pt();
      }
      //////////////////  CHARGED from PU  /////////////////////////
    } else {
      if (pfc.pt()>ptThresh){
	if(dr < deadcone_pu) continue;
	iso_pu += pfc.pt();
      }
    }
  }
  double iso(0.);
  if (charged_only){
    iso = iso_ch;
  } else {
    iso = iso_ph + iso_nh;
    iso -= 0.5*iso_pu;
    if (iso>0) iso += iso_ch;
    else iso = iso_ch;
  }
  iso = iso/ptcl->pt();

  return iso;
}

double getPFMiniIsolation_EffectiveArea(edm::Handle<pat::PackedCandidateCollection> pfcands,
					const reco::Candidate* ptcl,  
					double r_iso_min, double r_iso_max, double kt_scale,
					bool use_pfweight, bool charged_only, double rho) {

  if (ptcl->pt()<5.) return 99999.;

  double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.), deadcone_pu(0.);
  if(ptcl->isElectron()) {
    if (fabs(dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta())>1.479) {deadcone_ch = 0.015; deadcone_pu = 0.015; deadcone_ph = 0.08;}
  } else if(ptcl->isMuon()) {
    deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01;  
  } else {
    //deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01; // maybe use muon cones??
  }

  double iso_nh(0.); double iso_ch(0.); 
  double iso_ph(0.); double iso_pu(0.);
  double ptThresh(0.5);
  if(ptcl->isElectron()) ptThresh = 0;
  double r_iso = std::max(r_iso_min,std::min(r_iso_max, kt_scale/ptcl->pt()));
  for (const pat::PackedCandidate &pfc : *pfcands) {
    if (abs(pfc.pdgId())<7) continue;

    double dr = deltaR(pfc, *ptcl);
    if (dr > r_iso) continue;
      
    //////////////////  NEUTRALS  /////////////////////////
    if (pfc.charge()==0){
      if (pfc.pt()>ptThresh) {
	double wpf(1.);
	/*if (use_pfweight){
            double wpv(0.), wpu(0.);
            for (const pat::PackedCandidate &jpfc : *pfcands) {
              double jdr = deltaR(pfc, jpfc);
              if (pfc.charge()!=0 || jdr<0.00001) continue;
              double jpt = jpfc.pt();
              if (pfc.fromPV()>1) wpv *= jpt/jdr;
              else wpu *= jpt/jdr;
            }
            wpv = log(wpv);
            wpu = log(wpu);
            wpf = wpv/(wpv+wpu);
	    }*/
	/////////// PHOTONS ////////////
	if (abs(pfc.pdgId())==22) {
	  if(dr < deadcone_ph) continue;
	  iso_ph += wpf*pfc.pt();
          /////////// NEUTRAL HADRONS ////////////
	} else if (abs(pfc.pdgId())==130) {
	  if(dr < deadcone_nh) continue;
	  iso_nh += wpf*pfc.pt();
	}
      }
      //////////////////  CHARGED from PV  /////////////////////////
    } else if (pfc.fromPV()>1){
      if (abs(pfc.pdgId())==211) {
	if(dr < deadcone_ch) continue;
	iso_ch += pfc.pt();
      }
      //////////////////  CHARGED from PU  /////////////////////////
    } else {
      if (pfc.pt()>ptThresh){
	if(dr < deadcone_pu) continue;
	iso_pu += pfc.pt();
      }
    }
  }
  double iso(0.);
  //if (charged_only){
  //  iso = iso_ch;
  //} else {
  //  iso = iso_ph + iso_nh;
  //  if (!use_pfweight) iso -= 0.5*iso_pu;
  //  if (iso>0) iso += iso_ch;
  //  else iso = iso_ch;
  //}
  
  int em = 0;
  if(ptcl->isMuon())
    em = 1;
  
  //double Aeff[2][5] = {{ 0.1013, 0.0988, 0.0572, 0.0842, 0.1530 },{ 0.0913, 0.0765, 0.0546, 0.0728, 0.1177 }}; // ancient SUSY
  
  // used for 2016 analyses based on SUSY group recommendations on SUSLeptonSF TWiki
  double Aeff_Fall15Anal[2][7] = {{ 0.1752, 0.1862, 0.1411, 0.1534, 0.1903 , 0.2243, 0.2687 },{ 0.0735, 0.0619, 0.0465, 0.0433, 0.0577 , 0.0,0.0}};
  // Possible values that we might want when rerunning 2016....SUSY keeps the 2015 versions
  //https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt
  double Aeff_Summer16Anal[2][7] = {{ 0.1703, 0.1715, 0.1213, 0.1230, 0.1635, 0.1937, 0.2393 },{ 0.0735, 0.0619, 0.0465, 0.0433, 0.0577 , 0.0,0.0}};
	
  // JH 8/22/19, updating to 94X values from 92X values for electrons, a very small change (2-3rd decimal place)
  // https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt
  // Muon values might be old but also are no longer used, in favor of the built-in flag 
  double Aeff_Fall17Anal[2][7] = {{ 0.1440, 0.1562, 0.1032, 0.0859, 0.1116, 0.1321, 0.1654 },{ 0.0735, 0.0619, 0.0465, 0.0433, 0.0577 , 0.0,0.0}};
  
  double CorrectedTerm=0.0;
  double riso2 = r_iso*r_iso;
  
  if(em){
    if( TMath::Abs( ptcl->eta() ) < 0.8 ) CorrectedTerm = rho * Aeff_Fall17Anal[1][ 0 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 0.8 && TMath::Abs( ptcl->eta() ) < 1.3  )   CorrectedTerm = rho * Aeff_Fall17Anal[1][ 1 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 1.3 && TMath::Abs( ptcl->eta() ) < 2.0  )   CorrectedTerm = rho * Aeff_Fall17Anal[1][ 2 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 2.0 && TMath::Abs( ptcl->eta() ) < 2.2  )   CorrectedTerm = rho * Aeff_Fall17Anal[1][ 3 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 2.2 && TMath::Abs( ptcl->eta() ) < 2.5  )   CorrectedTerm = rho * Aeff_Fall17Anal[1][ 4 ]*(riso2/0.09);
  }
  else{
    if( TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) < 1.0 ) CorrectedTerm = rho * Aeff_Fall17Anal[0][ 0 ]*(riso2/0.09);
    else if( TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) > 1.0 && TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) < 1.479  )   CorrectedTerm = rho * Aeff_Fall17Anal[0][ 1 ]*(riso2/0.09);
    else if( TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) > 1.479 && TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) < 2.0  )   CorrectedTerm = rho * Aeff_Fall17Anal[0][ 2 ]*(riso2/0.09);
    else if( TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) > 2.0 && TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) < 2.2  )   CorrectedTerm = rho * Aeff_Fall17Anal[0][ 3 ]*(riso2/0.09);
    else if( TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) > 2.2 && TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) < 2.3  )   CorrectedTerm = rho * Aeff_Fall17Anal[0][ 4 ]*(riso2/0.09);
    else if( TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) > 2.3 && TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) < 2.4  )   CorrectedTerm = rho * Aeff_Fall17Anal[0][ 5 ]*(riso2/0.09);
    else if( TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) > 2.4 && TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) > 2.4 && TMath::Abs( dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta() ) < 2.5  )   CorrectedTerm = rho * Aeff_Fall17Anal[0][ 6 ]*(riso2/0.09);
  }
  //std::cout<<"riso = "<<r_iso<<", iso_nh = "<<iso_nh<<", iso_ch = "<<iso_ch<<", iso_ph = "<<iso_ph<<",rho = "<<rho<<" adn CTerm = "<<CorrectedTerm<<"\n";  
   
  iso = iso_ch + TMath::Max(0.0, iso_ph + iso_nh - CorrectedTerm );
  
  
  iso = iso/ptcl->pt();

  return iso;
}

double getPFMiniIsolation_SUSY(edm::Handle<pat::PackedCandidateCollection> pfcands,
			     const reco::Candidate* ptcl,
			     double r_iso_min, double r_iso_max, double kt_scale,
			     bool use_pfweight, bool charged_only, double rho) {
    
  if (ptcl->pt()<5.) return 99999.;
    
  double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.), deadcone_pu(0.);
  if(ptcl->isElectron()) {
    if (fabs(dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta())>1.479) {deadcone_ch = 0.015; deadcone_pu = 0.015; deadcone_ph = 0.08;}
  } else if(ptcl->isMuon()) {
    deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01;
  } else {
    //deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01; // maybe use muon cones??
  }
    
  double iso_nh(0.); double iso_ch(0.);
  double iso_ph(0.); double iso_pu(0.);
  double ptThresh(0.5);
  if(ptcl->isElectron()) ptThresh = 0;
  double r_iso = std::max(r_iso_min,std::min(r_iso_max, kt_scale/ptcl->pt()));
  for (const pat::PackedCandidate &pfc : *pfcands) {
    if (abs(pfc.pdgId())<7) continue;
        
    double dr = deltaR(pfc, *ptcl);
    if (dr > r_iso) continue;
        
    //////////////////  NEUTRALS  /////////////////////////
    if (pfc.charge()==0){
      if (pfc.pt()>ptThresh) {
	double wpf(1.);
	if (use_pfweight){
	  double wpv(0.), wpu(0.);
	  for (const pat::PackedCandidate &jpfc : *pfcands) {
	    double jdr = deltaR(pfc, jpfc);
	    if (pfc.charge()!=0 || jdr<0.00001) continue;
	    double jpt = jpfc.pt();
	    if (pfc.fromPV()>1) wpv *= jpt/jdr;
	    else wpu *= jpt/jdr;
	  }
	  wpv = log(wpv);
	  wpu = log(wpu);
	  wpf = wpv/(wpv+wpu);
	}
	/////////// PHOTONS ////////////
	if (abs(pfc.pdgId())==22) {
	  if(dr < deadcone_ph) continue;
	  iso_ph += wpf*pfc.pt();
	  /////////// NEUTRAL HADRONS ////////////
	} else if (abs(pfc.pdgId())==130) {
	  if(dr < deadcone_nh) continue;
	  iso_nh += wpf*pfc.pt();
	}
      }
      //////////////////  CHARGED from PV  /////////////////////////
    } else if (pfc.fromPV()>1){
      if (abs(pfc.pdgId())==211) {
	if(dr < deadcone_ch) continue;
	iso_ch += pfc.pt();
      }
      //////////////////  CHARGED from PU  /////////////////////////
    } else {
      if (pfc.pt()>ptThresh){
	if(dr < deadcone_pu) continue;
	iso_pu += pfc.pt();
      }
    }
  }
  double iso(0.);
    
  int em = 0;
  if(ptcl->isMuon())
    em = 1;
    

  double Aeff[2][7] = {{ 0.1752, 0.1862, 0.1411, 0.1534, 0.1903, 0.2243, 0.2687 },{ 0.0735, 0.0619, 0.0465, 0.0433, 0.0577,0,0 }};
    
    
  double CorrectedTerm=0.0;
  double riso2 = r_iso*r_iso;
  if(ptcl->isMuon()) {
    if( TMath::Abs( ptcl->eta() ) < 0.8 ) CorrectedTerm = rho * Aeff[em][ 0 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 0.8 && TMath::Abs( ptcl->eta() ) < 1.3  )   CorrectedTerm = rho * Aeff[em][ 1 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 1.3 && TMath::Abs( ptcl->eta() ) < 2.0  )   CorrectedTerm = rho * Aeff[em][ 2 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 2.0 && TMath::Abs( ptcl->eta() ) < 2.2  )   CorrectedTerm = rho * Aeff[em][ 3 ]*(riso2/0.09);
    else if( TMath::Abs( ptcl->eta() ) > 2.2 && TMath::Abs( ptcl->eta() ) < 2.5  )   CorrectedTerm = rho * Aeff[em][ 4 ]*(riso2/0.09);
  } else {
    double elEta = fabs(dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta());
    if( elEta < 1.0 ) CorrectedTerm = rho * Aeff[em][ 0 ]*(riso2/0.09);
    else if( elEta > 1.0 && elEta < 1.479  )   CorrectedTerm = rho * Aeff[em][ 1 ]*(riso2/0.09);
    else if( elEta > 1.479 && elEta < 2.0  )   CorrectedTerm = rho * Aeff[em][ 2 ]*(riso2/0.09);
    else if( elEta > 2.0 && elEta < 2.2  )   CorrectedTerm = rho * Aeff[em][ 3 ]*(riso2/0.09);
    else if( elEta > 2.2 && elEta < 2.3  )   CorrectedTerm = rho * Aeff[em][ 4 ]*(riso2/0.09);
    else if( elEta > 2.3 && elEta < 2.4  )   CorrectedTerm = rho * Aeff[em][ 5 ]*(riso2/0.09);
    else if( elEta > 2.4 && elEta < 2.5  )   CorrectedTerm = rho * Aeff[em][ 6 ]*(riso2/0.09);
  }
    
  if (charged_only){
    iso = iso_ch;
  } else {

    iso = iso_ph + iso_nh;

    iso -= CorrectedTerm; //EA PU correction
    if (iso>0) iso += iso_ch;
    else iso = iso_ch;
  }
  iso = iso/ptcl->pt();
    
  return iso;
}
