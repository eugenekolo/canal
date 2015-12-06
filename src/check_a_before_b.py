#!/usr/bin/python3
################################################################################
# Takes in a cflow file. That must be generated.
# An error is raised if a function call to "a" happens before "b"
# e.g. if `system` happens before `seteuid`, or `setegid`
#      if `malloc` happens before `seteuid` ...
#
################################################################################

import sys
import os

badtuples = [("malloc", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("system", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("execve", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("execl", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("execlp", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("execle", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("execv", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("execvp", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
             ("execvpe", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']),
            ]

def main(fileName):
    program_errors = [] # e.g. [{"error":"seteuid", "line":52, "comment":"You should..."}, 
                        #       {"error":"setregid", "line":19, "comment":""}, ...]
    

    for badtuple in badtuples:
        f = open(fileName, 'r')
        for lineNum, line in enumerate(f):
            is_error = False

            ## "a" happened, check the rest of the file for "b"
            if badtuple[0] in line:
                print(badtuple[0])
                with open(fileName, 'r') as fo:
                    for i in range(lineNum + 1): # Skip ahead
                        fo.readline()
                    for bLineNum, bLine in enumerate(fo):
                        # Check if any "b" in the line
                        for b in badtuple[1]:
                            if b in bLine:
                                program_errors.append({'error': badtuple[0], 'line': lineNum, 
                                    'comment': "[ERROR] {0} happens before {1} on {2}!".format(badtuple[0], b, bLineNum)})
        f.close()

    ## Report results!
    print("[CHECK_A_BEFORE_B] Errors in: " + fileName)
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

