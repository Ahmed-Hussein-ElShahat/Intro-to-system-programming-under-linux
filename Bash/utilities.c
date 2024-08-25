#include "utilities.h"

int read_write_files(int fd1, int fd2); //read the file and then write in the other file

int myecho(int argc, char *argv[])
{
	if(argc<2)
	{
		write(2, "missing arguments to print\n", strlen("missing arguments to print\n"));
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)

	}
	for(int i=1; i<argc; i++)
	{
		char *out = (char *) malloc( strlen(argv[i]) * sizeof(char) + 2);
                int n = sprintf(out,"%s ", argv[i]);
                write(STDOUT, out, n);
	}
	write(STDOUT, "\n", 1);
	return 0;
}

int mycp(int argc, char *argv[])
{
	if(argc < 3 && argc > 4)
	{
		write(2, "only the sorce and destination file should be used beside thee name of command, you can use the option -a also to append\nexample: cp [-a] source dest/[file_name]\n", strlen("only the sorce and destination file should be used beside thee name of command, you can use the option -a also to append\nexample: cp [-a] source dest/[file_name]\n"));
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
		else return -1; // return to the bash if an error happened in the parent(bash)
	}
	int fd1 = open(argv[argc-2], O_RDONLY);
	if(fd1 == -1){
		write(2, "The pathname is incorrect or no access permission for the source file\n", strlen("The pathname is incorrect or no access permission for the source file\n"));

		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

	int opt;
	char opt_flag=0;
	while((opt = getopt(argc, argv, "a")) != -1)
	{
		if(opt=='a')
			opt_flag=1;
		else
		{
			write(2, "The only available option is -a to append on destination file\n", strlen("The only available option is -a to append on destination file\n"));
			close(fd1);
	
			if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
	                else return -1; // return to the bash if an error happened in the parent(bash)
		}
	}

	int fd2;
	if(!opt_flag) 
		fd2 = open(argv[argc-1], O_WRONLY|O_CREAT|O_EXCL, 0644);

	else 
		fd2 = open(argv[argc-1], O_WRONLY|O_APPEND|O_CREAT, 0644);

	if(fd2==-1){
		if(!opt_flag)
		{
			write(2, "The file already exist, use -a option to append on the file\n", strlen("The file already exist, use -a option to append on the file\n"));
		}
		else{
			write(2, "No access permission for the destination file\n", strlen("No access permission for the destination file\n"));
		}
		close(fd1);
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

	if(read_write_files(fd1, fd2)==-1)
	{
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

	return 0;
}

int mymv(int argc, char *argv[])
{
	
	if(argc < 3 && argc > 4)
	{
		char *out =  "only the sorce and destination file should be used beside the name of command, you can use the option -f to force move to the destination\nexample: mv [-f] source dest/[file_name]\n";
		write(2, out, strlen(out));

		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

	int fd1 = open(argv[argc-2], O_RDONLY);
	if(fd1 == -1){
		write(2, "The pathname is incorrect or no access permission for the source file\n", strlen("The pathname is incorrect or no access permission for the source file\n"));
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

	int opt;
	char opt_flag=0;
	while((opt = getopt(argc, argv, "f")) != -1)
	{
		if(opt=='f')
			opt_flag=1;
		else
		{
			char *out = "The only available option is -f to force write on the file with the same destination file name\n";
			write(2, out, strlen(out));
			close(fd1);
			if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
	                else return -1; // return to the bash if an error happened in the parent(bash)
		}
	}

	int fd2;
	if(!opt_flag)
		fd2 = open(argv[argc-1], O_WRONLY|O_CREAT|O_EXCL, 0644);

	else 
		fd2 = open(argv[argc-1], O_WRONLY|O_CREAT, 0644);

	if(fd2==-1){
		if(!opt_flag)
		{
			write(2, "The file already exist, you can force move the file using -f option\n", strlen("The file already exist, you can force move the file using -f option\n"));
			if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                        else return -1; // return to the bash if an error happened in the parent(bash)
		}

		else{
			write(2, "No access permission for the file\n", strlen("No access permission for the file\n"));		
			if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                        else return -1; // return to the bash if an error happened in the parent(bash)
		}	
		close(fd1);
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}
	if(read_write_files(fd1,fd2)==-1){
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

	remove(argv[argc-2]);
	return 0;
}

int mypwd(int argc)
{
	if(argc>1){
		write(2, "pwd doesn't accept any arguments\n", strlen("pwd doesn't accept any arguments\n"));
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	
	}
	char buff[200];
	char *ret_ptr;
	ret_ptr = getcwd(buff, 200);
	if(ret_ptr==0)
	{
		write(2, "couldn't print the current working directory\n", strlen("couldn't print the current working directory\n"));
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}
	char out[201];
	sprintf(out, "%s\n", buff);
	write(STDOUT, out, strlen(out));
	return 0;

}

int mycd(int argc, char *argv[]){
	char dir[200] = {0};
	
	if(argc==1){
		// if no argument is given then the path is the home path
		strcpy(dir, mygetenv("HOME"));
	}	
	//if two arguments then may be because the strtok tokenized the path such as in  session\ 2
	else if(argc > 2){
		//if the first char inpath is \" then the path was given as a string
		if(argv[1][0] == '"'){
			//first concatenate the path again
			int i=1;	
			while(strcat(dir, argv[i++]) && i<argc){
				strcat(dir, " ");
			}

			dir[0]='\0';

			//search for the other symbol at the end of the directory
			for(i=0; dir[i]!= '"'; i++);
			dir[i]='\0';

			//remove the first one
			strcpy(dir, &dir[1]);
		}
		else{
		char *instruction = "Only one argument which is the desired working directory is allowed\nexample:\t cd /usr/bin\nor:\t cd \"session 5\"";
		write(2, instruction, strlen(instruction));
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
		}
	}

	//if only one argument then it's the path
	else strcpy(dir, argv[1]);
	
	//if the path is not a directory
	if(!is_dir(dir)){
		char *instruction = "The given path is not a directory\n";
                write(2, instruction, strlen(instruction));
                
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

	//if the input format is right then execute the syscall and handle the errors
	int ret_value = chdir(dir);

	if(ret_value == -1){
		write(2, "Permission denied or not a valid path\n", strlen("Permission denied or not a valid path\n"));
		
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}
}

int mytype(char *argv[], char *cmds[]){
	for(int i=1; argv[i]; i++){
		//search for the command in the built-in commands
		char found=0;
		for (int j=0; cmds[j]; j++){
			if(!strcmp(argv[i],cmds[j])) {
				char out[50];
				sprintf(out, "%s is a shell builtin\n",argv[i]);
				write(STDOUT, out, strlen(out));
				found=1;
				break;
			}
		}
		if(found)continue;

		//search for access function in C library
		//tokenize the PATH env variable
		char *ptr = mygetenv("PATH");
		char *token;
		while((token = strtok_r(ptr, ":", &ptr))!=NULL){
			char *path = (char *) malloc((strlen(token) + strlen(argv[i]) + 2) * sizeof(char));

			//start loading the path
			strcpy(path, token);
			strcat(path, "/");
			strcat(path, argv[i]);

			//check whether the path can be executed or not
			if(access(path, X_OK) == 0){
				char *out = (char*)malloc((strlen(argv[i]) + strlen(path) + 30) * sizeof(char));
				sprintf(out, "%s is (external) %s\n", argv[i], path);
				write(STDOUT, out, strlen(out));
				free(out);
				free(path);
				break;
			}
			
			//if not found then free the path and search again
			free(path);
		}
		// if the command wasn't found then it's not supported
		char out[50];
                sprintf(out, "%s isn't a supported command\n",argv[i]);
		write(STDOUT, out, strlen(out));
	}

	return 0;
}

int envir(int argc, char *argv[]){
	if(argc == 1){
		for(int i=0; environ[i] != NULL; i++){
			write(STDOUT, environ[i], strlen(environ[i]));
			write(STDOUT, "\n", 1);
		}
	}
	else{
		for(int i=1; argv[i]; i++){
			char *str = mygetenv(argv[i]);
			write(STDOUT, str, strlen(str));
			write(STDOUT, "\n", 1);
			free(str);
		}
	}

	return 0;
}

int phist(history *hist){
	for(int i=0;i<10;i++){
		// if no content in the argv so break the loop
		if(hist[i].argv==NULL)break;

		//loop on all the array content till argv is zero
		for(int j=0; hist[i].argv[j]!=NULL; j++){
			write(STDOUT, hist[i].argv[j], strlen(hist[i].argv[j]));
			write(STDOUT, " ", 1);
		}
		char out[6];
		int num = sprintf(out, "%d\n", (int)hist[i].exit_status);
		write(STDOUT, out, num);
	}
	return 0;
}

char *mygetenv(char *env){
	for(int i=0; environ[i] != NULL; i++){

		//if the environment variable is equal to the given varaible then return the value of the
		//environment variable in a new string
		if(strcmp_till_char(env, environ[i], '=')==0){
			//create a pointer to the value of the environment varaible
			char *ptr = strchr(environ[i], '=');
			return strdup(++ptr);
		}
	}
	return NULL;
}

char strcmp_till_char(char *s1, char *s2, char c){

	//loop till the characte and compare between the strings
	//if the character isn't found then comapre till the end of the string
	char equal=1;
	for(int i=0; s1[i]!=0  && s1[i]!=c && s2[i]!=c && s2[i]!=0; i++){
		if(s1[i]!=s2[i]){
			equal=0;
			return -1;//return -1 if they aren't equal
		}
	}
	return 0;//return 0 if they are equal
}

int read_write_files(int fd1, int fd2){
        char buff[100];
        int read_count,write_count;
        do
        {
                read_count = read(fd1, buff, 100);
                if(read_count == -1)
                {
                        write(2, "No read permission on the source file\n", strlen("No read permission on the source file\n"));
                        close(fd1);
                        close(fd2);
                        return -1;
                }

                write_count = write(fd2, buff, read_count);
                if(write_count==-1)
                {
                        write(2, "No write permission  on the destiation file\n", strlen("No write permission  on the destiation file\n"));
                        close(fd1);
                        close(fd2);
                        return -1;
                }
        }
        while(read_count);
        close(fd1);
        close(fd2);
        return 0;
}


char myfree(){
	int fd_mem = open("/proc/meminfo", O_RDONLY);

        if(fd_mem==-1){
                write(2, "could't open the file\n", strlen("could't open the file\n"));
		if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
	}

        char buff[35], ch;
        int num;
        meminfo mem;
	
	//make a for loop to take the buff so that when the  ichar is a newline 
	//that is the end of the buffer
        for(int i=0; num = read(fd_mem, &ch, 1); i++){
                if(ch =='\n'){
			//if a newline is reached so take a token till the space 
			//to know the name of the field
                        char *str = strtok(buff, ":");
			
			//according to the name of the field assign it to meminfo field struct
                        if(!strcmp(str, "MemTotal"))mem.memtotal = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "MemFree"))mem.memfree = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "MemAvailable"))mem.memavailable = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "Buffers"))mem.buf = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "Cached"))mem.cache = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "SwapTotal"))mem.swaptotal = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "SwapFree"))mem.swapfree = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "Shmem"))mem.shmem = atoi(strtok(NULL, " "));
                        else if (!strcmp(str, "SReclaimable")) mem.sreclaim = atoi(strtok(NULL, " "));
                        i=-1;
                }
                else{
                        buff[i] = ch;
                }
        }
	
        close(fd_mem);
        char str[128];

        //format the string to show the fields
        int padding = 19,inter_padding = 5;
        int n = sprintf(str, "%8s %11s %11s %11s %11s %11s %11s\n", "", "total", "used", "free", "shared", "buff/cache", "available");
        write(STDOUT, str, n);

        n = sprintf(str, "%-8s %11d %11d %11d %11d %11d %11d\n%-8s %11d %11d %11d\n",
                        "Mem:",
                        mem.memtotal,
                        mem.memtotal - mem.memavailable,
                        mem.memfree,
                        mem.shmem,
                        mem.cache + mem.sreclaim + mem.buf,
                        mem.memavailable,
                        "Swap:",
                        mem.swaptotal,
                        mem.swaptotal-mem.swapfree,
                        mem.swapfree);
        write(STDOUT, str, n);
	return 0;	
}

