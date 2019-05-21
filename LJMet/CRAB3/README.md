       source /cvmfs/cms.cern.ch/crab3/crab.sh

### Submitting a crab job

Always good to do a dryrun first (will show if it could be successfully delivered to the CRAB server):

       crab submit --dryrun crab_FWLJMET_cfg.py

### Submitting multiple crab jobs (with options of --finalState & --nominalTreeOnly): 1Lep

       python create_crab_config_files_from_template.py --finalState singleLep (--nominalTreeOnly)

       python submit_mutiple_crab_jobs.py --finalState singleLep (--nominalTreeOnly)
### Or: 3Lep

       python create_crab_config_files_from_template.py --finalState multiLep (--nominalTreeOnly)

       python submit_mutiple_crab_jobs.py --finalState multiLep (--nominalTreeOnly)


Note:
 * sample_list_XX.py contain the dataset lists.
 * crab_script.sh -- is used when running VLQ samples. This resets environment variable in order to access LHApdf files not in CMSSW. This is run using scripExe in the crab cfg.
