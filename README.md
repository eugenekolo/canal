#*.CANAL - C Vulnerability Static Analysis Tool

This tool set is a compilation of `python` and `bash` scripts intended to provide an automized bug checker for *.c files that use set*id() family functions.  The goal of this project is to eventually integrate into an IDE and aide users with warnings of potential security vulnerabilities in security critical projects.

Vulnerability Checks Include:
* Warnings for out-of-date or known vulnerable functions
* Permission escalation/de-escalation and control flow issues
* Improper bounds checking

Simple_Example.c:
```c
/*******************************************************************************
* Tests modules:
*	- a before b
*	- check set
*	- syschecker
*	- no return checker
*
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {

  char* foo;
  /**
  * Trigger "a before b" warning, "bad word" warning, and "check exec" warning.
  */
  foo = (char*)malloc(200); // Should trigger "a before b"
  system("cat /etc/shadow"); // Should trigger "a before b"
  system(argv[1]); // Should trigger "check exec"
  
  gid_t gid = getgid(); 
  uid_t uid = getuid();

  /**
  * Trigger "bad word" warning, and "return" warning.
  */
  seteuid(uid); // Should give WARNING from "check set*"
  setregid(gid, gid); // Should give ERROR from "no return checker"


  /**
  * Trigger "bad word" warning, and "check exec" warning.
  */ 
  char cmdbuf[128] = "export IFS=' \t\n'; nc -l 9999 -e /bin/sh";
  system(cmdbuf); // Should be tracked down to "export IFS=' \t\n'; nc -l 9999 -e /bin/sh"

  return 0;
}
```

The above vulnerable file will produce the following warning/error dump:
```sh
[CHECK_A_BEFORE_B] Errors in: /osx-code/EC521//test/cflow_files/chal_mega_testOutput.cflow
	error = malloc, line = 1, comment = [ERROR] malloc happens before seteuid on 4!
	error = malloc, line = 1, comment = [ERROR] malloc happens before setregid on 5!
	error = system, line = 2, comment = [ERROR] system happens before seteuid on 4!
	error = system, line = 2, comment = [ERROR] system happens before setregid on 5!
[CHECK_BAD_WORDS] Errors in: ./chal_mega_testOutput/chal_mega.c
	error = system, line = 21, comment = [ERROR] Dangerous exec* call!
	error = system, line = 22, comment = [ERROR] Dangerous exec* call!
	error = seteuid, line = 30, comment = [ERROR] Dangerous setuid operation. Be sure to know your OS!
	error = setregid, line = 31, comment = [ERROR] Dangerous setuid operation. Be sure to know your OS!
	error = system, line = 38, comment = [ERROR] Dangerous exec* call!
[CHECK_RETURN]./chal_mega_testOutput/chal_mega.c
	error = setgid, line = 30, comment = [ERROR] Not checking return!
	error = setgid, line = 31, comment = [ERROR] Not checking return!
``` 

#Documentation

Our full documentation is available at [this link.](docs/CANAL_Report.pdf)
You can also see compilation and run instructions in the comments of the script as well as in the individual subfolders.
[./src](src/) - Contains everything you need to start checking your security
[./docs](docs/) - Contains all analysis reports and project coordination documents
[./eval](eval/) - other utilities preforming similar functions to evaluate against our processing
[./test](test/) - test files and logs from our mass analysis of OS X, FreeBSD, and BusyBox utility source code; the /test/logs folder also contains post processing program code and results 


# Vulnerability Detection Modules
CANAL is composed of four main modules. Each of these modules reports potential vulnerabilities found in a C project.
### Detecting Vulnerable Function Calls
This module performs a simple linear search for a list of known dangerous functions. A major limitation of this approach is the sheer number of false positives due to assuming every usage of a “dangerous” function is inherently dangerous without regard to its actual danger. Some functions on the list include functions that do not check bounds and allow buffer overflows, gets, strcpy, strcat. Other groups of dangerous functions include functions that allow program execution (execve, system, etc.) , and functions that allow privilege escalation (setuid, setresuid, etc.). Additionally a limitation of this module is that it only performs a string search, so commented out code, and comments that contain the the function names are detected.
### Function Call Sequence Analyzer
This module checks for the dangerous function execution flows. We define a dangerous function execution flow as one where a function, “A”, occurs before another function, “B”. This flow can be seen in our motivating rsh vulnerability. Calling malloc before dropping privileges via setuid is a dangerous flow that is easily exploited. As such this module looks for occurrences of memory allocation functions before setuid functions. Additionally, this module looks for the usage of exec functions, before setuid functions. 

The flows of programs are analyzed by utilizing cflow in our pre-processing step as explained in 2.1.2. By traversing the graph that cflow generates, we are able to determine which functions occur before others. However, this is not perfect, and due to static code analysis limitations, we are not able to follows control statements. As such, code that essentially commented out with an “#ifdef” will be considered active, and all conditional control blocks are also considered active.

### Function Return Value Validation
This module is designed to check whether the return values of setuid functions are actually checked to verify that the setuid call successfully dropped privileges. The module performs several different checks in order to see if the return value is used for verification by the rest of the program. The first method is to see if the setuid function is called in a conditional statement. If so, the module verifies that the program performs return value check. The second method this module performs is to check if the return value of the setuid function is assigned to a variable and if that variable is used later in the program. Our module traces setuid function return values being set to a variable by traversing through the C file using string parsing and comparison. 
### Taint Checking for Exec Calls
Using unsanitized input data or program arguments in exec* or system calls introduces significant security risks due to potential injection and overflow attacks. This module is designed to address this security risk and checks whether system or exec functions are being called with arguments without proper bounds checking such as strcpy(), gets(). 

This module utilizes a number of string parsing functions to detect potential injection and buffer overflow vulnerabilities. Specifically, the module checks for exec* family functions or calls to command line arguments, which alias or concatenate files to which the user may have access (or be able to spoof via a symbolic link), and when functions that lack appropriate bounds checking ('gets', 'scanf', 'strcat', 'strcpy') are called as parameters. Then, the module searches the rest of the file for one level of indirection - namely variables (char * and char []) which were declared earlier in the file and initialized with these same functions and remain vulnerable to the aforementioned injection and overflow vulnerabilities. 

While this module have successfully identified the target vulnerabilities in our test programs, we point to two main weaknesses. In terms of false negatives, it will fail to detect multiple levels of indirection - if a vulnerable variable is aliased to a new variable (or chain of variables) and passed, it will not be detected, and it will not trigger if the variable is the result of an unsafe function without proper bounds checking declared outside of the file. In terms of false positives, it does not have a means to detect when functions that do not have proper bounds checking are sanitized, e.g. a custom function that bounds-checks the results of strcpy(). As has been our general strategy, we err on the side of false positives. A closer approximation to the goal of covering multiple variable (re)assignment may be possible in the future using a Backus-Naur-Form specification ala pycparser[18, 19] or another grammar-based approach without heading down the path of dynamic analysis.

<for more see CANAL_Report.pdf>

    
