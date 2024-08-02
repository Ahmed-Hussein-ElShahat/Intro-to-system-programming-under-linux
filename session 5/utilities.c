#include "utilities.h"

char strcmp_till_char(char *, char *, char );//compare between two string till the given character
int read_write_files(int fd1, int fd2); //read the file and then write in the other file

int myecho(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("missing arguments to print\n");
		return -1;
	}
	for(int i=1; i<argc; i++)
	{
		printf("%s ",argv[i]);
	}
	printf("\n");
	return 0;
}

int mycp(int argc, char *argv[])
{
	if(argc < 3 && argc > 4)
	{
		printf("only the sorce and destination file should be used beside thee name of command, you can use the option -a also to append\nexample: cp [-a] source dest/[file_name]\n");
		return -1;
	}
	int fd1 = open(argv[argc-2], O_RDONLY);
	if(fd1 == -1){
		printf("The pathname is incorrect or no access permission for the source file\n");
		return -1;
	}

	int opt;
	char opt_flag=0;
	while((opt = getopt(argc, argv, "a")) != -1)
	{
		if(opt=='a')
			opt_flag=1;
		else
		{
			printf("The only available option is -a to append on destination file\n");
			close(fd1);
			return -1;
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
			printf("The file already exist, use -a option to append on the file\n");
		}
		else{
			printf("No access permission for the destination file\n");
		}
		close(fd1);
		return -1;
	}

	if(read_write_files(fd1, fd2)==-1)
		return -1;

	return 0;
}

int mymv(int argc, char *argv[])
{
	if(argc < 3 && argc > 4)
	{
		printf("only the sorce and destination file should be used beside the name of command, you can use the option -f to force move to the destination\nexample: mv [-f] source dest/[file_name]\n");
		return -1;
	}

	int fd1 = open(argv[argc-2], O_RDONLY);
	if(fd1 == -1){
		printf("The pathname is incorrect or no access permission for the source file\n");
		return -1;
	}

	int opt;
	char opt_flag=0;
	while((opt = getopt(argc, argv, "f")) != -1)
	{
		if(opt=='f')
			opt_flag=1;
		else
		{
			printf("The only available option is -f to force write on the file with the same destination file name\n");
			close(fd1);
			return -1;
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
			printf("The file already exist, you can force move the file using -f option\n");
		}
		else{
			printf("No access permission for the file\n");		
		}	
		close(fd1);
		return -1;
	}
	if(read_write_files(fd1,fd2)==-1)
		return -1;

	remove(argv[argc-2]);
	return 0;
}

int mypwd(int argc)
{
	if(argc>1){
		printf("pwd doesn't accept any arguments\n");
		return -1;
	}
	char buff[200];
	char *ret_ptr;
	ret_ptr = getcwd(buff, 200);
	if(ret_ptr==0)
	{
		printf("couldn't print the current working directory\n");
		return -1;
	}
	char out[201];
	sprintf(out, "%s\n", buff);
	write(1, out, strlen(out));
	return 0;

}

int mycd(int argc, char *argv[]){
	char dir[100] = {0};
	
	//if two arguments then may be because the strtok tokenized the path
	if(argc > 2){
		//if the first char inpath is \" then the path was given as a string
		if(argv[1][0] == '"'){
			//first concatenate the path again
			int i=1;	
			while(strcat(dir, argv[i++]) && i<argc){
				strcat(dir, " ");
			}

			//remove the \" symbol
			char *address =(char *) &dir;
			address[0]='\0';

			//search for the other symbol at the end of the directory
			for(i=0; dir[i]!= '"'; i++);
			dir[i]='\0';

			//remove the first one
			strcpy(dir, &dir[1]);
		}
		else{
		char *instruction = "Only one argument which is the desired working directory is allowed\nexample:/t cd /usr/bin\n";
		write(1, instruction, strlen(instruction));
		return -1;
		}
	}

	//if only one argument then it's the path
	else strcpy(dir, argv[1]);
	
	//if the path is not a directory
	if(!is_dir(dir)){
		char *instruction = "The given path is not a directory\n";
                write(1, instruction, strlen(instruction));
                return -1;

	}

	//if the input format is right then execute the syscall and handle the errors
	int ret_value = chdir(dir);

	if(ret_value == -1){
		write(1, "Permission denied or not a valid path\n", strlen("Permission denied or not a valid path\n"));
		return -1;
	}
}

int type(char *argv[], char *cmds[]){
	for(int i=1; argv[i]; i++){
		//search for the command in the built-in commands
		char found=0;
		for (int j=0; cmds[j]; j++){
			if(!strcmp(argv[i],cmds[j])) {
				char out[50];
				sprintf(out, "%s is a shell builtin\n",argv[i]);
				write(1, out, strlen(out));
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
				write(1, out, strlen(out));
				free(out);
				free(path);
				break;
			}
			free(path);
		}
	}

}

int envir(int argc, char *argv[]){
	if(argc == 1){
		for(int i=0; environ[i] != NULL; i++){
			write(1, environ[i], strlen(environ[i]));
			write(1, "\n", 1);
		}
	}
	else{
		for(int i=1; argv[i]; i++){
			char *str = mygetenv(argv[i]);
			write(1, str, strlen(str));
			write(1, "\n", 1);
			free(str);
		}
	}

}

int phist(history *hist){
	for(int i=0;i<10;i++){
		if(hist[i].argv==NULL)break;
		for(int j=0; hist[i].argv[j]!=NULL; j++){
			write(1, hist[i].argv[j], strlen(hist[i].argv[j]));
			write(1, " ", 1);
		}
		char out[6];
		int num = sprintf(out, "%d\n", (int)hist[i].exit_status);
		write(1, out, num);
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
                        printf("No read permission on the source file\n");
                        close(fd1);
                        close(fd2);
                        return -1;
                }

                write_count = write(fd2, buff, read_count);
                if(write_count==-1)
                {
                        printf("No write permission  on the destiation file\n");
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

int is_dir(char *path){
	struct stat path_stat;
        stat(path, &path_stat);
	return S_ISDIR(path_stat.st_mode);
}
