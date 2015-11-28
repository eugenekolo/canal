#!/bin/bash

########################### INFO ################################
# A wrapper script to run cflow on a set of c project
# files and feed through submodules for vulnerability
# checks. Performs optional graph visualization.

# Configuration is specified through the env_setup.sh script
# Source env_setup.sh everytime one of the options is updated

#################################################################

# Input and Environment Check/Validation
error_checklist(){

	if [ ! -d "$CFLOW_INPUT_PROJECT_DIRECTORY" ]; then
		echo "[VulnAnalysis]: Invalid Project Source Directory"
		exit
	fi

	if [ ! -d "$CFLOW_OUTPUT_DIRECTORY" ]; then
                echo "[VulnAnalysis]: Invalid Cflow Output Directory"
                exit
        fi
}

############################
# Initializing Environment #
############################

DEFAULT_FLAGS="--format=posix --omit-arguments --level-indent='0=\t' --level-indent='1=\t' --level-indent=start='\t'"
: ${CFLOW_FLAGS="$DEFAULT_FLAGS"}

#########################
# Program Flow Analysis #
#########################

# Error checking, input validation
error_checklist

# Iterate through each C program
for project_path in $CFLOW_INPUT_PROJECT_DIRECTORY/*; do

	# Project name is the last field in the directory path
	PROJECT_NAME=${project_path##*/}

	echo "[VulnAnalysis]: Analyzing $PROJECT_NAME ... "

	#  Get a list of c programs within the c project
	LIST_OF_C_PROGRAMS=`find $project_path -iname "*.c"`
	#echo $LIST_OF_C_PROGRAMS

	# Run cflow on the C programs
	#echo $LIST_OF_C_PROGRAMS
	cflow --format=posix --omit-arguments --level-indent='0=\t' --level-indent='1=\t' --level-indent=start='\t' $LIST_OF_C_PROGRAMS > "$CFLOW_OUTPUT_DIRECTORY/""$PROJECT_NAME"".cflow"

	# Perform Optional Graph Visualization
	if [ $VISUALIZATION_ENABLED == 1 ]; then
		./cflow2png.sh "$CFLOW_OUTPUT_DIRECTORY/""$PROJECT_NAME"".cflow"
	fi

done

# Vulnerability checks

# TODO: first check which options are enabled
# to decide which submodules to run
# Feed output to a log file and store at destination

# Report results
# Check vulnerability logs and report statistics:
# Which programs have which vulnerability
