#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  /* set up command buffer */
  char cmdbuf[128] = "export IFS=' \t\n'; /usr/bin/file ";
  char *input = cmdbuf + strlen(cmdbuf);
  int len = sizeof(cmdbuf) - (strlen(cmdbuf) + 1);

  gid_t egid = getegid();
  setregid(egid, egid);

  /* read input -- use safe function to prevent buffer overrun */
  fprintf(stdout, "Please enter a filename: ");
  fgets(input, len, stdin);

  /* sanitize input -- replace unsafe shell characters */
  for (; *input != '\0'; ++input) {
    switch (*input) {
    case '|': case '&':
    case '<': case '>':
    case '!': case '$':
    case ';':
      *input = ' ';
      break;
    }
  }
  
  /* execute command */
  system(cmdbuf);

  return 0;
}
