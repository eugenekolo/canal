#!/bin/bash

#########################  INFO ####################
# Each argument can be left unassigned.
# Default arguments are assigned in that case.

# Usage: source env_setup [INPUT]

# @Input: "0" to clear environment
#       : "1" to set enviroment variables as below
###################################################

if [ "$#" -ne 1 ]; then
        echo "[EnvSetup]: Usage: source env_setup.sh [0/1]"
        return
fi

if [ $1 == "0" ]; then
        echo "[EnvSetup]: Clearing environment."
        unset CFLOW_OUTPUT_DIRECTORY CFLOW_INPUT_PROJECT_DIRECTORY CFLOW_GENERATION_ENABLED \
                VISUALIZATION_ENABLED VULNERABILITY_ANALYSIS_ENABLED
	return
fi

#################
# CFLOW OPTIONS #
#################

export CFLOW_OUTPUT_DIRECTORY="$HOME/GoogleDrive/Courses/EC521/Project/Cflow_Results"
export CFLOW_INPUT_PROJECT_DIRECTORY="$HOME/GoogleDrive/Courses/EC521/Project/Test_Programs"

export CFLOW_GENERATION_ENABLED=1

##################################
# Vulnerability Analysis Options #
##################################

#export VULNERABILITY_ANALYSIS_ENABLED=1
#export VULNERABILITY_ANALYSIS_TYPE1_ENABLED=1 	# Checking occurance of A before B
#export VULNERABILITY_ANALYSIS_TYPE2_ENABLED=1	# Checking of function call returns
#export VULNERABILITY_ANALYSIS_TYPE3_ENABLED=1 	# Checking of exec* or system calls with known vulnerable binaries

###################
# Logging Options #
###################



###############################
# Graph Visualization Options #
###############################

# Enable flow visualization?
export VISUALIZATION_ENABLED=1

# Where to store .png images for call graphs
export OUTPUT_PNG_PATH="$HOME/GoogleDrive/Courses/EC521/Project/Cflow_Images"

# Keep (or remove) .dot (graph) files?
export KEEP_DOT_FILES=0
