import sample_list_3L as sample
import os, sys
import datetime
cTime=datetime.datetime.now()
date_str='%i_%i_%i'%(cTime.year,cTime.month,cTime.day)


CRABCONFIG_DIR = "TEST_DIR"
CRABCONFIG_TEMPLATE = "crab_FWLJMET_cfg_template.py"

REQNAME = 'FWLJMET_1Lep_'+date_str+'_rizki'
OUTFOLDER = 'FWLJMET_crab_test'

def create_crab_config_files_from_template():

	os.system('mkdir -vp '+CRABCONFIG_DIR)

	#### MC

	ISMC = 'True'
	
	for dataset in sample.bkgdict:

		print dataset,sample.bkgdict[dataset] 
	
		filename = 'crab_FWLJMET_cfg_'+dataset+'.py'

		#copy template file to new directory
		os.system('cp -v '+CRABCONFIG_TEMPLATE+' '+CRABCONFIG_DIR+'/'+filename)
	
		#replace strings in new file
		os.system("sed -i 's|INPUT|"+sample.bkgdict[dataset]+"|g' "+CRABCONFIG_DIR+"/"+filename)

		os.system("sed -i 's|REQNAME|"+REQNAME+"|g' "+CRABCONFIG_DIR+"/"+filename)

		os.system("sed -i 's|OUTFOLDER|"+OUTFOLDER+"|g' "+CRABCONFIG_DIR+"/"+filename)
	
		os.system("sed -i 's|ISMC|"+ISMC+"|g' "+CRABCONFIG_DIR+"/"+filename)
		
		
if __name__=='__main__':

	
	create_crab_config_files_from_template()

