#!/bin/bash

########################### INFO ################################
# A small script to read through the generated .cflow files
# and plot flow diagrams

# Called by the vulnAnalysis.sh
#################################################################

# Input and Environment Check/Validation
error_checklist(){

	if [ ! -d "$OUTPUT_PNG_PATH" ]; then
		echo "[Cflow Visualize]: Invalid target directory."
		exit
	fi

	if [ ! -d "$CFLOW_OUTPUT_DIRECTORY" ]; then
                echo "[Cflow Visualize]: Invalid cflow directory."
                exit
        fi
}

#########################
# Program Flow Analysis #
#########################

# Error checking, input validation
error_checklist

# Iterate through each cflow file
for file_path in $CFLOW_OUTPUT_DIRECTORY/*; do

	file_name=${file_path##*/}

	echo "[Cflow Visualize]: Plotting $file_name ..."

	./cflow2png.sh $file_path
done
