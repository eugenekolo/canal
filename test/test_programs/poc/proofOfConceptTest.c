/*-
 * Copyright (c) 1983, 1990, 1993, 1994, 2002
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
 /* Multinode modifications, 
 David Mathog, mathog@caltech.edu, 31-Dec-2002.
 To be applied to inetutils 1.4.2
 
 See VERSTRING below for version number
 
 
 node1,node2,node3,..nodeN 
    run the same command sequentially on each of N nodes.
    Output from each command is emitted as in the single
    node case.  Data piped into rsh in this mode will
    probably not go to the intended node.  Input data
    is NOT replicated and sent to each node.
    DO NOT do this:
       cat file | rsh node1,node2,node3 cat - \>/tmp/outfile
    The input stream will all go to the first node and nothing will be left
    for the other nodes!
    
    user1@node1,user2@node2 will not work either,  Best to leave
    [user@] off and use -l instead.
 -z 
    eliminates all IO - rsh only runs the command and exits
    Should allow rsh to start (but not monitor) jobs on
    an unlimited number of remote machines without rcmd()
    using up all ports below 1024.
 -f 
    Changes meaning of "host" to be a file containing a list of hostnames,
    One hostname per line.  
    Lines starting with # are comments and are ignored.
    A line with a hostname may not have any other characters on it.
 
 -q --queue     message queue key (decimal, not hex)
 -t --type     message type 
    If these are provided rsh reads the next matching message
    from the message queue and sends it to the remote machine
    as if it came from stdin.  stdin is ignored. Return data,
    if any, goes out through stdout.  This  provides a
    trivial mechanism for funneling data (in small chunks)
    through a single rsh connection.
    
 Example usage:
 
   % rsh -zf allmynodes.txt poweroff      #turn off all nodes in a cluster
   % rsh -zf allmynodes.txt /etc/rc.d/init.d/daemon restart  #restart daemon on all nodes
   % rsh node1,node2,node3 sensors | grep 'CPU0 Temp' #measure CPU0 temp on 3 nodes

 Example compilation:
   Obtain inetutils 1.4.07, rename rsh.c there to rsh.c.dist, place
   this rsh.c in the rsh directory.  Make from top.
   
 HISTORY:
 
 1.4  17-JUN-2005. David Schwartz (email redacted by google) answered
      a post in comp.unix.programmers stating that the 1.3 fix was probably
      dangerous.  So I took it out and put a seteuid(0) in at the
      bottom of the loop, which worked just as well.

 1.3  17-JUN-2005. For some reason on nonpriv'd users could not do
      multiple connections, it works to one target but blows up with an
      rcmd error on the second.  This turned out to be the seteuid
      and setuid which are commented out just above the
      ifdef HAVE_SIGACTION.  It seems to work ok without it
      
 1.2  31-DEC-2002. changed msgbuf definition to rsh_msgbuf to allow a
      compile on Solaris.  Additional changes in inetutils package
      which are required to build rsh on Solaris are:
      1) drop linux error.h into top level of inetutils
      2) in setenv.c change to:
      #ifndef	HAVE_GNU_LD
      # define __environ      environ
      extern char **environ;
      #endif
      3) make inetutils. It will bomb trying to make telnetd because "error" won't
      be found anywhere.  But enough will have been made to build rsh.
      Drop into rsh and do "make".

      
 1.1  20-DEC-2002.  Added -f option. 
 1.0  Initial release.

 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif
#include <sys/file.h>
#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <netinet/in.h>
#include <netdb.h>

#include <err.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
# include <stdarg.h>
#else
# include <varargs.h>
#endif
#include <getopt.h>
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

/*needed for message queue */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAXHOSTNAMELEN 256
#define VERSTRING  "1.4.07_caltechbio_1.4, 17-JUN-2005"
char hostbuffer[MAXHOSTNAMELEN];
int  key=0;    /* an invalid queue key*/
int  mtype=0;  /* message type == get next */
struct rsh_msgbuf {
     long mtype;             /* message type, must be > 0 */
     char mtext[BUFSIZ];     /* message data */
};
struct rsh_msgbuf thismsgbuf;


