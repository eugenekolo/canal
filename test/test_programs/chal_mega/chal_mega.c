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
  foo = (char*)malloc(200); // Should trigger "a before b" later

  gid_t gid = getgid(); 
  uid_t uid = getuid();

  seteuid(uid); // Should give WARNING from "check set*"
  setregid(gid, gid); // Should give ERROR from "no return checker"
  system(argv[1]); // Should give WARNING from "syschecker"
                   // Should give ERROR from "a before b"

  char cmdbuf[128] = "export IFS=' \t\n'; /usr/bin/file ";
  system(cmdbuf); // Should be tracked down to "export IFS=' \t\n'; /usr/bin/file "

  return 0;
}