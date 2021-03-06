# SETUID NOTES

There's actually not that many static analysis tools for C/C++.

See ``/test/README` for more information and my testing of 3rd party tools, cppcheck and flawfinder.

## What we promised Egele

- Apple sauce malloc occuring before dropping privileges.
    - Because: Dangerous because of Malloc logging bug.
    - Difficulty: Doable.
- Signal handling before dropping.
    - Because: Often misunderstood, can have format strings, and other things.
    - Note: Potentially lot of false pos.
    - Difficulty: Doable.
- Exec* before dropping.
    - Because: Exec as privileged user.
    - Difficulty: Doable.
- Not checking setuid return.
    - Because: Dropping privilege might've failed.
    - Difficulty: Doable.
- Assuming privileges are dropped in all shells code.
    - Because: Different behaviour in different OSes.
    - Difficulty: Hard.
- Unsafe function use near seteuid.
    - Because: Buffer overflow? Seems the issue is not seteuid though, but a general misuse of a bad func.
    - Difficulty: Doable-ish.


## Notes from, https://www.usenix.org/legacy/event/sec02/full_papers/chen/chen.pdf

In some applications, a user process needs extra privileges, such as permission to read the password file.
By the principle of least privilege, the process should drop its privileges as soon as possible to minimize risk to
the system should it be compromised and execute malicious code.

Unix systems offer a set of system calls, called the uid-setting system calls, for a process to raise and
drop privileges. Such a process is called a setuid process. Unfortunately, for historical reasons, the
uid-setting system calls are poorly designed, insufficiently documented, and widely misunderstood. Unix setuid
model is mysterious, and the resulting confusion has caused many security vulnerabilities.

We approach  the  setuid  mystery  as  follows.   First,  we study  the  semantics  of  the  uid-setting  system
calls  by reading kernel sources. We compare and contrast the se- mantics among different Unix systems, which is
useful for authors of setuid programs. In doing so,  we found that manual inspection is tedious and error-prone.
This motivates our second contribution:  we construct a formal model to capture the behavior of the operating system
and use it to guide our analysis.

Each process has three user IDs: the real user ID ( real uid , or ruid ), the effective user ID (effective uid,
or euid), and the saved user ID (saved uid, or suid).

When a process is created by fork , it inherits the three user IDs from its parent process.
When a process executes a new file by exec,  it keeps its three user IDs unless the set-user-ID bit of the new file
is set, in which case the effective uid and saved uid are assigned the user ID of the owner of the new file.

First,  it checks if the process has permission to invoke the system call. If so, it then modifies the user IDs
of the process according to certain rules

Essentially,  the  term appropriate  privilege serves  as  a wildcard  that  allows  compliant  operating  systems
to use  any  policy  whatsoever  for  deeming  when  a  call to setuid should  be  allowed.  The conditional
flag { POSIX SAVED IDS }

POSIX specifies that if a process has appropriate priv- ileges , setuid(newuid) sets all three user IDs to newuid;
otherwise, setuid(newuid) only sets the effective uid to newuid (if newuid is equal to the real uid or saved uid)
As described in Section 5.2, in Linux or Solaris, if the effective user ID is zero, setuid(newuid) sets all
three user IDs to newuid ; otherwise, it sets only the effective user ID to newuid.

## Notes from reading ~5-7 papers on static code analysis
Some papers on the topic: https://scholar.google.com/scholar?q=static+source+code+flow+analysis&btnG=&hl=en&as_sdt=0%2C22

Few interesting ways people go about static code analysis.
1. Model based. Establish some sort of model that the code should be matching based on a spec
2. Flow-sensitive. Track the flow of the code for bad stuff. i.e. use-after-free.
3. Taint analysis. Set up source and sinks, track what data touches on its journey from source to sink
4. White-list based. If X occurs then Y must occur before it.
5. Context-insensitive. Just grep essentially.

flow-sensitive + context-sensitive is what we're doing

http://web.stanford.edu/~engler/suny-distinguished-lecture.pdf - sums up metacompilation which is a cross between
    compiling the code and running it through gcc to let gcc do teh work

http://web.stanford.edu/~engler/mc-osdi.pdf - seems boring, but the paper that coverity is based on

http://www.clic.cs.columbia.edu/~junfeng/reliable-software/papers/coverity.pdf - nice ppaper

> For those who keep track of such things, checkers in the research system typically traverse program paths (flow-sensitive)
in a forward direction, going across function calls (inter-procedural) while keeping track of call-site-specific
information (context-sensitive)

​> Thus, most require that checking runs complete in 12 hours, though those with larger code bases (10+MLOC) grudgingly
accept 24 hours. A tool that cannot analyze at least 1,400 lines of code per minute makes it difficult to meet these
targets.
​
> Our approach in the initial months of commercialization in 2002 was a low-tech, read-only replay of the build commands:
run make, record its out - put in a file, and rewrite the invocations to their compiler (such as gcc) to instead call our
checking tool, then rerun everything. Easy and simple.

​> The right approach, which we have used for the past seven years, kicks off the build process and intercepts every system
call it invokes. As a result, we can see everything needed for checking, including the exact executables invoked, their
command lines, the directory they run in, and the version of the com piler (needed for compiler-bug work-arounds).

> We have thus far discussed only C, a simple language; C++ compilers diverge to an even worse degree, and we go to
great lengths to support them. On the other hand, C# and Java have been eas ier, since we analyze the bytecode they
compile to rather than their source

> At the most basic level, errors found with little analysis are often better than errors found with deeper tricks. A good
error is probable, a true error, easy to diagnose; best is difficult to misdiagnose. As the number of analysis steps
increases, so, too, does the chance of analysismistake, user confusion, or the perceived improbability of event sequence.

---

cflow might not give us everything we want, sicne we not only care what gets called, but also may care what operations
happen before
* like x = 3; will not tracked by cflow

Somebody look into:
1. http://stackoverflow.com/questions/31154555/parsing-fdump-tree-cfg-output-from-gcc
  1. https://gcc.gnu.org/ml/gcc/2009-11/msg00395/dump2dot
2. Parsing -fdump-tree-cfg output from GCC
3. https://github.com/eliben/pycparser- give us more power than what cflow gives



## Some more papers

http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.229.3952&rep=rep1&type=pdf - Good paper that covers a lot in
the field of static code anal
