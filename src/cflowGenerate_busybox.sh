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

#  Get a list of c programs within the c project
LIST_OF_C_PROGRAMS=`find ./ -iname "*.c"

# Feed every c program to cflow
for program in $LIST_OF_C_PROGRAMS; do

	program_name=${program##*/}
	echo "[Cflow Generate]: Running cflow on $program ..."
	timeout 10 cflow --format=posix --omit-arguments --level-indent='0=\t' --level-indent='1=\t' --level-indent=start='\t' $program > "$CFLOW_OUTPUT_DIRECTORY/""$program_name"".cflow" 2>/dev/null
done

cd $main_dir
