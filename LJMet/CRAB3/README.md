### Submitting a crab job

Always good to do a dryrun first:

       crab submit --dryrun crab_FWLJMET_cfg.py

### Submitting multiple crab jobs: modify files accordingly and run below

    python create_crab_config_files_from_template.py

    pythion submit_multiple_crab_jobs,py


Note:
 * sample_list_XX.py contain the dataset lists.
 * crab_script.sh -- is used when running VLQ samples. This resets environment variable in order to access LHApdf files not in CMSSW. This is run using scripExe in the crab cfg.