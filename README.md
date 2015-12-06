#Project_Name_Logo_Here

This tool set is a compilation of `python` and `bash` scripts intended to provide an automized bug checker for *.c files that use set*id() family functions.

Simple_Example.c:
```c
#include <stdio.h>

int main(){
	return 0;
}
```

The above vulnerable file will produce the following warning/error dump:
```sh
./top_module_replace_this /root/folder
ERROR: info here
``` 

#Main Static Analysis Modules:

* `check_a_before_b` : this module uses cflow to take in a parent directory and recursively
* `check_return` : this module uses
* `check_badwords` : 
* `check_sys` :  

#Documentation

Our full documentation is available at [this link.](KIERK_WRITE_UP.odt)

##Integrate Into README.md:

TODO(eugenek): Write more here

./eval - Testing of third party applications and seeing if they service our goal
./test - Test files for our solution
./src
    
