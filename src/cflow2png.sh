#!/bin/bash

# A small script to convert cflow
# output into png images

# @Input	: a path to source .cflow file
# @Output	: a flow diagram as a png image

# Dependencies:
#	dot
#	cflow2dot


####################
# Helper Functions #
####################
# Input and Environment Check/Validation
error_checklist(){

	if [ ! -f "$SOURCE_FILE_PATH" ]; then
		echo "[cflow2png]: Invalid path to source cflow file."
		echo "[cflow2png]: Usage: ./cflow2png [PATH TO '.cflow; FILE]"
		exit
	fi
}

####################
# MAIN STARTS HERE #
####################

# Set target path for storing the PNG images
DEFAULT_PATH="./"
: ${OUTPUT_PNG_PATH="$DEFAULT_PATH"}

SOURCE_FILE_PATH="$1"

# Error checks
error_checklist

# Plot flow diagram and save as a PNG image
OUTPUT_IMAGE_NAME=${SOURCE_FILE_PATH##*/} # last field in the path
cat $SOURCE_FILE_PATH | cflow2dot | dot > $SOURCE_FILE_PATH".dot"

dot -Tpng $SOURCE_FILE_PATH".dot" > "$OUTPUT_PNG_PATH/""$OUTPUT_IMAGE_NAME"".png"

if [ "$KEEP_DOT_FILES" == 0 ]; then
        rm $SOURCE_FILE_PATH".dot"
fi
