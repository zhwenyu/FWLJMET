import os, argparse, imp

parser = argparse.ArgumentParser()
parser.add_argument("--finalState",action="store")
option = parser.parse_args()

#Sample list file
sampleListPath = "sample_list_"+option.finalState+".py"
sample = imp.load_source("Sample",sampleListPath,open(sampleListPath,"r"))

CRABCONFIG_DIR      = 'crabConfigs_'+option.finalState

def submit_multiple_crab_jobs(sample_dict):

	for dataset in sample_dict:

		print 'Submitting ',dataset,':',sample_dict[dataset]

		crab_cfg = CRABCONFIG_DIR+'/crab_FWLJMET_cfg_'+dataset+'.py'

		#os.system('echo crab submit '+crab_cfg)
		os.system('crab submit '+crab_cfg)


if __name__ == '__main__':

	submit_multiple_crab_jobs(sample.signaldict)
	submit_multiple_crab_jobs(sample.bkgdict)
	submit_multiple_crab_jobs(sample.datadict)