#ifdef KERBEROS
# include <kerberosIV/des.h>
# include <kerberosIV/krb.h>

CREDENTIALS cred;
Key_schedule schedule;
int use_kerberos = 1, doencrypt;
char dst_realm_buf[REALM_SZ], *dest_realm;
extern char *krb_realmofhost();
#endif


/*
 * rsh - remote shell
 */
int	rfd2;

char *copyargs __P ((char **));
void sendsig   __P ((int));
void talk      __P ((int, sigset_t *, pid_t, int));
void usage     __P ((void));
void warning   __P ((const char *, ...));
static char      *readnexthost(FILE *hostfile);
void              getmessagefromq(char *buf, int *cc, int key, int mtype);

/* basename (argv[0]).  NetBSD, linux, & gnu libc all define it.  */
extern  char *__progname;

#ifdef KERBEROS
#ifdef ENCRYPTION
#define	OPTIONS	"8Kdek:l:nmxVhfzq:t:"
#else
#define	OPTIONS	"8Kdek:l:nmVhfzq:t:"
#endif
#else
#define	OPTIONS	"8KLdel:nmVhfzq:t:"
#endif
static const char *short_options = OPTIONS;
static struct option long_options[] =
{
  { "debug", no_argument, 0, 'd' },
  { "user", required_argument, 0, 'l' },
  { "escape", required_argument, 0, 'e' },
  { "queue", required_argument, 0, 'q' },
  { "type", required_argument, 0, 't' },
  { "hostfile", no_argument, 0, 'f' },
  { "zero", no_argument, 0, 'z' },
  { "8-bit", no_argument, 0, '8' },
  { "kerberos", no_argument, 0, 'K' },
  { "no-input", no_argument, 0, 'n' },
  { "maskrest", no_argument, 0, 'm' },
#ifdef KERBEROS
  { "realm", required_argument, 0, 'k' },
  { "encrypt", no_argument, 0, 'x' },
#endif
  { "help", no_argument, 0, 'h' },
  { "version", no_argument, 0, 'V' },
  { 0, 0, 0, 0 }
};

static void
pusage (FILE *stream)
{
  fprintf (stream,
	  "Usage: %s [-nd%s]%s[-l USER] [USER@]HOST[1,HOST2,...,HOSTN] [COMMAND [ARG...]]\n",
	   __progname,
#ifdef KERBEROS
#ifdef ENCRYPTION
	    "x", " [-k REALM] "
#else
	    "", " [-k REALM] "
#endif
#else
	    "", " "
#endif
	   );
}

/* Print a help message describing all options to STDOUT and exit with a
   status of 0.  */
static void
help (void)
{
  pusage (stdout);
  puts ("Execute COMMAND on remote system HOST");
  puts ("When use as rlogin:");
  puts ("\
  -8, --8-bit       allows an eight-bit input data path at all times");
  puts ("\
  -E, --no-escape   stops any character from being recognized as an escape\n\
                    character");
  puts ("\
  -d, --debug       turns on socket debugging (see setsockopt(2))");
  puts ("\
  -e, --escape=CHAR allows user specification of the escape character,\n\
                    which is ``~'' by default");
  puts ("\
  -l, --user USER   run as USER on the remote system");
#ifdef KERBEROS
  puts ("\
  -K, --kerberos    turns off all Kerberos authentication");
  puts ("\
  -k, --realm REALM Obtain tickets for the remote host in REALM\n\
                    instead of the remote host's realm");
# ifdef ENCRYPTION
  puts ("\
  -x, --encrypt     encrypt all data using DES");
# endif
#endif
  puts ("\
  -n, --no-input    use /dev/null as input");
  puts ("\
  -m, --maskrest    host and command files only follow (hides -options in commands)");
  puts ("\
  -f  	            host is a file of hostnames, one per line");
  puts ("\
  -z  	            run the remote command but ignore all IO");
  puts ("\
  -q, --queue KEY   read data from message queue specified by KEY instead of stdin");
  puts ("\
  -t, --type  TYPE  match messages: TYPE>0, exactly; TYPE=0, any; TYPE<0, any but |TYPE|");
  puts ("\
      --help        give this help list");
  puts ("  -V, --version     print program version\n");
  fprintf (stdout, "Limits:   Maximum bytes per message from queue: %d\n",BUFSIZ);
  puts ("	  If no command is specified an rlogin is performed instead");
  puts ("	  Commands are run sequentially on listed hosts.");
  puts ("	  Sending data from stdin or a message queue to multiple hosts is not supported");
  puts ("	  Running with no command to multiple hosts is not supported\n");
  fprintf (stdout, "Version: %s, derived from inetutils ",VERSTRING);
  fprintf (stdout, "\nSubmit bug reports to %s.\n", PACKAGE_BUGREPORT);
  exit (0);
}