char myuptime(){
	//open the uptime file
	int fd_up = open("/proc/uptime", O_RDONLY);
	
	//if an error occur while opening then return -1
        if(fd_up==-1){
                write(2, "could't open the file\n", strlen("could't open the file\n"));
                if(bash_pid != getpid()) exit(-1); //exit the process if an error happend in a child
                else return -1; // return to the bash if an error happened in the parent(bash)
        }

	//read the file
	char buff[30];
	int num = read(fd_up, buff, 30);
	
	//tkenize the file to uptime and idle time
	float uptime, idletime;
	char *uptime_s = strtok(buff, " ");
	char *idletime_s = strtok(NULL, " ");

	uptime = strtof(uptime_s, NULL);
	idletime = strtof(idletime_s, NULL);
	
	//format the output string
	char str[50];
	int n = sprintf(str, "uptime = %d:%d\nidle time = %d:%d\n",
			(int)(uptime/(60*60)),
			(int)(uptime/(60)) - ((int)(uptime/(60*60)))*60,
			(int)(idletime/(60*60)),
                        (int)(idletime/(60)) - ((int)(idletime/(60*60)))*60);
	write(STDOUT, str, n);
	return 0;
	
}

void allVar(){
	for(int i=0; environ[i] != NULL; i++){
                        write(STDOUT, environ[i], strlen(environ[i]));
                        write(STDOUT, "\n", 1);
        }

	for(int i=0; lclvariables[i] != NULL; i++){
		write(STDOUT, lclvariables[i], strlen(lclvariables[i]));
                write(STDOUT, "\n", 1);
	}

}

int is_dir(char *path){
	struct stat path_stat;
        stat(path, &path_stat);
	return S_ISDIR(path_stat.st_mode);
}
