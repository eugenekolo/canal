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