/* Print a message saying to use --help to STDERR, and exit with a status of
   1.  */
static void
try_help (void)
{
  fprintf (stderr, "Try `%s --help' for more information.\n", __progname);
  exit (1);
}

void
usage()
{
  pusage (stderr);
  try_help ();
}


int
main (int argc, char **argv)
{
  struct passwd *pw;
  struct servent *sp;
  sigset_t sigs, osigs;
  int asrsh, ch, dflag, nflag, rem, shutstat;
  pid_t pid = 0;
  uid_t uid;
  char *args, *host, *user;
  int zflag; /* if set, no io to/from remote machine, just start job and leave */
  int fflag; /* if set, the host named refers to a file of host names, one per line, #comments */
  FILE *hostfile; /* file pointer for host file */
  char *hostlist;
  int maskrest;

#ifndef HAVE___PROGNAME
  extern char *__progname;
  __progname = argv[0];
#endif

  asrsh = dflag = nflag = 0;
  host = user = NULL;
  zflag = 0;
  fflag = 0;
  hostfile=NULL;
  maskrest = 0;

  /* If called as something other than "rsh", use it as the host name */
  {
    char *p = strrchr(argv[0], '/');
    if (p)
      ++p;
    else
      p = argv[0];
    if (strcmp (p, "rsh"))
      host = p;
    else
      asrsh = 1;
  }

  while ((ch = getopt_long (argc, argv, short_options, long_options, 0))
	 != EOF)
    {
      switch (ch)
	{
	case 'L':	/* -8Lew are ignored to allow rlogin aliases */
	case 'e':
	case 'w':
	case '8':
	  break;

	case 'd':
	  dflag = 1;
	  break;

	case 'l':
	  user = optarg;
	  break;

	case 'z':
	  zflag = 1;
	  break;
		
	case 'f':
	  fflag = 1;
	  break;
		
	case 'm':
	  maskrest = 1;
	  break;
		
	case 't':
      	  if(sscanf(optarg,"%d",&mtype) != 1){
      	    (void) fprintf(stderr,"missing or invalid message type value\n");
      	    exit(EXIT_FAILURE);
      	  }
	  break;
	  
	case 'q':
      	  if(sscanf(optarg,"%d",&key) != 1){
      	    (void) fprintf(stderr,"missing or invalid message queue key value\n");
      	    exit(EXIT_FAILURE);
      	  }
	  if(key<=0){
	     fprintf(stderr, "rsh: message queue keys must be greater than zero.\n");
	     exit(EXIT_FAILURE);
	  }
	  break;
	  
	case 'K':
#ifdef KERBEROS
	  use_kerberos = 0;
#endif
	  break;

#ifdef KERBEROS
	case 'k':
	  strncpy (dest_realm_buf, optarg, sizeof dest_realm_buf);
	  dest_realm_buf [REALM_SZ - 1] = '\0';
	  dest_realm = dst_realm_buf;
	  break;

# ifdef ENCRYPTION
	case 'x':
	  doencrypt = 1;
	  des_set_key (cred.session, schedule);
	  break;
# endif
#endif

	case 'n':
	  nflag = 1;
	  break;

	case 'h':
	  help ();

	case 'V':
	  printf ("rsh (%s) %s\n", PACKAGE_NAME, PACKAGE_VERSION);
	  exit (0);

	case '?':
	  try_help ();

	default:
	  usage();
	}
        if(maskrest)break; /* no more command line qualifiers other than host and command */
    }

  if (optind < argc)
    host = argv[optind++];

  /* To few args.  */
  if (!host)
    usage ();

  /* If no further arguments, must have been called as rlogin. */
  if (!argv[optind])
    {
      if (asrsh)
	*argv = (char *)"rlogin";
      seteuid (getuid ());
      setuid (getuid ());
      execv (PATH_RLOGIN, argv);
      errx (1, "can't exec %s", PATH_RLOGIN);
    }

  argc -= optind;
  argv += optind;

  /* We must be setuid root.  */
  if (geteuid ())
    errx (1, "must be setuid root.\n");

  if (!(pw = getpwuid (uid = getuid ())))
    errx(1, "unknown user id");

  /* Accept user1@host format, though "-l user2" overrides user1 */
  {
    char *p = strchr (host, '@');
    if (p)
      {
	*p = '\0';
	if (!user && p > host)
	  user = host;
	host = p + 1;
	if (*host == '\0')
	  usage ();
      }
    if (!user)
      user = pw->pw_name;
  }

#ifdef KERBEROS
#ifdef ENCRYPTION
  /* -x turns off -n */
  if (doencrypt)
    nflag = 0;
#endif
#endif

  args = copyargs (argv);

  sp = NULL;
#ifdef KERBEROS
  if (use_kerberos)
    {
      sp = getservbyname ((doencrypt ? "ekshell" : "kshell"), "tcp");
      if (sp == NULL)
	{
	  use_kerberos = 0;
	  warning ("can't get entry for %s/tcp service",
		   doencrypt ? "ekshell" : "kshell");
	}
    }
#endif
  if (sp == NULL)
    sp = getservbyname("shell", "tcp");
  if (sp == NULL)
    errx (1, "shell/tcp: unknown service");

  /*  Implement comma or space separated host list */
  if(!fflag){
    hostlist = host;
    host=strtok(hostlist,", ");
  }
  else {
    hostfile=fopen(host,"r"); /* the host field specifies a file of host names, one per line */
    if(hostfile==NULL){
       fprintf(stderr, "rsh: could not read hostfile: %s\n",host);
       exit(EXIT_FAILURE);
    }
    host=readnexthost(hostfile);
    if(host==NULL){
      fprintf(stderr, "rsh: hostfile %s contains no host names.\n",host);
      exit(EXIT_FAILURE);
    }
  }

  while(1){  /* loop on host list */
  rem=0;
  rfd2=0;
#ifdef KERBEROS
 try_connect:
  if (use_kerberos)
    {
      struct hostent *hp;

      /* fully qualify hostname (needed for krb_realmofhost) */
      hp = gethostbyname(host);
      if (hp != NULL && !(host = strdup (hp->h_name)))
	err (1, NULL);

      rem = KSUCCESS;
      errno = 0;
      if (dest_realm == NULL)
	dest_realm = krb_realmofhost (host);

#ifdef ENCRYPTION
      if (doencrypt)
	rem = krcmd_mutual (&host, sp->s_port, user, args,
			    &rfd2, dest_realm, &cred, schedule);
      else
#endif
	rem = krcmd (&host, sp->s_port, user, args, &rfd2,
		     dest_realm);
      if (rem < 0)
	{
	  use_kerberos = 0;
	  sp = getservbyname ("shell", "tcp");
	  if (sp == NULL)
	    errx (1, "shell/tcp: unknown service");
	  if (errno == ECONNREFUSED)
	    warning ("remote host doesn't support Kerberos");
	  if (errno == ENOENT)
	    warning ("can't provide Kerberos auth data");
	  goto try_connect;
	}
    }
  else
    {
      if (doencrypt)
	errx (1, "the -x flag requires Kerberos authentication");
      rem = rcmd (&host, sp->s_port, pw->pw_name, user, args, &rfd2);
    }
#else
  rem = rcmd (&host, sp->s_port, pw->pw_name, user, args, &rfd2);
#endif

  if (rem < 0)
    exit (1);

  if (rfd2 < 0)
    errx (1, "can't establish stderr");

  if (dflag)
    {
      int one = 1;
      if (setsockopt (rem, SOL_SOCKET, SO_DEBUG, (char *) &one,
		      sizeof one) < 0)
	warn ("setsockopt");
      if (setsockopt (rfd2, SOL_SOCKET, SO_DEBUG, (char *) &one,
		      sizeof one) < 0)
	warn ("setsockopt");
    }

  seteuid (uid);
  setuid (uid);
  if(!zflag){
#ifdef HAVE_SIGACTION
  sigemptyset (&sigs);
  sigaddset (&sigs, SIGINT);
  sigaddset (&sigs, SIGQUIT);
  sigaddset (&sigs, SIGTERM);
  sigprocmask (SIG_BLOCK, &sigs, &osigs);
#else
  sigs = sigmask (SIGINT) | sigmask (SIGQUIT) | sigmask (SIGTERM);
  osigs = sigblock (sigs);
#endif
  if (signal (SIGINT, SIG_IGN) != SIG_IGN)
    signal (SIGINT, sendsig);
  if (signal (SIGQUIT, SIG_IGN) != SIG_IGN)
    signal (SIGQUIT, sendsig);
  if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
    signal (SIGTERM, sendsig);

  if (!nflag)
    {
      pid = fork ();
      if (pid < 0)
	err (1, "fork");
    }

#ifdef KERBEROS
#ifdef ENCRYPTION
  if (!doencrypt)
#endif
#endif
    {
      int one = 1;
      ioctl (rfd2, FIONBIO, &one);
      ioctl (rem, FIONBIO, &one);
    }

  talk (nflag, &osigs, pid, rem);

  if (!nflag)kill (pid, SIGKILL);
  
  } /* !zflag condition*/
  
  /* multiple host - get next host */
  if(fflag){  host=readnexthost(hostfile); }
  else {      host=strtok(NULL,", ");      }
  
  if(host==NULL)break;
  
  /* Mathog, have to manually close the socket before it 
  can move on to the next node?  Apparently this can be omitted
  at least in some cases, but it seems like a good idea, so leave
  it in. */
  shutstat=shutdown(rem, SHUT_RDWR);
  
  /*MATHOG, restore root's euid, else the next rcmd generates:
    rcmd: socket: Permission denied
  */
  seteuid (0);
  
  }  /* loop on host list */
  return 0;
}

