#!/usr/bin/python3
################################################################################
# Finds badwords in a file and checks if their return value is checked/used 
#
################################################################################

import sys
import os

## Globals
badwords = ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']

##
# Finds badwords in a file and checks if their return value is checked/used
#
def main(fileName):
    program_errors = [] # e.g. [{"error":"seteuid", "line":52, "comment":"You should..."}, 
                        #       {"error":"setregid", "line":19, "comment":""}, ...]

    ## Get a list of all the possible bad lines
    badlineNums = []
    f = open(fileName, 'r')
    for lineNum, line in enumerate(f):
        for badword in badwords:
            if badword in line:
                badlineNums.append(lineNum)

    ## Check each assumed bad line, and see if it's actually a return error
    for badlineNum in badlineNums:
        is_error = True

        ## Check each line in the file for a bad line
        f = open(fileName, 'r')
        for lineNum, line in enumerate(f):
            if (lineNum != badlineNum - 1): # No point checking if it's not a badline
                continue
    
            if "if" in line: # No error here boys
                is_error = False
                break
    
            ## Check the variable is assigned and used again later in the file
            # Extract the variable name
            varName = ""
            if "=" in line:
                varName = line.rsplit("=", 1)[0]
                if "int" in varName:
                    varName = varName.rsplit("int", 1)[1]
                    while "," in varName:
                        varName = varName.rsplit(",", 1)[1]
                varName = varName.strip()
    
                with open(fileName, "r") as fo:
                    for i in range(lineNum + 1): # Skip ahead
                        fo.readline()
                    for line in fo:
                        if varName in line:
                            is_error = False
                            continue

        if (is_error): 
            program_errors.append({'error': badword, 'line': badlineNum, 'comment': "[ERROR] Not checking return!"})

    ## Report results!
    print("[CHECK_RETURN]" + fileName)
    for error in program_errors:
        print("\terror = {0}, line = {1}, comment = {2}".format(error['error'], str(error['line']), error['comment']))


###############################################################################
# Main ~!
###############################################################################
if __name__ == "__main__":
    if len(sys.argv) > 1:
        for i in range (1, len(sys.argv)):
            main(sys.argv[i])
    else:
        print ("Please input C files to be examined")
