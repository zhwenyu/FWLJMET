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

	## HOT tagger part1
	cd ${CMSSW_BASE}/src
	git cms-merge-topic -u pastika:AddAxis1_946p1
	git clone git@github.com:susy2015/TopTagger.git

	### BestCalc: copy lwtnn so that BestCalc.cc will compile. ( This is bad practice, should always try to get official CMSSW recipes whenever possible)
	cd ${CMSSW_BASE}/src
	cp -r ~jmanagan/nobackup/CMSSW_9_4_12/src/lwtnn .   ## use scp after a Fermilab kinit to copy onto non-LPC clusters


	## FWLJMET code
	git clone git@github.com:rsyarif/FWLJMET.git


	## JetSubCalc currently uses uses PUPPI mass corrections:
	cd ${CMSSW_BASE}/src/FWLJMET/LJMet/data/
	git clone https://github.com/thaarres/PuppiSoftdropMassCorr

	cd -

	#Compile
	scram b

	## HOT tagger part2
	cd ${CMSSW_BASE}/src
	mkdir -p ${CMSSW_BASE}/src/TopTagger/TopTagger/data
	getTaggerCfg.sh -o -n -t DeepResolved_DeepCSV_GR_noDisc_Release_v1.0.0 -d $CMSSW_BASE/src/TopTagger/TopTagger/data


Some info:

- LJMet/plugins/LJMet.cc : the EDAnalyzer that wraps LJMet classes
- el ID V2 : https://twiki.cern.ch/twiki/bin/view/CMS/EgammaMiniAODV2
- HOT tagger : https://github.com/susy2015/TopTagger/tree/master/TopTagger#instructions-for-saving-tagger-results-to-nanoaod-with-cmssw_9_4_11
- BoostedEventShapeTagger(BEST) :
     - https://bregnery.github.io/docs/BESTstandaloneTutorial/
     - https://github.com/justinrpilot/BESTAnalysis/tree/master
     - https://github.com/demarley/lwtnn/tree/CMSSW_8_0_X-compatible#cmssw-compatibility


run LJMet:

    cmsRun LJMet/runFWLJMet_multiLep.py (or runFWLJMet_singleLep.py)
