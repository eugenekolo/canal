#! /usr/bin/python3

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
	ast = parse_file(filename, use_cpp=True, cpp_path='gcc', cpp_args=['-E', r'-I/home/sahin/GoogleDrive/Courses/EC521/Project/pycparser-master/utils/fake_libc_include'])
	v = FuncCallVisitor(funcname)
	v.visit(ast)


#chekcing to see if there are errors in the c file
def errorCheck(filename):
	global returnCounter
	global programLine
	programErrorCounter = 0
	lookupList = ['setuid', 'seteuid', 'setegid', 'setfsuid', 'setreuid', 'setregid', 'setresuid', 'setresgid', 'setgid']
	errorDict = {}
	for element in lookupList:
		elementErrorFound = True
		show_func_calls(filename, element)
		errorDict[element] = returnCounter
		if returnCounter == 0:
			elementErrorFound = False
		elif returnCounter == 1:
			f = open(filename, 'r')
			for lineNum, line in enumerate(f):
				if lineNum == int(programLine) - 1:
					#check if its used in a conditional statement. if it is, there is no error
					if "if" in line:
						elementErrorFound = False
					#check to see if its being assigned to a variable
					elif "=" in line:
						varName = line.rsplit("=", 1)[0]
						if "int" in varName:
							varName = varName.rsplit("int", 1)[1]
							while "," in varName:
								varName = varName.rsplit(",", 1)[1]
						varName = varName.strip()
						#now i have the variable name, I am going to traverse the document to see if it is ever being used again
						#if it is used again, there is no error. otherwise, there is
						with open(filename, "r") as fo:
							for i in range(lineNum + 1):
								fo.readline()
							for line in fo:
								if varName in line:
									elementErrorFound = False
					#only used once, so ther must be an error
					else:
						pass
		else:
			elementErrorFound = False
			
		returnCounter = 0
		if elementErrorFound:
			programErrorCounter += 1
		else:
			pass

	#this is where i want function to return with either errors or not
	#will be edited to fit whole program
	if programErrorCounter >= 1:
		print (filename,": This file is not checking the return of a set*id() family function")
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
