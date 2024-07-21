#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("argc = %d\n", argc);
	if (argc != 3)
	{
		printf("-1\n");
		return -1;
	}

	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_WRONLY|O_CREAT, 0644);

	if(fd1 == -1 || fd2==-1)
	{
	printf("-1");
	return -1;
	}

	char *buff;

	int count;
        do{
                count = read(fd1, buff, 100);
                if(count == -1)
                {
                        printf("Error: couldn't read\n");
                        return -1;
                }
                int char_ret = write(fd2, buff, count);
                if(char_ret == -1)
                {
                        printf("Error: couldn't write\n");
                        return -1;
                }

        }
        while(count);

	return 0;
}
