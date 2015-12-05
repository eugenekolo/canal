#!/usr/bin/python3
################################################################################
# An error is raised if a function call to "a" happens before "b"
# e.g. if `seteuid` happens before `system`, or `execve`
#      if `malloc` happens before `seteuid`
#
################################################################################

import sys
import os

badtuples = [("seteuid", ['execve', 'execl', 'execlp', 'execle', 'execv', 'execvp', 'execvpe', 'system']),
          ("malloc", ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid'])
         ]

def main(fileName):
    program_errors = [] # e.g. [{"error":"seteuid", "line":52, "comment":"You should..."}, 
                        #       {"error":"setregid", "line":19, "comment":""}, ...]
    
    for badtuple in badtuples:
        pass

    ## Report results!
    print("== Errors in: " + fileName)
    for error in program_errors:
        print("error = {0}, line = {1}, comment = {2}".format(error['error'], str(error['line']), error['comment']))

###############################################################################
# Main ~!
###############################################################################
if __name__ == "__main__":
    if len(sys.argv) > 1:
        for i in range (1, len(sys.argv)):
            main(sys.argv[i])
    else:
        print ("Please input C files to be examined")