void
talk (int nflag, sigset_t *osigs, pid_t pid, int rem)
{
  int cc, wc;
  fd_set readfrom, ready, rembits;
  char *bp, buf[BUFSIZ];

  if (!nflag && pid == 0)
    {
      close (rfd2);

    reread:
      errno = 0;
      if(key==0){
        if ((cc = read (STDIN_FILENO, buf, sizeof buf)) <= 0)goto done;
      }
      else {
       /* expects: string\n where cc includes the final null character  */
       getmessagefromq(buf,&cc,key,mtype);
      }
      bp = buf;

    rewrite:
      FD_ZERO (&rembits);
      FD_SET (rem, &rembits);
      if (select (rem + 1, 0, &rembits, 0, 0) < 0)
	{
	  if (errno != EINTR)
	    err (1, "select");
	  goto rewrite;
	}
      if (!FD_ISSET (rem, &rembits))
	goto rewrite;
#ifdef KERBEROS
#ifdef ENCRYPTION
      if (doencrypt)
	wc = des_write (rem, bp, cc);
      else
#endif
#endif
	wc = write (rem, bp, cc);
      if (wc < 0)
	{
	  if (errno == EWOULDBLOCK)
	    goto rewrite;
	  goto done;
	}
      bp += wc;
      cc -= wc;
      if (cc == 0)
	goto reread;
      goto rewrite;
    done:
      shutdown (rem, 1);
      exit (0);
    }

#ifdef HAVE_SIGACTION
  sigprocmask (SIG_SETMASK, osigs, NULL);
#else
  sigsetmask (*osigs);
#endif
  FD_ZERO (&readfrom);
  FD_SET (rfd2, &readfrom);
  FD_SET (rem, &readfrom);
  do
    {
      int maxfd = rem;
      if (rfd2 > maxfd)
	maxfd = rfd2;
      ready = readfrom;
      if (select (maxfd + 1, &ready, 0, 0, 0) < 0)
	{
	  if (errno != EINTR)
	    err (1, "select");
	  continue;
	}
      if (FD_ISSET (rfd2, &ready))
	{
	  errno = 0;
#ifdef KERBEROS
#ifdef CRYPT
	  if (doenencryption)
	    cc = des_read (rfd2, buf, sizeof buf);
	  else
#endif
#endif
	    cc = read (rfd2, buf, sizeof buf);
	  if (cc <= 0)
	    {
	      if (errno != EWOULDBLOCK)
		FD_CLR (rfd2, &readfrom);
	    }
	  else
	    write (2, buf, cc);
	}
      if (FD_ISSET (rem, &ready))
	{
	  errno = 0;
#ifdef KERBEROS
#ifdef ENCRYPTION
	  if (doencrypt)
	    cc = des_read (rem, buf, sizeof buf);
	  else
#endif
#endif
	    cc = read (rem, buf, sizeof buf);
	  if (cc <= 0)
	    {
	      if (errno != EWOULDBLOCK)
		FD_CLR (rem, &readfrom);
	    }
	  else
	    write (1, buf, cc);
	}
    } while (FD_ISSET (rfd2, &readfrom) || FD_ISSET (rem, &readfrom));
}

