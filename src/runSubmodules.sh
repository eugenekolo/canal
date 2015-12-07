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
: ${VULNERABILITY_ANALYSIS_TYPE4_ENABLED=0} # bad words

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

##################
# RETURN WRAPPER #
##################
check_return_wrapper(){
	scripts_dir=`pwd`
	cd $CFLOW_INPUT_PROJECT_DIRECTORY

	# Get a copy of the submodule code
	cp $scripts_dir/check_return.py .

	#  Get a list of all the c programs
	LIST_OF_C_PROGRAMS=`find ./ -iname "*.c"`

	# Remove any previously existing log file
	rm $VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_RETURN_CHECKS.log 2>/dev/null

	# Feed every c program to vulnerability checker submodule
	for program in $LIST_OF_C_PROGRAMS; do
		echo "[Run Submodules]: Checking $program ..."
		python check_return.py $program 1>>$VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_RETURN_CHECKS.log 2>>$VULNERABILITY_LOGS_DIRECTORY/ERRORS.log
	done

	rm check_return.py
	cd $scripts_dir
}

######################
# A BEFORE B WRAPPER #
######################
check_a_before_b_wrapper(){

  # Remove any previously existing log file
	rm $VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_A_BEFORE_B.log 2>/dev/null

	# Iterate through each cflow file
	for cflow_output in $CFLOW_OUTPUT_DIRECTORY/*; do
		echo "[Run Submodules]: Checking $program ..."
		python check_a_before_b.py $cflow_output 1>>$VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_A_BEFORE_B.log 2>>$VULNERABILITY_LOGS_DIRECTORY/ERRORS.log
	done
}

################
# EXEC WRAPPER #
################
check_exec_wrapper(){
	scripts_dir=`pwd`
  cd $CFLOW_INPUT_PROJECT_DIRECTORY

  # Get a copy of the submodule code
  cp $scripts_dir/check_exec.py .

  #  Get a list of all the c programs
  LIST_OF_C_PROGRAMS=`find ./ -iname "*.c"`
  #echo $LIST_OF_C_PROGRAMS

  # Remove any previously existing log file
	rm $VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_EXEC_CALLS.log 2>/dev/null

  # Feed every c program to vulnerability checker submodule
  for program in $LIST_OF_C_PROGRAMS; do
	echo "[Run Submodules]: Checking $program ..."
	python check_exec.py $program 1>>$VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_EXEC_CALLS.log 2>>$VULNERABILITY_LOGS_DIRECTORY/ERRORS.log
  done

  rm check_exec.py
  cd $scripts_dir
}

#####################
# BAD WORDS WRAPPER #
#####################
check_bad_words_wrapper(){
  scripts_dir=`pwd`
  cd $CFLOW_INPUT_PROJECT_DIRECTORY

  # Get a copy of the submodule code
  cp $scripts_dir/check_bad_words.py .

  #  Get a list of all the c programs
  LIST_OF_C_PROGRAMS=`find ./ -iname "*.c"`

  # Remove any previously existing log file
	rm $VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_BAD_WORDS.log 2>/dev/null

  # Feed every c program to vulnerability checker submodule
  for program in $LIST_OF_C_PROGRAMS; do
	echo "[Run Submodules]: Checking $program ..."
     	python check_bad_words.py $program 1>>$VULNERABILITY_LOGS_DIRECTORY/VULNERABILITIES_BAD_WORDS.log 2>>$VULNERABILITY_LOGS_DIRECTORY/ERRORS.log
  done

  rm check_bad_words.py
  cd $scripts_dir
}

####################
# MAIN STARTS HERE #
####################

rm $VULNERABILITY_LOGS_DIRECTORY/ERRORS.log 2>/dev/null

if [ $VULNERABILITY_ANALYSIS_TYPE1_ENABLED == 1 ]; then
	echo "[Run Submodules]: Checking A before B vulnerability."
	check_a_before_b_wrapper
fi

if [ $VULNERABILITY_ANALYSIS_TYPE2_ENABLED == 1 ]; then
	echo "[Run Submodules]: Checking function returns."
	check_return_wrapper
fi

if [ $VULNERABILITY_ANALYSIS_TYPE3_ENABLED == 1 ]; then
	echo "[Run Submodules]: Checking exec calls."
 	check_exec_wrapper
fi

if [ $VULNERABILITY_ANALYSIS_TYPE4_ENABLED == 1 ]; then
	echo "[Run Submodules]: Checking for bad words."
 	check_bad_words_wrapper
fi


