#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{

  gid_t gidUser = getgid();
  setregid(gidUser, gidUser);
  system(argv[1]);

  return 0;
}