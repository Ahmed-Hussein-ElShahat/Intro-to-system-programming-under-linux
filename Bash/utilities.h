#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>


typedef struct{                 
        char **argv;            
        char exit_status;       
	char *buff;
}history; 


typedef struct{
        int memtotal;
        int memfree;
        int memavailable;
        int buf;
        int cache;
        int swaptotal;
        int swapfree;
        int shmem;
        int sreclaim;
}meminfo;


#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define variableSize 1000 

extern char **environ;
extern int bash_pid;
extern char *lclvariables[variableSize];

int myecho(int argc, char *argv[]);//echo reprints the input on the screen
int mycp(int argc, char *argv[]);//cp copies the source file to the destiantion
int mymv(int argc, char *argv[]);//mv moves the source file to the destination
int mypwd(int argc); //pwd prints the current working directory
int mycd(int argc, char *argv[]);//cd changes the current directory
int mytype(char *argv[], char *cmds[]);//specifies the type of the command whether builtin, external or invalid
int envir(int argc, char *argv[]);//prints the environment variables
				  //or the value of the environment varaible specified according to the arguments given
int phist(history *hist);//prints the last ten commands with their exit status
char *mygetenv(char *env);//get the environment variable
char myfree();//prints info about the memory
char myuptime();//prints the uptime and idletime	     
void allVar();
int is_dir(char *path);//checks whether a path is a directory or not
char strcmp_till_char(char *, char *, char );//compare between two string till the given character


#endif
