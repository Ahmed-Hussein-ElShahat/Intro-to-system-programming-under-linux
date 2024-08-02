#include "utilities.h"
#include <sys/wait.h>
#include <sys/stat.h>

#define argsize 200
#define histsize 10


char filename(char *source, char *dest);
void change_hist(history *hist, char *argv[], char exit_stat);

int main(){

	// construct an array having all the built-in commands (ends with a NULL) and specify the number of commands
	char *cmds[] = {"exit", "echo", "cp", "mv", "pwd", "help","cd","type", "envir", "phist", NULL};
	int cmd_count = sizeof(cmds)/sizeof(char *) - 1; 
	
        //construct an array of hist struct
    	history hist[10];
	memset(hist, 0, 10*sizeof(history));

	write(1, "Hello to the new bash\n", strlen("Hello to the new bash\n"));

	//The loop prints the shell prompt and handles  all inputs
	while(1){
		write(1, "ektb ely t7bo> ", strlen("ektb ely t7bo> "));

		//construct an array that holds the input(removes the newline special character) and get its size
		char *buff = (char *)malloc(argsize * sizeof(char));
		fgets(buff, argsize, stdin);
		int len = strlen(buff);
		if(len==1)continue;
		buff[len-1] = 0;

		//exit -> exits the bash
		if(!strcmp(buff, "exit")){
			write(1, "Goodbye\n", strlen("Goodbye\n"));
			exit(0);
		}

		//parsing the input in an array of tokens (argv) with a length argc
		char **argv = (char **)malloc(100 * sizeof(char *));
		int argc=0;
		char *rest= buff;
		while(argv[argc] = strtok_r(rest, " ", &rest))argc++;
		argv[argc]=NULL;

		//Checking if the inputs s from the built-in commands and hanlding them if they are	
		if(!strcmp(argv[0], "echo")){
			int exit = myecho(argc, argv);
			change_hist(hist, argv, exit);
		}

		else if(!strcmp(argv[0], "cp")){
			filename(argv[argc-2], argv[argc-1]);
			int exit = mycp(argc, argv);
			change_hist(hist, argv, exit);
		}

		else if(!strcmp(argv[0], "mv")){
			filename(argv[argc-2], argv[argc-1]);
			int exit = mymv(argc, argv);
			change_hist(hist, argv, exit);

		}

		else if(!strcmp(argv[0], "pwd")){
			int exit = mypwd(argc);
			change_hist(hist, argv, exit);

		}

		else if(!strcmp(argv[0], "cd")){
			int exit = mycd(argc, argv);
			change_hist(hist, argv, exit);

		}

		//specifies the type of the command whether builtin, external or invalid
		else if(!strcmp(argv[0], "type")){
			int exit = type(argv, cmds);
			change_hist(hist, argv, exit);
		}

		//prints the last ten commands with their exit status
		else if(!strcmp(argv[0], "phist")){
			int exit = phist(hist);
			change_hist(hist, argv, exit);
		}

		//prints all the environment cariables
		else if(!strcmp(argv[0], "envir")){
			int exit = envir(argc, argv);
			change_hist(hist, argv, exit);
		}

		else if(!strcmp(argv[0], "help")){

			//help prints all built-in commands and what they do
			char *help_statment = "echo\treprints the geiven arguments\ncp\tcopies the source file to the destination\nmv\tmoves the source file to the destination\ncd\tchanges the current directory\npwd\tprints the curent working directory\ntype\tprints the type of the command(builtin or external or invalid command)\nenvir\tprints all environment variables\nexit\tleaves the bash\n";	
			write(1,help_statment, strlen(help_statment));
			change_hist(hist, argv, 0);
		}
		
		
		//if the input didn't match one of the commands so it's an invalid command
		else{
			int ret_val = fork();
			
			if(ret_val == -1){
				write(1, "Couldn't fork\n", strlen("Couldn't fork\n"));
			}
			else if(ret_val==0){
				//here is the child	
				execvp(argv[0], argv);
				write(1, "Invalid command name\n", strlen("Invalid command name\n"));
				change_hist(hist, argv, -1);

			}
			else{
				//here is the parent
				int status;
				wait(&status);
				change_hist(hist, argv, status);

			}
			
		}
	}
	exit(0);
}




//filename function changes the destination file name so it has a valid name to be used in the functions
char filename(char *source, char *dest){

	//calculate the legth of the destination array
	int end=0;
	while(dest[end]!=0)end++;

	//if the destination is a directory then we should add a '/' so that then we can specify the name of the file
	if(is_dir(dest) && dest[end-1]!='/')
	{
		dest[end]='/';
		dest[++end]=0;
	}

	//if destination file ends with '/' so we can concatenate the source file name so both of them have the same name
	//(since the destination file name wasn't given)
	if(dest[end-1]=='/'){
		char *first_char;
		char *name;

		//Duplicate the file name (starts from the last slash '/') 
		if(first_char = strrchr(source, '/')){
			name = strdup(++first_char);
		}
		else{
			name = strdup(source);
		}

		//concatenate the name of the file to the destination
		dest = strcat(dest, name);
		return 1;
	}
	return 0;
}


//changes the history array to  hold the write values
void change_hist(history *hist, char *arguments[], char exit_stat){
	for(int i=0; i<10; i++){
		if(hist[i].argv==0){
			hist[i].argv = (char **)arguments;
			hist[i].exit_status = exit_stat;
			return;
		}
	}
	for(int i=0; i<9; i++){
		hist[i] = hist[i+1];
	}
	hist[9].argv = arguments;
        hist[9].exit_status = exit_stat;
        return;

}
