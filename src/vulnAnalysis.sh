#!/bin/bash

########################### INFO ################################
# This is the main script to generate and visualize flow graphs
# and run each security module against a list of programs.

# HOW TO RUN:
# 1- Set your environment variables:
#	source env_setup.sh
# 2-Just run:
# 	./vulnAnalysis.sh
#################################################################

############################
# Initializing Environment #
############################

# We don't do anything by default
: ${CFLOW_GENERATION_ENABLED=0}
: ${VULNERABILITY_ANALYSIS_ENABLED=0}
: ${VISUALIZATION_ENABLED=0}

################
# Main Routine #
################
if [ $CFLOW_GENERATION_ENABLED == 1 ]; then
	echo "[VulnAnalysis]: Runing cflow."
	./cflowGenerate.sh
fi

if [ $VISUALIZATION_ENABLED == 1 ]; then
	echo "[VulnAnalysis]: Plotting call graphs."
	./cflowVisualize.sh
fi

if [ $VULNERABILITY_ANALYSIS_ENABLED == 1 ]; then
	echo "[VulnAnaysis]: Running security checkers."
	./runSubmodules.sh
fi

echo "[VulnAnalysis]: DONE!"
