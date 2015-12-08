# CANAL


## Main
`main.sh` - runs cflow and each module of the solution 

## Cflow related parsing
`env_setup.sh` - sets up environment for cflow parsing  
`cflowGenerate.sh` - runs cflow on a set of c files  
`cflowVisualize.sh` - visualizes in PNG diagrams for an entire dir of cflow files

## File scraping 
`doc_scrape.py` - does the web scraping to get files

## Dangerous A before B
`check_a_before_b.py` - looks for occurences of functions in, a, occuring before, b.

## Bad word search
`check_bad_words.py` looks for dangerous functions like `execve`, `gets`, `strcat`, and `setuid`.

## Exec* Checking & Backtracking
`check_exec.py` - Backtracks potentially dangerous exec* calls and prints their real string arguments.

## Return checking
`check_return.py` - looks for unused/unset return values
