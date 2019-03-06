### FWLJMET -- Full Framework LJMET (wrapper)

Currently this is build using CMSSW_9_4_12

install:

	source /cvmfs/cms.cern.ch/cmsset_default.csh
	setenv SCRAM_ARCH slc6_amd64_gcc630
	cmsrel CMSSW_9_4_12
	cd CMSSW_9_4_12/src/
	cmsenv

	git clone git@github.com:rsyarif/FWLJMET.git

	scram b

Some info:

- LJMet/plugins/LJMet.cc : the EDAnalyzer that wraps LJMet classes

run LJMet by running:

    cmsRun LJMet/runFWLJMet.py

If needed SkimMiniAOD ( EDFilter ) can be downloaded here: git clone git@github.com:rsyarif/PreLJMET.git PreLJMet/