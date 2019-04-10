### FWLJMET -- Full Framework LJMET (wrapper)

Currently this is build using CMSSW_9_4_13

install:

	source /cvmfs/cms.cern.ch/cmsset_default.csh
	setenv SCRAM_ARCH slc6_amd64_gcc630
	cmsrel CMSSW_9_4_13
	cd CMSSW_9_4_13/src/
	cmsenv

	## if wanting to use el ID v2 (for 2017 data):
	git cms-merge-topic cms-egamma:EgammaPostRecoTools

	## HOT tagger
	cd ${CMSSW_BASE}/src
	git cms-merge-topic -u pastika:AddAxis1_946p1
	git clone git@github.com:susy2015/TopTagger.git
	cd ${CMSSW_BASE}/src
	mkdir -p ${CMSSW_BASE}/src/TopTagger/TopTagger/data
	getTaggerCfg.sh -o -n -t DeepResolved_DeepCSV_GR_noDisc_Release_v1.0.0 -d $CMSSW_BASE/src/TopTagger/TopTagger/data


	git clone git@github.com:rsyarif/FWLJMET.git

	scram b

Some info:

- LJMet/plugins/LJMet.cc : the EDAnalyzer that wraps LJMet classes
- el ID V2 : https://twiki.cern.ch/twiki/bin/view/CMS/EgammaMiniAODV2
- HOT tagger : https://github.com/susy2015/TopTagger/tree/master/TopTagger#instructions-for-saving-tagger-results-to-nanoaod-with-cmssw_9_4_11

run LJMet by running:

    cmsRun LJMet/runFWLJMet_multiLep.py