void
sendsig (int sig)
{
  char signo;

  signo = sig;
#ifdef KERBEROS
#ifdef ENCRYPTION
  if (doencrypt)
    des_write (rfd2, &signo, 1);
  else
#endif
#endif
    write (rfd2, &signo, 1);
}

#ifdef KERBEROS
/* VARARGS */
void
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
warning (const char * fmt, ...)
#else
warning (va_alist)
va_dcl
#endif
{
  va_list ap;
#if !(defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__)
  const char *fmt;
#endif

  fprintf (stderr, "%s: warning, using standard rsh: ", __progname);
#if defined(HAVE_STDARG_H) && defined(__STDC__) && __STDC__
  va_start (ap, fmt);
#else
  va_start (ap);
#endif
  fmt = va_arg (ap, char *);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fprintf (stderr, ".\n");
}
#endif

char *
copyargs (char **argv)
{
  int cc;
  char **ap, *args, *p;

  cc = 0;
  for (ap = argv; *ap; ++ap)
    cc += strlen (*ap) + 1;
  if (!(args = malloc ((u_int)cc)))
    err (1, NULL);
  for (p = args, ap = argv; *ap; ++ap)
    {
      strcpy (p, *ap);
      for (p = strcpy (p, *ap); *p; ++p);
      if (ap[1])
	*p++ = ' ';
    }
  return args;
}

