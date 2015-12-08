#!/usr/bin/python3
################################################################################
# Finds badwords in a file, particularly in exec* family of function calls,
# and trys to determine what they are actually executing, to lower false positives from check_bad_words.py.
#
# calls to system() have high risk include the following: 
# When passing an unsanitized or improperly sanitized command string originating from a tainted source
# If a command is specified without a path name and the command processor path name resolution mechanism is accessible to an attacker
# If a relative path to an executable is specified and control over the current working directory is accessible to an attacker
# If the specified executable program can be spoofed by an attacker
#
################################################################################

import sys, re
#import os

badwords = ['execve', 'execl', 'execlp', 'execle', 'execv', 'execvp', 'execvpe', 'system']
no_bound_calls = ['gets', 'scanf', 'strcat', 'strcpy', 'cat', 'argv']


#chekcing to see if there are errors in the c file
def main(fileName):
    program_errors = [] # e.g. [{"error":"seteuid", "line":52, "comment":"You should..."}, 
                          #       {"error":"setregid", "line":19, "comment":""}, ...]

    print("[CHECK EXEC] Errors in:", fileName)
    #simple method - start by finding vulnerable calls (badwords)

    badlineNums = []
    badlineCalls = {}
    badlineLines = {}
    f = open(fileName, 'r')
    for lineNum, line in enumerate(f):
        for badword in badwords:
            if badword in line:
                if((badword == 'execv') and (('execv(' not in line) and ('execv (' not in line))): #avoid duplicating execv*    
                    continue
                else:
                    badlineNums.append(lineNum)  # keep track of the lines
                    badlineCalls[lineNum] = badword  # keep track of the calls by line number
                    badlineLines[lineNum] = line  # keep track of the whole lines by line number


    #find out what is called by the badword
    for item in badlineNums:
        targetline = badlineLines[item]
        targetword = badlineCalls[item]
        targetline = targetline.split(targetword, 1)[1] #trim to the badword function args
        if '(' in targetline:
            targetline = targetline.split('(', 1)[1] #trim the leading (
            parenthesisCount = 0
            for index, character in enumerate(targetline):
                if character == '(':
                    parenthesisCount += 1
                elif (character == ')'):
                    if (parenthesisCount > 0):
                        parenthesisCount -= 1
                    else:
                        targetline = targetline[:index]

            badArguments = targetline.split(", ")

            # print("Line", item, "calls", targetword, "with ", end = "")
            print ("\t","error = " + targetword + ", line = " + str(item) + ", comment = [ERROR] ", end=" ")
            # print(*badArguments, sep=', ')
            # ok, now we have the arguments for the badword function in targetline
            # simple vulnerability check - are we calling from argv
            if "argv" in targetline:
                print("argv called in vulnerable function!")
            if "cat " in targetline:
                print("cat called in vulnerable function")

            # hard part - finding user-vulnerable strings that are passed to the function
            # start by hunting for variables that are declared previously as char * or char ____ [___]
            for argument in badArguments:
                if(('(' in argument) or ("cat " in argument) or ("argv" in argument)) : # avoid function calls within function calls for now
                   continue
                else:
                    #print(argument)
                    q = open(fileName, 'r')
                    for lineNum, line in enumerate(q):
                        if((argument in line) and (lineNum < item) and (lineNum not in badlineLines)): # avoid calls past the argument, and avoid prior bad calls to system/execv in case of repeats
                            result = find_bounds_issues(line, argument)
                            if(result): 
                               print("\t\t", lineNum, line)


    # ## Report results!
    # print("[CHECK_EXEC] Errors in: " + fileName)
    # for error in program_errors:
    #     print("\terror = {0}, line = {1}, comment = {2}".format(error['error'], str(error['line']), error['comment']))

def find_bounds_issues(line, variable):
    frontHalf, backHalf = line.split(variable, 1)
    # look for target of no bound call issues
    for call in no_bound_calls:
        if((call in frontHalf) and (string_with_open_parenthesis(frontHalf))):
            print("error = Vulnerable input via", call)
            return call  # this is the front half check, e.g. scanf(badsource, vulnerable_variable)
        elif((call in backHalf) and (detect_variable_is_array(backHalf)) or (detect_variable_is_char_star(frontHalf)) and (assignment_before_call(backHalf, call))):
            print("error = Vulnerable input via", call)
            return call # this is the back half check for badness, e.g. = strcpy(badness, badness)
                # look for user-editable files and such
    if(('=' in backHalf) and ("/bin" in backHalf)):
        #get start and end of vulnerable input
        inputStart = backHalf.find("usr/bin")
        theEnding = backHalf.split("usr/bin",1)[1]
        inputEnd = theEnding.find("\"")
        print("error = Vulnerable input from user-accessable file in ", backHalf[inputStart:inputEnd])
        return 1 #anything will count as true here, will add more for handler function



    #fail
    return 0

def string_with_open_parenthesis(theString):
    parenthesisCount = 0
    for character in theString:
                if character == '(':
                    parenthesisCount += 1
                elif (character == ')'):
                    if (parenthesisCount > 0):
                        parenthesisCount -= 1
    if(parenthesisCount == 0):
        return 0
    else:
        return 1

def detect_variable_is_array(theString):
    theString = theString.strip() #nab the blanks
    re.sub("[^0-9]", "", theString) #nab the numbers between []
    if((theString[0] == '[') and (theString[1] == ']')):
        return 1
    else:
        return 0

def detect_variable_is_char_star(theString):
    theString = theString.strip() #nab the blanks
    if(';' in theString):
        theString = theString.rsplit(';',1)[1] #check and scrub for no whitespace between lines
    elif(',' in theString):
        theString = theString[:theString.find('*')+1] #check and scrub predecessors in multiple declarations

    if("char*" in theString):
        return 1
    else:
        return 0

def assignment_before_call(theString, call):
    theString = theString.split(call, 1)[0]
    if('=' in theString):
        return 1
    else:
        return 0




###############################################################################
# Main ~!
###############################################################################
if __name__ == "__main__":
    if len(sys.argv) > 1:
        for i in range (1, len(sys.argv)):
            main(sys.argv[i])
    else:
        print ("Please input C files to be examined")

