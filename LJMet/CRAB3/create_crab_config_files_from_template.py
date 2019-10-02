import os, sys, argparse, imp
import datetime
cTime=datetime.datetime.now()
date_str='%i_%i_%i'%(cTime.year,cTime.month,cTime.day)

userName = os.environ['USER']
relBase = os.environ['CMSSW_BASE']
home = os.environ['HOME']

parser = argparse.ArgumentParser()
parser.add_argument("--finalState",action="store")
parser.add_argument("--year",action="store")
parser.add_argument("--nominalTreeOnly",action="store_true")
option = parser.parse_args()

#Sample list file
sampleListPath = "sample_list_"+option.finalState+option.year+".py"
sample = imp.load_source("Sample",sampleListPath,open(sampleListPath,"r"))

####################
### SET YOUR STRINGS
####################
#cmsRun config
runScript = option.finalState+option.year if option.nominalTreeOnly else option.finalState+option.year+'_multipleTree'

CMSRUNCONFIG        = '../runFWLJMet_'+runScript+'.py'
#CMSRUNCONFIG        = relBase+'/src/FWLJMET/LJMet/runFWLJMet_'+runScript+'.py'  #JH: crab stuff in nobackup

#folder to save the created crab configs
CRABCONFIG_DIR      = 'crabConfigs_'+option.finalState+option.year
#CRABCONFIG_DIR      = home+'/nobackup/FWLJMET102X_crabConfigs_'+option.finalState+option.year  #JH: crab stuff in nobackup

#the crab cfg template to copy from
CRABCONFIG_TEMPLATE = 'crab_FWLJMET_cfg_template.py'

#crab request name
REQNAME             = option.finalState+option.year

#eos out folder
OUTFOLDER           = 'FWLJMET102X_1lep2018_070919'  # special 4tops
#OUTFOLDER           = 'FWLJMET102X_1lep2017_052219'  #JH: single lepton 2017

#log folder
LOGFOLDER           = 'FWLJMET_crab_test' ## JH: this is not actually used in the sed commands below, dummy variable

#JSON for Data
JSONFORDATA         = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/Final/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt' #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2017Analysis#DATA
if option.year == '2018': 
	JSONFORDATA = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt' #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2018Analysis#DATA

def create_crab_config_files_from_template(sample_dict,**kwargs):

	for dataset in sample_dict:

		print dataset,sample_dict[dataset]

		filename = 'crab_FWLJMET_cfg_'+dataset+'.py'
		cmsRunname = 'run_FWLJMET_'+dataset+'.py'

		#copy template file to new directory
		os.system('cp -v '+CRABCONFIG_TEMPLATE+' '+CRABCONFIG_DIR+'/'+filename)
		os.system('cp -v '+CMSRUNCONFIG+' '+CRABCONFIG_DIR+'/'+cmsRunname)

		#replace strings in new crab file
		os.system("sed -i 's|CMSRUNCONFIG|"+CRABCONFIG_DIR+"/"+cmsRunname+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|INPUT|"+sample_dict[dataset]+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|REQNAME|"+REQNAME+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|OUTFOLDER|"+OUTFOLDER+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|LOGFOLDER|"+dataset+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|JSONFORDATA|"+JSONFORDATA+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|ISMC|"+kwargs['ISMC']+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|ISVLQSIGNAL|"+kwargs['ISVLQSIGNAL']+"|g' "+CRABCONFIG_DIR+"/"+filename)

		#replace strings in new cmsRun file
		if 'EGamma' in dataset or 'Single' in dataset:
			os.system("sed -i 's|DATASET|"+dataset+"|g' "+CRABCONFIG_DIR+"/"+cmsRunname)
		else:
			os.system("sed -i 's|DATASET|"+sample_dict[dataset].split('/')[1]+"|g' "+CRABCONFIG_DIR+"/"+cmsRunname)
		os.system("sed -i 's|ISMC|"+kwargs['ISMC']+"|g' "+CRABCONFIG_DIR+"/"+cmsRunname)
		os.system("sed -i 's|ISVLQSIGNAL|"+kwargs['ISVLQSIGNAL']+"|g' "+CRABCONFIG_DIR+"/"+cmsRunname)		
		os.system("sed -i 's|ISTTBAR|"+kwargs['ISTTBAR']+"|g' "+CRABCONFIG_DIR+"/"+cmsRunname)


if __name__=='__main__':

	os.system('mkdir -vp '+CRABCONFIG_DIR)

	#### Bkg MC - no ttbar
#	create_crab_config_files_from_template(
#		sample.bkgdict,
#		ISMC='True',
#		ISVLQSIGNAL='False',
#		ISTTBAR='False',
#		)
#
#	#### Bkg MC - ttbar
	create_crab_config_files_from_template(
 		sample.ttbarbkgdict,
 		ISMC='True',
 		ISVLQSIGNAL='False',
 		ISTTBAR='True',
 		)
#     #### fourtops MC
#        create_crab_config_files_from_template(
#                sample.fourtopssigdict,
#                ISMC='True',
#                ISVLQSIGNAL='False',
#                ISTTBAR='False',
#                )
        create_crab_config_files_from_template(
                sample.fourtopsttdict,
                ISMC='True',
                ISVLQSIGNAL='False',
                ISTTBAR='True',
                )
#        create_crab_config_files_from_template(
#                sample.fourtopsbkgdict,
#                ISMC='True',
#                ISVLQSIGNAL='False',
#                ISTTBAR='False',
#                )
#
#	#### VLQ signal MC
#	create_crab_config_files_from_template(
#		sample.signaldict,
#		ISMC='True',
#		ISVLQSIGNAL='True',
#		ISTTBAR='False',
#		)
#
#	#### Data
#	create_crab_config_files_from_template(
#		sample.datadict,
#		ISMC='False',
#		ISVLQSIGNAL='False',
#		ISTTBAR='False',
#		)
