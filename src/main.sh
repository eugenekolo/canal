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
# Environment Init and 		 #
# Input Argument Checks 	 #
############################
print_usage(){
	echo "[MAIN]: main.sh -d [PATH TO SOURCE DIRECTORY]"
	exit
}

if [[ "$#" -ne 2 ]]; then
	echo "a"
	print_usage
fi
if [ "$1" != "-d" ]; then
	echo "b"
	print_usage
fi
if [ ! -d "$2" ]; then
	echo "[MAIN]: Invalid Project Source Directory."
	exit
fi

source env_setup.sh 0;
source env_setup.sh $2; # Setting up the env variables

################
# Main Routine #
################
echo "[MAIN]: Runing cflow."
#./cflowGenerate.sh
./cflowGenerate_busybox

echo "[MAIN]: Plotting call graphs."
#./cflowVisualize.sh

echo "[MAIN]: Running security checkers."
./runSubmodules.sh

echo "[MAIN]: DONE!"

