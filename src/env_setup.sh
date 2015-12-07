#!/bin/bash

#########################  INFO ####################
# You don't need to call this script directly.
# Please refer to main.sh

# Usage: source env_setup [INPUT]

# @Input: "0" to clear environment
#       : "[DIR]" 
###################################################

if [ "$#" -ne 1 ]; then
        echo "[EnvSetup]: Usage: source env_setup.sh [0/1]"
	echo "[EnvSetup]: [ 0: Clear Env, <DIR> path/to/c/projects] "
        return
fi

if [ $1 == "0" ]; then
        #echo "[EnvSetup]: Clearing environment."
        unset CFLOW_OUTPUT_DIRECTORY CFLOW_INPUT_PROJECT_DIRECTORY CFLOW_GENERATION_ENABLED \
		VISUALIZATION_ENABLED OUTPUT_PNG_PATH KEEP_DOT_FILES \
      		VULNERABILITY_ANALYSIS_ENABLED VULNERABILITY_ANALYSIS_TYPE1_ENABLED VULNERABILITY_ANALYSIS_TYPE2_ENABLED VULNERABILITY_ANALYSIS_TYPE3_ENABLED \
		PATH_TO_PYPARSER_FAKELIBC
	return
fi

####################
## PATH VARIABLES ##
####################

# YOU NEED TO CHANGE THE FOLLOWING PATH TO YOUR GIT DIRECTORY
#export PROJECT_PATH="$HOME/GoogleDrive/Courses/EC521/Project/EC521/"
export PROJECT_PATH="/osx-code/EC521/"

export CFLOW_INPUT_PROJECT_DIRECTORY="$1"
export CFLOW_OUTPUT_DIRECTORY="$PROJECT_PATH""/test/cflow_files"
export VULNERABILITY_LOGS_DIRECTORY="$PROJECT_PATH""/test/logs/"
export OUTPUT_PNG_PATH="$PROJECT_PATH""/test/cflow_images"

# Vulnerability options
#export VULNERABILITY_ANALYSIS_TYPE1_ENABLED=1 	# Checking occurance of A before B
#export VULNERABILITY_ANALYSIS_TYPE2_ENABLED=1		# Checking of function call returns
export VULNERABILITY_ANALYSIS_TYPE3_ENABLED=1 	# Checking of exec* or system calls with known vulnerable binaries
#export VULNERABILITY_ANALYSIS_TYPE4_ENABLED=1 	# Checking of bad words