char *readnexthost(FILE *hostfile){
char *newline;
  while( fgets(hostbuffer,MAXHOSTNAMELEN,hostfile) != NULL){
    newline=strstr(hostbuffer,"\n");
    if(newline == NULL){   /* string truncated, this is fatal in this program */
	fprintf(stderr,"rsh: host name found in host file is too long\n");
	exit(EXIT_FAILURE);
    }
    *newline='\0'; /* replace the \n with a terminator */
    if(*hostbuffer=='#')continue;
    return hostbuffer;
  }
  return NULL;
}

void getmessagefromq(char *buf, int *cc, int key, int mtype){
int msgqid;
int result;

    msgqid = msgget((key_t)key,0);
    if(msgqid == -1)exit(EXIT_FAILURE);
    if(mtype >= 0 ){  thismsgbuf.mtype=mtype; }
    else {            thismsgbuf.mtype=-mtype;}
    result = msgrcv(msgqid, (void *) &thismsgbuf, BUFSIZ, mtype, 0); /*lock and wait for it*/
    if(result==-1)exit(EXIT_FAILURE);
    /* string MUST end in \n and CC must point to it.
       Not sure if an empty message is possible, but handle that too just in case */
    if(result == 0 || thismsgbuf.mtext[result-1] != '\n'){
      if(result >= BUFSIZ-1)exit(EXIT_FAILURE); /* no room for the \n */
      thismsgbuf.mtext[result]='\n';
    } 
    result++;
    (void) strncpy(buf,thismsgbuf.mtext,result);
    *cc=result;
}
