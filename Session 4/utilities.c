#include "utilities.h"

int read_write_files(int, int);

int echo(int argc, char *argv[])
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

int cp(int argc, char *argv[])
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

int mv(int argc, char *argv[])
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

int pwd(int argc)
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
	printf("%s\n",buff);
	return 0;
}

int read_write_files(int fd1, int fd2)
{
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
