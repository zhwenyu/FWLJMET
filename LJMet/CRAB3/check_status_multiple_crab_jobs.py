import os, argparse, imp

parser = argparse.ArgumentParser()
parser.add_argument("--finalState",action="store", default="multiLep")
parser.add_argument("--year",action="store", default="2018")
parser.add_argument("--resubmit",action="store_true", default=False)
parser.add_argument("--report",action="store_true", default=False)
parser.add_argument("--kill",action="store_true", default=False)
option = parser.parse_args()

#Sample list file
sampleListPath = "sample_list_"+option.finalState+option.year+".py"
sample = imp.load_source("Sample",sampleListPath,open(sampleListPath,"r"))

CRABSUBMIT_DIR      = 'crabSubmitLogs'

def check_status_multiple_crab_jobs(sample_dict):

        for dataset in sample_dict:

                crab_submit_dir = CRABSUBMIT_DIR+'/'+option.finalState+option.year+'/crab_'+option.finalState+option.year+'_'+dataset+'/'
                                
                if(option.resubmit):
                        print '\nAttempting to resubmit ',dataset,':',sample_dict[dataset]
                        os.system('crab resubmit '+crab_submit_dir)

                elif(option.report):
                        print '\Requesting report of ',dataset,':',sample_dict[dataset]
                        os.system('crab report '+crab_submit_dir)

                elif(option.kill):
                        print '\nAttempting to kill ',dataset,':',sample_dict[dataset]
                        os.system('crab kill '+crab_submit_dir)

                else:
                        print '\nChecking status ',dataset,':',sample_dict[dataset]
                        os.system('crab status '+crab_submit_dir)



if __name__ == '__main__':

        check_status_multiple_crab_jobs(sample.signaldict)
        check_status_multiple_crab_jobs(sample.bkgdict)
        check_status_multiple_crab_jobs(sample.ttbarbkgdict)
        check_status_multiple_crab_jobs(sample.datadict)

