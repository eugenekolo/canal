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

#Main Static Analysis Modules:

* `check_a_before_b` : this module uses cflow to take in a parent directory and recursively
* `check_return` : this module uses
* `check_badwords` : 
* `check_sys` :  

#Documentation

Our full documentation is available at [this link.](docs/KIERK_WRITE_UP.odt)

##Integrate Into README.md:

TODO(eugenek): Write more here

./eval - Testing of third party applications and seeing if they service our goal
./test - Test files for our solution
./src
    
