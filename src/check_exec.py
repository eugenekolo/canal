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

import sys
import os
from pycparser import c_parser, c_ast, parse_file

badwords = ['execve', 'execl', 'execlp', 'execle', 'execv', 'execvp', 'execvpe', 'system']

## Visits function calls in an AST
## Based off https://github.com/eliben/pycparser/examples/func_calls.py
class FuncCallVisitor(c_ast.NodeVisitor):
    def __init__(self, funcname):
        self.funcname = funcname
        self.badlineNums = []

    def visit_FuncCall(self, node):
        if node.name.name == self.funcname:
            self.badlineNums.append(int(str(node.name.coord).rsplit(':', 1)[1]))

## Generate an AST and then visit each node with our custom visitor
def visit_func_calls(fileName, funcname):    
    ast = parse_file(fileName, use_cpp=True, cpp_path='gcc', cpp_args=['-E', r'-I'+os.environ['PATH_TO_PYPARSER_FAKELIBC']])
    visitor = FuncCallVisitor(funcname)
    visitor.visit(ast) 
    return visitor.badlineNums


#chekcing to see if there are errors in the c file
def main(fileName):
    program_errors = [] # e.g. [{"error":"seteuid", "line":52, "comment":"You should..."}, 
                        #       {"error":"setregid", "line":19, "comment":""}, ...]

    for badword in badwords:
        badlineNums = visit_func_calls(fileName, badword)

        for badlineNum in badlineNums:
            is_error = True         
            if (is_error): 
                program_errors.append({'error': badword, 'line': badlineNum, 'comment': "[ERROR] Dangerous exec* call!"})

    ## Report results!
    print("[CHECK_EXEC] Errors in: " + fileName)
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
