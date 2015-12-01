#!/bin/bash

#################### INFO #########################
# This is a wrapper script to run each
# vulnerability check submodule against
# target set of programs.

# HOW TO RUN:
# This script is called by the main vulnAnalysis.sh:
#	source env_setup.sh 1
#	chmod 744 runSubmodules.sh; ./runSubmodules
####################################################

# We do not run any submodules by default
: ${VULNERABILITY_ANALYSIS_TYPE1_ENABLED=0} # A before B
: ${VULNERABILITY_ANALYSIS_TYPE2_ENABLED=0} # return check
: ${VULNERABILITY_ANALYSIS_TYPE3_ENABLED=0} # execve checker

# Input and Environment Check/Validation
error_checklist(){

        if [ ! -d "$CFLOW_INPUT_PROJECT_DIRECTORY" ]; then
                echo "[Run Submodules]: Invalid Project Source Directory."
                exit
        fi

        if [ ! -d "$VULNERABILITY_LOGS_DIRECTORY" ]; then
                echo "[Run Submodules]: Invalid directory to store vulnerability logs."
                exit
        fi
}

return_check_wrapper(){
	scripts_dir=`pwd`
	cd $CFLOW_INPUT_PROJECT_DIRECTORY

	# Get a copy of the submodule code
	cp $scripts_dir/returnCheckerSubmodule.py .

	#  Get a list of all the c programs
	LIST_OF_C_PROGRAMS=`find ./ -iname "*.c"`
	#echo $LIST_OF_C_PROGRAMS

	# Remove any previously existing log file
	rm $VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_RETURN_CHECKS.log 2>/dev/null

	# Feed every c program to vulnerability checker submodule
	for program in $LIST_OF_C_PROGRAMS; do
		python returnCheckerSubmodule.py $program 1>>$VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_RETURN_CHECKS.log 2>>$VULNERABILITY_LOGS_DIRECTORY/ERRORS.log
	done

	rm returnCheckerSubmodule.py
	cd $scripts_dir
}

a_before_b_wrapper(){
	echo "TODO-1: Call A before B submodule with every .cflow file"
	echo "Discuss with Eugene about how to handle/log output"
}

execve_wrapper(){

	echo "TODO-3: Call execve checker submodule with every c file"
	echo "Discuss with Josh about how to handle/log output"
}

# TODO: Call each wrapper depending on the options

if [ $VULNERABILITY_ANALYSIS_TYPE2_ENABLED == 1 ]; then
	return_check_wrapper
fi
