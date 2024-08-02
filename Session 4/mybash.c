#include "utilities.h"
#include <sys/wait.h>
#include <sys/stat.h>

#define argsize 200


int filename(char *source, char *dest);
int main(){
	printf("Hello to the new bash\n");

	while(1){
		printf("ektb ely t7bo> ");

		char buff[argsize];
		fgets(buff, argsize, stdin);
		int len = strlen(buff);
		if(len==1)continue;
		buff[len-1] = 0;
		
		if(!strcmp(buff, "exit")){
			printf("Goodbye\n");
			return 0;
		}
		
		char *argv[100]={NULL};
		int argc=0;
		char *rest= buff;
		while(argv[argc] = strtok_r(rest, " ", &rest))argc++;
			
		if(!strcmp(argv[0], "echo")){
			echo(argc, argv);
		}
		else if(!strcmp(argv[0], "cp")){
			filename(argv[argc-2], argv[argc-1]);
			cp(argc, argv);
		}
		else if(!strcmp(argv[0], "mv")){
			filename(argv[argc-2], argv[argc-1]);
			mv(argc, argv);
		}
		else if(!strcmp(argv[0], "pwd")){
			pwd(argc);
		}
		else if(!strcmp(argv[0], "help")){
			printf("echo\treprints the geiven arguments\ncp\tcopies the source file to the destination\nmv\tmoves the source file to the destination\npwd\tprints the curent working directory\nexit\tleaves the bash\n");
		}
		else{	
			printf("Invalid command name\n");
		}
	}
	return 0;
}

int filename(char *source, char *dest){
	int end=0;
	while(dest[end]!=0)end++;
	
	struct stat path_stat;
	stat(dest, &path_stat);
	if(S_ISDIR(path_stat.st_mode) && dest[end-1]!='/')
	{
		dest[end]='/';
		dest[++end]=0;
	}
	
	if(dest[end-1]=='/'){
		char *first_char;
		char *name;
		if(first_char = strrchr(source, '/')){
			name = strdup(++first_char);
		}
		else{
		       	name = strdup(source);
		}
		
		dest = strcat(dest, name);
		return 1;
	}
	return 0;
}
