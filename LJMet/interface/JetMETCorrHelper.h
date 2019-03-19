#ifndef FWLJMET_LJMet_interface_JetMETCorrHelper_h
#define FWLJMET_LJMet_interface_JetMETCorrHelper_h


// Helper functions originally in BaseEventSelector.cc -- Mar 19, 2019.

#include "FWLJMET/LJMet/interface/BaseEventSelector.h"
#include "FWLJMET/LJMet/interface/LjmetFactory.h"


#include "DataFormats/PatCandidates/interface/Jet.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <TRandom3.h>
#include <regex>


class JetMETCorrHelper{

    public:
    	JetMETCorrHelper();
    	JetMETCorrHelper(const edm::ParameterSet& iConfig, bool isMc);
    	
    	void Initialize(const edm::ParameterSet& iConfig, bool isMc);
        
        void SetFacJetCorr(edm::EventBase const & event);
        
        TLorentzVector correctJet(const pat::Jet & jet,
						  edm::Event const & event,
						  bool isMc,
						  edm::EDGetTokenT<double> rhoJetsToken,
						  bool doAK8Corr=false,
						  bool reCorrectJet=false,
						  unsigned int syst=0);
						  
						  
		pat::Jet correctJetReturnPatJet(const pat::Jet & jet,
								edm::Event const & event,
								bool isMc,
								edm::EDGetTokenT<double> rhoJetsToken,
								bool doAK8Corr=false,
								bool reCorrectJet=false,
								unsigned int syst=0);
								
		TLorentzVector correctMet(const pat::MET & met, 
						  edm::Event const & event,
						  bool isMc, 
						  edm::EDGetTokenT<double> rhoJetsToken,
						  std::vector<edm::Ptr<pat::Jet>> vAllJets,
						  bool reCorrectjet = false, 
						  unsigned int syst = 0, 
						  bool useHF = true);
						  
						  
		TLorentzVector correctJetForMet(const pat::Jet & jet, 
								edm::Event const & event,
								bool isMc, 
								edm::EDGetTokenT<double> rhoJetsToken,
								unsigned int syst = 0);

    private:
            
        bool debug=true;

        std::string mLegend;
        
        TRandom3 JERrand;

		JetCorrectionUncertainty *jecUnc;

		JME::JetResolution resolution;
		JME::JetResolution resolutionAK8;
		JME::JetResolutionScaleFactor resolution_SF;

		std::vector<JetCorrectorParameters> vPar;
		std::vector<JetCorrectorParameters> vParAK8;
		FactorizedJetCorrector *JetCorrector;
		FactorizedJetCorrector *JetCorrectorAK8;

		std::map<std::string,std::string> mJetParStr;
	
		std::map<std::string, std::vector<JetCorrectorParameters> > mEraVPar;
		std::map<std::string, std::vector<JetCorrectorParameters> > mEraVParAK8;    
		std::map<std::string, JetCorrectorParameters*> mStrJetCorPar; 

		std::map<std::string, std::map<std::string, JetCorrectorParameters*> > mEra_mStrJetCorPar; 
		std::map<std::string, std::map<std::string, std::string>>              mEraJetParStr;
		std::map<std::string, std::string> mEraReplaceStr;
	
		std::map<std::string, FactorizedJetCorrector*> mEraFacJetCorr;
		std::map<std::string, FactorizedJetCorrector*> mEraFacJetCorrAK8;

};

#endif