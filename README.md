### FWLJMET -- Full Framework LJMET (wrapper)

Currently this is build using CMSSW_9_4_13

install:

	source /cvmfs/cms.cern.ch/cmsset_default.csh
	setenv SCRAM_ARCH slc6_amd64_gcc630
	cmsrel CMSSW_9_4_13
	cd CMSSW_9_4_13/src/
	cmsenv

	#if wanting to use el ID v2 (for 2017 data):
	git cms-merge-topic cms-egamma:EgammaPostRecoTools

	git clone git@github.com:rsyarif/FWLJMET.git

	scram b

Some info:

- LJMet/plugins/LJMet.cc : the EDAnalyzer that wraps LJMet classes

run LJMet by running:

    cmsRun LJMet/runFWLJMet_multiLep.py
