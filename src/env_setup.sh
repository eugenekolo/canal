#!/bin/bash

# Each argument can be left unassigned.
# Default arguments are assigned in that case.

#################
# CFLOW OPTIONS #
#################

export CFLOW_OUTPUT_DIRECTORY="$HOME/GoogleDrive/Courses/EC521/Project/Cflow_Results"
export CFLOW_INPUT_PROJECT_DIRECTORY="$HOME/GoogleDrive/Courses/EC521/Project/Test_Programs"

export CFLOW_FLAGS="--format=posix"" ""--omit-arguments"" ""--level-indent='0=\t'"" "" --level-indent='1=\t'"" ""--level-indent=start='\t'"

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
