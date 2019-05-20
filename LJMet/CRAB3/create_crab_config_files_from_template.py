import sample_list_3L as sample
import os, sys
import datetime
cTime=datetime.datetime.now()
date_str='%i_%i_%i'%(cTime.year,cTime.month,cTime.day)

####################
### SET YOUR STRINGS 
####################
#cmsRun config
CMSRUNCONFIG        = '../runFWLJMet_multiLep_multipleTree.py'
#folder to save the created crab configs
CRABCONFIG_DIR      = 'crabConfigs_3L'
#the crab cfg template to copy from
CRABCONFIG_TEMPLATE = 'crab_FWLJMET_cfg_template.py'
#crab request name
REQNAME             = 'FWLJMET_3Lep_'+date_str+'_rizki'
#eos out folder
OUTFOLDER           = 'FWLJMET_crab_test'
#JSON for Data
JSONFORDATA         = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt' #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2018Analysis#DATA

def create_crab_config_files_from_template(sample_dict,**kwargs):

	for dataset in sample_dict:

		print dataset,sample_dict[dataset] 
	
		filename = 'crab_FWLJMET_cfg_'+dataset+'.py'

		#copy template file to new directory
		os.system('cp -v '+CRABCONFIG_TEMPLATE+' '+CRABCONFIG_DIR+'/'+filename)
		
		#replace strings in new file
		os.system("sed -i 's|CMSRUNCONFIG|"+CMSRUNCONFIG+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|INPUT|"+sample_dict[dataset]+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|REQNAME|"+REQNAME+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|OUTFOLDER|"+OUTFOLDER+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|ISMC|"+kwargs['ISMC']+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|ISVLQSIGNAL|"+kwargs['ISVLQSIGNAL']+"|g' "+CRABCONFIG_DIR+"/"+filename)
		os.system("sed -i 's|ISTTBAR|"+kwargs['ISTTBAR']+"|g' "+CRABCONFIG_DIR+"/"+filename)

		
if __name__=='__main__':

	os.system('mkdir -vp '+CRABCONFIG_DIR)
	
	#### Bkg MC - no ttbar
	create_crab_config_files_from_template(
		sample.bkgdict,
		ISMC='True',
		ISVLQSIGNAL='False',
		ISTTBAR='False',
		)

	#### Bkg MC - ttbar
# 	create_crab_config_files_from_template(
# 		sample.bkg_ttbar_dict,
# 		ISMC='True',
# 		ISVLQSIGNAL='False',
# 		ISTTBAR='True',
# 		)

	#### VLQ signal MC
	create_crab_config_files_from_template(
		sample.signaldict,
		ISMC='True',
		ISVLQSIGNAL='True',
		ISTTBAR='False',
		)

	#### Data
	create_crab_config_files_from_template(
		sample.datadict2018,
		ISMC='False',
		ISVLQSIGNAL='False',
		ISTTBAR='False',
		)
