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
for SOURCE_FILE_PATH in $CFLOW_OUTPUT_DIRECTORY/*; do

	#file_name=${file_path##*/}
	#echo "[Cflow Visualize]: Plotting $file_name ..."

	# Plot flow diagram and save as a PNG image
	OUTPUT_IMAGE_NAME=${SOURCE_FILE_PATH##*/} # last field in the path
	cat $SOURCE_FILE_PATH | cflow2dot | dot > $SOURCE_FILE_PATH".dot"

	dot -Tpng $SOURCE_FILE_PATH".dot" > "$OUTPUT_PNG_PATH/""$OUTPUT_IMAGE_NAME"".png"

	if [ "$KEEP_DOT_FILES" == 0 ]; then
		rm $SOURCE_FILE_PATH".dot"
	fi

done

