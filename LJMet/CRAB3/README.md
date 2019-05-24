### Set environment for CRAB3

       source /cvmfs/cms.cern.ch/crab3/crab.sh

### Configure the crab jobs

Check the following variables in create_crab_config_files_from_template.py. You will need to set OUTFOLDER, likely the others are ok as is.
The default behavior is to create crab configs and store logs inside this CRAB3 directory. Commented options for some variables are available to move these logs to your nobackup area (also check crab_FWLJMET_cfg_template.py if you want to use these option). 

CMSRUNCONFIG        
CRABCONFIG_DIR      
CRABCONFIG_TEMPLATE 
REQNAME             
OUTFOLDER           

Check crab_FWLJMET_cfg_template.py to understand it and look for any necessary changes to the job splitting or master eos folder ("lpcljm").
Default behavior:
  -- Memory: 4000 MB. This is supposed to be "per core" (default 4) but it seems to be the total based on test jobs
  -- General MC: file-based splitting, 1 file per job. Should be safe, but a dryrun is recommended! Some heavy jobs may need lumi-based splitting instead
  -- VLQ signal: due to recreating the PDF, these jobs can't handle 1 file without going over time and memory. Set to 2 lumi-sections instead. 
  -- Data: "Automatic" splitting, max of 24 hours. This has not been tested as much -- time might need to be shortened if memory failures appear

### Submitting a crab job

Always good to do a dryrun first (will show if it could be successfully delivered to the CRAB server):
First configure crab_FWLJMET_cfg.py with meaningful info: 

       crab submit --dryrun crab_FWLJMET_cfg.py

OR use the "create config" script below and do a dryrun on one of the configs it creates:

       crab submit --dryrun /path/to/your/crab/configs/crab_FWLJMET_cfg_<your dataset>.py


### Submitting multiple crab jobs (with options of --finalState & --year & --nominalTreeOnly): 

#### 1Lep

       python create_crab_config_files_from_template.py --finalState singleLep --year 2017 (--nominalTreeOnly)  # year can be 2018 too

       python submit_mutiple_crab_jobs.py --finalState singleLep --year 2017 (--nominalTreeOnly) # year can be 2018 too

#### 3Lep

       python create_crab_config_files_from_template.py --finalState multiLep --year 2017 (--nominalTreeOnly) # year can be 2018 too

       python submit_mutiple_crab_jobs.py --finalState multiLep --year 2017 (--nominalTreeOnly) # year can be 2018 too


Note:
 * sample_list_XX.py contain the dataset lists.
 * crab_script.sh -- is used when running VLQ samples. This resets environment variable in order to access LHApdf files not in CMSSW. This is run using scripExe in the crab cfg.
