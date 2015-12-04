
#import libraries needed
import sys
from pycparser import c_parser, c_ast, parse_file

#set up some global variables
returnCounter = 0
programLine = ""

#functions to let me use pycparser
class FuncCallVisitor(c_ast.NodeVisitor):
	def __init__(self, funcname):
		self.funcname = funcname

	def visit_FuncCall(self, node):
		if node.name.name == self.funcname:
			global returnCounter
			global programLine
			returnCounter += 1
			programLine = str(node.name.coord)
			programLine = programLine.rsplit(':', 1)[1]
			# print (programLine)
			# print('%s called at %s' % (self.funcname, node.name.coord))

#need to change the cpp_args in ast to match your directory (-I/your/path/to/utils/fake_libc_include)
def show_func_calls(filename, funcname):    
	ast = parse_file(filename, use_cpp=True, cpp_path='gcc', cpp_args=['-E', r'-I/Users/joshjoseph/Downloads/pycparser-master/utils/fake_libc_include'])
	v = FuncCallVisitor(funcname)
	v.visit(ast)

####################################################
# Exec family of function calls
# int execl(const char *path, const char *arg, ...
#                 /* (char  *) NULL */);
# int execlp(const char *file, const char *arg, ...
#                 /* (char  *) NULL */);
# int execle(const char *path, const char *arg, ...
#                 /*, (char *) NULL, char * const envp[] */);
# int execv(const char *path, char *const argv[]);
# int execvp(const char *file, char *const argv[]);
# int execvpe(const char *file, char *const argv[],
#                 char *const envp[]);
#
#
##########################################################

# calls to system() have high risk include the following: 
# When passing an unsanitized or improperly sanitized command string originating from a tainted source
# If a command is specified without a path name and the command processor path name resolution mechanism is accessible to an attacker
# If a relative path to an executable is specified and control over the current working directory is accessible to an attacker
# If the specified executable program can be spoofed by an attacker

#chekcing to see if there are errors in the c file
def errorCheck(filename):
	global returnCounter
	global programLine
	programErrorCounter = 0
	lookupListVulnCall = ['execve', 'execl', 'execlp', 'execle', 'execv', 'execvp', 'execvpe', 'system']
	errorDict = {}
	for element in lookupListVulnCall:
		elementErrorFound = True
		show_func_calls(filename, element)
		errorDict[element] = returnCounter
		if returnCounter == 0:
			elementErrorFound = False
		elif returnCounter == 1:
			f = open(filename, 'r')
			for lineNum, line in enumerate(f):
				if lineNum == int(programLine) - 1:
					print (lineNum, " ", line) # print the line where execve or system occurs
					varName = line.rsplit(element, 1)[1]
					varName = varName.rsplit("(", 1)[1]
					varName = varName.rsplit(")", 1)[0]
					print (varName)
					

		else:
			elementErrorFound = False
			
		returnCounter = 0
		if elementErrorFound:
			programErrorCounter += 1
		else:
			pass

	if programErrorCounter >= 1:
		print (filename,": This program calls a vulnerable function")

	else:
		print ("No errors found in file", filename)

#main will call errorCheck() for each file passed to it
if __name__ == "__main__":
	if len(sys.argv) > 1:
		#for every file, do something
		for i in range (1, len(sys.argv)):
			#this is what i do with each file
			errorCheck(sys.argv[i])
	else:
		print ("Please input C files to be examined")
