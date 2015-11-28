#!/bin/bash

# A small script to convert cflow
# output into png images

# @input-1: source .cflow file
# @output: png image

# Dependencies:
#	dot
#	cflow2dot


# Set target path for storing the PNG images
DEFAULT_PATH="./"
: ${OUTPUT_PNG_PATH="$DEFAULT_PATH"}

# Convert to PNG
SOURCE_FILE_PATH="$1"
OUTPUT_IMAGE_NAME=${SOURCE_FILE_PATH##*/} # last field in the path

cat $SOURCE_FILE_PATH | cflow2dot | dot > $SOURCE_FILE_PATH".dot"

dot -Tpng $SOURCE_FILE_PATH".dot" > "$OUTPUT_PNG_PATH/""$OUTPUT_IMAGE_NAME"".png"

if [ "$KEEP_DOT_FILES" == 0 ]; then
        rm $SOURCE_FILE_PATH".dot"
fi
