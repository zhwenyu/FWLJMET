[rsyarif@cmslpc33 LJMet]$ scram b -j8
>> Local Products Rules ..... started
>> Local Products Rules ..... done
>> Entering Package FWLJMET/LJMet
>> Creating project symlinks
>> Compiling edm plugin /uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc
/uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc: In member function 'virtual void TestEventSelector::BeginJob(std::map<std::__cxx11::basic_string<char>, const edm::ParameterSet>)':
/uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc:70:19: error: 'consumes' was not declared in this scope
  triggersToken  = consumes<edm::TriggerResults>("TriggerResults","","HLT");
                   ^~~~~~~~
/uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc:70:47: error: expected primary-expression before '>' token
  triggersToken  = consumes<edm::TriggerResults>("TriggerResults","","HLT");
                                               ^
/uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc:70:66: error: left operand of comma operator has no effect [-Werror=unused-value]
  triggersToken  = consumes<edm::TriggerResults>("TriggerResults","","HLT");
                                                                  ^~
/uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc:70:69: error: right operand of comma operator has no effect [-Werror=unused-value]
  triggersToken  = consumes<edm::TriggerResults>("TriggerResults","","HLT");
                                                                     ^~~~~
/uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc: In member function 'virtual bool TestEventSelector::operator()(const edm::EventBase&, pat::strbitset&)':
/uscms_data/d3/rsyarif/Brown2018/TT_BB_trilepton_LJMET_2017data/BuildingFullFWLjmet/CMSSW_9_4_12/src/FWLJMET/LJMet/plugins/TestEventSelector.cc:105:9: error: 'const class edm::EventBase' has no member named 'getByToken'; did you mean 'getByLabel'?
   event.getByToken(triggersToken,triggersHandle);
         ^~~~~~~~~~
cc1plus: some warnings being treated as errors
config/SCRAM/GMake/Makefile.rules:2068: recipe for target 'tmp/slc6_amd64_gcc630/src/FWLJMET/LJMet/plugins/FWLJMETLJMetAuto/TestEventSelector.o' failed
gmake: *** [tmp/slc6_amd64_gcc630/src/FWLJMET/LJMet/plugins/FWLJMETLJMetAuto/TestEventSelector.o] Error 1
gmake: *** [There are compilation/build errors. Please see the detail log above.] Error 2
