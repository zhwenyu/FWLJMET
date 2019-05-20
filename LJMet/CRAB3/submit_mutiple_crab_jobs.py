import os

#Sample list file
import sample_list_3L as sample

CRABCONFIG_DIR      = 'crabConfigs_3L'

def submit_multiple_crab_jobs(sample_dict):

	for dataset in sample_dict:

		print 'Submitting ',dataset,':',sample_dict[dataset] 
	
		crab_cfg = CRABCONFIG_DIR+'/crab_FWLJMET_cfg_'+dataset+'.py'
		
		os.system('echo crab submit '+crab_cfg)
		
if __name__ == '__main__':

	submit_multiple_crab_jobs(sample.signaldict)
	submit_multiple_crab_jobs(sample.bkgdict)
	submit_multiple_crab_jobs(sample.datadict2018)
