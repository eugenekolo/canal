#!/bin/bash

########################### INFO ################################
# A wrapper script to run cflow on a set of c project
# files and feed through submodules for vulnerability
# checks.

# Called by the vulnAnalysis.sh
#################################################################

# Input and Environment Check/Validation
error_checklist(){

	if [ ! -d "$CFLOW_INPUT_PROJECT_DIRECTORY" ]; then
		echo "[Cflow Generate]: Invalid Project Source Directory."
		exit
	fi

	if [ ! -d "$CFLOW_OUTPUT_DIRECTORY" ]; then
                echo "[Cflow Generate]: Invalid Cflow Output Directory."
                exit
        fi
}

#########################
# Program Flow Analysis #
#########################

# Error checking, input validation
error_checklist

####################
# MAIN STARTS HERE #
####################
rm  $CFLOW_OUTPUT_DIRECTORY/*

main_dir=`pwd`
cd $CFLOW_INPUT_PROJECT_DIRECTORY

# Iterate through each C project
for project_path in $CFLOW_INPUT_PROJECT_DIRECTORY/*; do

	if [ ! -d $project_path ]; then
		continue
	fi

	prev_dir=`pwd`
	cd $project_path

	# Project name is the last field in the directory path
	PROJECT_NAME=${project_path##*/}

	echo "[Cflow Generate]: Running cflow on $PROJECT_NAME ..."

	#  Get a list of c programs within the c project
	LIST_OF_C_PROGRAMS=`find ./ -iname "*.c"`

	# Run cflow on the C programs
	#echo $LIST_OF_C_PROGRAMS

	timeout 10 cflow --format=posix --omit-arguments --level-indent='0=\t' --level-indent='1=\t' --level-indent=start='\t' $LIST_OF_C_PROGRAMS > "$CFLOW_OUTPUT_DIRECTORY/""$PROJECT_NAME"".cflow" 2>/dev/null

	cd $prev_dir
done

cd $main_dir
