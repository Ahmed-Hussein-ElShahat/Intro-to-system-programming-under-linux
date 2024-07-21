#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char **argv)
{        
        char buff[100];

        if(argc != 3)
        {
                printf("Error: only three arguments are permitted\n");
                        return -1;
        }
        int fd1 = open(argv[1], O_RDONLY);
        int fd2 = creat(argv[2], 0644);

        if(fd1==-1)
        {
                printf("Error: couldn't open source file\n");
                return -1;
        }
	else if(fd2 == -1)
        {

                printf("Error: couldn't create destination file\n");
                return -1;
        }

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
	
	close(fd1);
	close(fd2);

	if(remove(argv[1]) == -1)
	{
		printf("Error: couldn't remove source file\n");
		return -1;
	}
        return 0;

}

