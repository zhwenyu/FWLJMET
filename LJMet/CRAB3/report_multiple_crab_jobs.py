import os, argparse, imp

parser = argparse.ArgumentParser()
parser.add_argument("--finalState",action="store", default="multiLep")
option = parser.parse_args()

#Sample list file
sampleListPath = "sample_list_"+option.finalState+".py"
sample = imp.load_source("Sample",sampleListPath,open(sampleListPath,"r"))

CRABSUBMIT_DIR      = 'crabSubmitLogs'

def check_status_multiple_crab_jobs(sample_dict):

	for dataset in sample_dict:

		crab_submit_dir = CRABSUBMIT_DIR+'/'+option.finalState+'/crab_'+option.finalState+'_'+dataset+'/'

		print '\nGenerating report for ',dataset,':',sample_dict[dataset]
		os.system('crab report '+crab_submit_dir)

if __name__ == '__main__':

	check_status_multiple_crab_jobs(sample.signaldict)
	check_status_multiple_crab_jobs(sample.bkgdict)
	check_status_multiple_crab_jobs(sample.ttbarbkgdict)
	check_status_multiple_crab_jobs(sample.datadict)
