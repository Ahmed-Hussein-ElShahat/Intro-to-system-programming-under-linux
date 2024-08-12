#include "utilities.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>        
#include <unistd.h>
#define _GNU_SOURCE


#define argsize 200
#define variableSize 1000 //size for loacl and environment varables
#define histsize 10


char filename(char *source, char *dest);
void change_hist(history *hist, char *buff, char *argv[], char exit_stat);
void change_lclvariable(char *str);

int bash_pid; //bash_pid is used to know if I am in bash or one of its children
int fd[2] = {0, 1}; //make an array that carry the fd of the input and output
		    //to be able to manipulate the reading and writing during pipe
char *lclvariables[variableSize];

int main(){
	bash_pid = getpid();

	//dup the pts symbolic link to be used after redirection
	//aslo make a flag to know if redirection happened
	dup2(0, 255);	
	char redirect_flag = 0;

	//change the environment varaible form stack to heap
	for (int i=0; environ[i] != NULL; i++)environ[i] = strdup(environ[i]);

	// construct an array having all the built-in commands (ends with a NULL) and specify the number of commands
	char *cmds[] = {"exit", "echo", "cp", "mv", "pwd", "help","cd","type", "envir", "phist", "free", NULL};
	int cmd_count = sizeof(cmds)/sizeof(char *) - 1; 


	//construct an array of hist struct
	history hist[histsize];
	memset(hist, 0, histsize*sizeof(history));

	write(STDOUT, "Hello to the new bash\n", strlen("Hello to the new bash\n"));

	//The loop prints the shell prompt and handles  all inputs
	while(1){
		write(STDOUT, "ektb ely t7bo> ", strlen("ektb ely t7bo> "));

		//construct an array that holds the input(removes the newline special character) and get its size
		char *buff = (char *)malloc(argsize * sizeof(char));
		fgets(buff, argsize, stdin);

		int len = strlen(buff);
		if(len==1)continue;

		//remove the "\n" and decrease the len of the buff
		len--;
		buff[len] = 0;

		while(buff[len-1] == '|'){
			write(STDOUT, "write the pipe process >", strlen("write the pipe process >"));

			//get the rest of the buff and concatenate it with the buff
			int n = argsize - strlen(buff);
			char *rest_of_buff = (char *) malloc(n * sizeof(char));
			fgets(rest_of_buff, n, stdin);
			strcat(buff, rest_of_buff);

			//remove the "\n" and decrease the len of the buff
			len--;
			buff[len] = 0;

			free(rest_of_buff);
		}

		//exit -> exits the bash
		if(!strcmp(buff, "exit")){
			write(STDOUT, "Goodbye\n", strlen("Goodbye\n"));
			exit(0);
		}

		//if bash didn't exit so we will get the exit status of the command
		int exit_status;

		//parsing the input in an array of tokens (argv) with a length argc
		char **token = (char **) malloc(100 * sizeof(char *));
		char *argv[100];

		int i=0;
		int argc=0;
		char *rest= buff;

		for(i=0; token[i] = strtok_r(rest, " ", &rest); i++);
		token[i]=NULL;

		int next_fd[2] = {-1, -1}; //fd to link different pipes with each other
		char pipe_flag = 0; // a flag which is used to show if a pipe isn't done (0),
				    // pipe is done and in parent (1) or
				    // pipe is done and in child  (2) 

				    //loop on the token
				    //if a redirection symbol is detected handle redirection
				    //else it's an argument

		char variable_flag=0; // 0 -> no envir or lclvariable
				      // 1 -> envir
				      // 2 -> lclvariable
		for(i=0; token[i]; i++){
			//if pipe was initialized so in parent ignore all tokens except for another pipe
			if(pipe_flag == 1 && strcmp(token[i], "|")) continue;


			if(!strcmp(token[i],">")){

				if(token[++i]){
					//open the file given or create if not found, and replace stdout with it
					int fd_o = open(token[i], O_WRONLY|O_CREAT, 0644);
					redirect_flag = 1; 
					dup2(fd_o,1);
					close(fd_o);
				}
				else{write(2, "no file given for Output redirection\n", strlen("no file given for Output redirection\n"));}
			}

			else if(!strcmp(token[i],"2>")){

				if(token[++i]){
					//open the file given or create if not found, and replace stderr with it
					int fd_err = open(token[i], O_WRONLY|O_CREAT, 0644);
					redirect_flag = 1; 
					dup2(fd_err,2);
					close(fd_err);
				}

				else{write(2, "no file given for error redirection\n", strlen("no file given for error redirection\n"));}
			}

			else if(!strcmp(token[i],"<")){

				if(token[++i]){
					//open the file given and replace the stdin with it
					int fd_in = open(token[i], O_RDONLY);
					//if the file isn't found
					if(fd_in==-1)
					{
						write(2, "There is no such file or directory\n", strlen("There is no such file or directory\n"));
					}

					else
					{
						redirect_flag = 1;
						dup2(fd_in,0);
						close(fd_in);
					}
				}
				else{write(2, "no file given for input redirection\n", strlen("no file given for input redirection\n"));}
			}

			else if(!strcmp(token[i],"|")){
				//if a child of the bash want to pipe then break to make the ppid is the bash_pid and then pipe
				if(pipe_flag == 2) break;

				if(token[++i]){

					if(pipe_flag != 1 && pipe(fd) != 0){
						write(STDERR, "couldn't make a pipe\n", strlen("couldn't make a pipe\n"));
						continue;
					}
					if(pipe(next_fd) != 0){
						write(STDERR, "couldn't make a pipe\n", strlen("couldn't make a pipe\n"));
						continue;
					}

					//for each pipe we need to fork
					int ret_pid = fork();

					if(ret_pid < -1){
						write(2, "Couldn't fork\n", strlen("Couldn't fork\n"));
						close(fd[0]);
						close(fd[1]);
						continue;
					}

					//at the child we will start taking arguments starting from the token after the pipe symbol
					else if(ret_pid == 0){
						//here is the child
						pipe_flag = 2;
						argc = 0;
						argv[argc++] = token[i];


						dup2(fd[0], STDIN);
						dup2(next_fd[1], STDOUT);


						//close the fd made from the pipe
						close(fd[0]);
						close(fd[1]);	

						close(next_fd[0]);
						close(next_fd[1]);

					}

					// The parent has finished taking its arguments and should start its function
					else if(ret_pid > 0){

						//replace the stdout with the write end of the pipe for the first time only and leave the read as it is
						if(pipe_flag == 0){
							dup2(fd[1], STDOUT);
						}

						dup2(next_fd[0] , fd[0]);

						close(fd[1]);
						close(next_fd[0]);
						close(next_fd[1]);

						pipe_flag = 1;

					}
				}


			}

			else{argv[argc++] = token[i];}
		}

		//null the last element in the argv
		argv[argc]=NULL;

		if(pipe_flag){
			//close all fds so it isn't a read end a
			close(fd[0]);
			close(fd[1]);
			close(next_fd[0]);
			close(next_fd[1]);
		}
		//if pipe was made and the last child is met make its output be printed at the terminal
		if(pipe_flag == 2 && token[i] == 0)dup2(255, STDOUT);

		//Checking if the inputs s from the built-in commands and hanlding them if they are	
		if(!strcmp(argv[0], "echo")){
			exit_status = myecho(argc, argv);
		}

		else if(!strcmp(argv[0], "cp")){
			filename(argv[argc-2], argv[argc-1]);
			exit_status = mycp(argc, argv);
		}

		else if(!strcmp(argv[0], "mv")){
			filename(argv[argc-2], argv[argc-1]);
			exit_status = mymv(argc, argv);

		}

		else if(!strcmp(argv[0], "pwd")){
			exit_status = mypwd(argc);

		}

		else if(!strcmp(argv[0], "cd")){
			exit_status = mycd(argc, argv);
		}

		//specifies the type of the command whether builtin, external or invalid
		else if(!strcmp(argv[0], "type")){
			exit_status = mytype(argv, cmds);
		}

		//prints the last ten commands with their exit status
		else if(!strcmp(argv[0], "phist")){
			exit_status = phist(hist);
		}

		//prints all the environment cariables
		else if(!strcmp(argv[0], "envir")){
			exit_status = envir(argc, argv);
		}

		else if(!strcmp(argv[0], "free")){
			exit_status = myfree();
		}

		else if(!strcmp(argv[0], "uptime")){
			exit_status = myuptime();
		}

		else if(variable_flag != 0){
			exit_status = 0;
		}
		else if(!strcmp(argv[0], "allVar")){
			allVar();
			exit_status = 0;
		}

		else if(!strcmp(argv[0], "help")){

			//help prints all built-in commands and what they do
			char *help_statment = "echo\treprints the geiven arguments\ncp\tcopies the source file to the destination\nmv\tmoves the source file to the destination\ncd\tchanges the current directory\npwd\tprints the curent working directory\ntype\tprints the type of the command(builtin or external or invalid command)\nenvir\tprints all environment variables\nexit\tleaves the bash\n";	
			write(STDOUT,help_statment, strlen(help_statment));
			exit_status = 0;
		}


		else{
			if(getpid() == bash_pid){// if in the main bash
				int ret_val = fork();

				if(ret_val == -1){
					write(STDOUT, "Couldn't fork\n", strlen("Couldn't fork\n"));
				}
				else if(ret_val==0){
					//here is the child
					execvp(argv[0], argv);

					//if the input didn't match one of the commands so it's an invalid command
					write(STDERR, "Invalid command name\n", strlen("Invalid command name\n"));
					exit(-1);
				}

				else{
					//here is the parent
					int status;
					//wait for the child process
					wait(&status);
					exit_status = WEXITSTATUS(status);
				}
			}

			else{
				//in case of piping execute immediately
				execvp(argv[0], argv);
				write(STDERR, "Invalid command name\n", strlen("Invalid command name\n"));
				exit(-1);

			}
		}


		dup2(255, STDOUT);
		int status;
		while(wait(&status) != -1);
		exit_status = WEXITSTATUS(status);

		//its time now for changing history 
		change_hist(hist, buff, token, exit_status);

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
void change_hist(history *hist, char *buff, char *arguments[], char exit_stat){
	for(int i=0; i<10; i++){
		//if the argv is null so it hasn't been used yet
		if(hist[i].argv==0){
			hist[i].argv = arguments;
			hist[i].exit_status = exit_stat;
			hist[i].buff = buff;
			return;
		}
	}

	//free the first history arguments
	free(hist[9].argv[0]);

	//The older commands are first then comes the new ones
	for(int i=1; i<9; i++){
		hist[i-1] = hist[i];
	}
	hist[9].argv = arguments;
	hist[9].exit_status = exit_stat;
	return;

}

void change_lclvariable(char *str){
	int found=0;
	int j;
	for(j=0; lclvariables[j]!=NULL && j!=variableSize; j++)
	{
		if(strcmp_till_char(str, lclvariables[j], '='))
		{
			free(lclvariables[j]);
			lclvariables[j] = strdup(str);
			found=1;
			return;
		}
	}
	if(!found)
	{
		lclvariables[j]=strdup(str);
		lclvariables[j+1]=0;
	}	
}
