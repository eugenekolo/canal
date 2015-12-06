#!/usr/bin/python3
################################################################################
# Finds badwords in a file, particularly in exec*, and no bounds checking functions.
#
################################################################################

import sys
import os

exec_calls = ['execve', 'execl', 'execlp', 'execle', 'execv', 'execvp', 'execvpe', 'system']
no_bound_calls = ['gets', 'scanf', 'strcat', 'strcpy']
setuid_calls = ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']

badwords = exec_calls + no_bound_calls + setuid_calls

#chekcing to see if there are errors in the c file
def main(fileName):
    program_errors = [] # e.g. [{"error":"seteuid", "line":52, "comment":"You should..."}, 
                          #       {"error":"setregid", "line":19, "comment":""}, ...]

    f = open(fileName, 'r')
    for lineNum, line in enumerate(f):
        for badword in badwords:
            if badword in line:
                comment = ""
                if badword in exec_calls:
                    comment = '[ERROR] Dangerous exec* call!'
                elif badword in no_bound_calls:
                    comment = '[ERROR] Dangerous boundless call used!'
                elif badword in setuid_calls:
                    comment = '[ERROR] Dangerous setuid operation. Be sure to know your OS!'

                program_errors.append({'error': badword, 'line': lineNum, 'comment': comment})

    ## Report results!
    print("[CHECK_BAD_WORDS] Errors in: " + fileName)
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