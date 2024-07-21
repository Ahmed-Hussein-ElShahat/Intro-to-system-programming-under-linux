#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define argSize 1000
#define arraySize 1000

int strcmp_till_char(const char *str1,const char *str2, char delim);
extern char **environ;

int main()
{
	printf("Hello to the new bash\n");
	char *argv[argSize]={NULL};
	char **envp=environ;
	char *token[argSize]={NULL};
	char *lclvariables[argSize]={NULL};
	int i,token_size;
	while(1)
	{
		printf("ekteb ely t7bh> ");

		char buff[arraySize];
		fgets(buff, arraySize, stdin);
		int len = strlen(buff);
		buff[len - 1] = 0;
		if(strlen(buff)==0)continue;

		int ret_id = fork();

		if(ret_id<0)
		{
			printf("failed to fork\n");
			return -1;
		}
		else if(ret_id==0)
		{
			 char *rest=buff;
                        for(i=0;token[i]=strtok_r(rest, " ",&rest);i++);
                        token[i]=0;
                        token_size=i;

                        if(strchr(token[0], '='))
                        {
                                return 3;
                        }
                        else if(!strcmp(token[0],"export"))
                        {
                        	printf("token[0] = %s\n",token[0]);
				return 4;
                        }
                        for(i=0; token[i]!=NULL; i++)
                        {
                                if(!strcmp(token[i],">"))
                                {
                                        printf("Output redirection\n");
                                        int fd = open(token[++i], O_WRONLY|O_CREAT, 0644);
                                        dup2(fd,1);
                                        close(fd);
                                }
                                else if(!strcmp(token[i],"2>"))
                                {
                                        printf("Error redirection\n");
                                        int fd = open(token[++i], O_WRONLY|O_CREAT, 0644);
                                        dup2(fd,2);
                                        close(fd);
                                }
                                else if(!strcmp(token[i],"<"))
                                {
                                        printf("Input redirection\n");
                                        int fd = open(token[++i], O_RDONLY);
                                        if(fd==-1)
                                        {
                                                printf("There is no such file or directory");
                                        }
                                        else
                                        {
                                                dup2(fd,0);
                                                close(fd);
                                        }
                                }
                                else
                                {
                                        argv[i] = token[i];
                                }
                        }
                        execve(buff, argv, envp);
                        printf("Exec failed\n");
                        return 0;


		}
		else if (ret_id>0)
		{
			int status;
			int i;
			wait(&status);
			if(WEXITSTATUS(status)==3 || WEXITSTATUS(status)==4)
			{
				char *rest=buff;
				for(i=0;token[i]=strtok_r(rest, " ",&rest);i++);
				token[i]=0;
				token_size=i;

				if(WEXITSTATUS(status)==3)
				{
					for(int args=0;token[args]!=NULL;args++)
					{
						int found=0;
						for(i=0; lclvariables[i]!=NULL && i!=argSize; i++)
						{
							if(strcmp_till_char(token[args], lclvariables[i], '='))
							{
								lclvariables[i] = strdup(token[args]);
								found=1;
								printf("The local variable is found so it became lclvariable[%d]:   %s\n",i, lclvariables[i]);
								break;
							}
						}
						if(!found)
						{
							lclvariables[i]=strdup(token[args]);
							lclvariables[i+1]=0;
							printf("The local variable variable[%d] = %s \n", i, lclvariables[i]);
						}

						for(i=0;envp[i]!=NULL;i++)
						{
							if(strcmp_till_char(token[args], envp[i], '='))
							{
								envp[i] = strdup(token[args]);
								printf("The envp variable is found so it  became envp[%d] = %s\n", i, envp[i]);
								break;
							}
						}

					}
				}
				else if(WEXITSTATUS(status)==4)
				{
					for(int args=1;token[args]!=NULL;args++)
					{
						int found=0;
						for(i=0;envp[i]!=NULL;i++)
						{
							if(strcmp_till_char(token[args], envp[i], '='))
							{
								envp[i] = strdup(token[args]);
								found = 1;
								printf("The envp variable is found so it  became envp[%d] = %s\n", i, envp[i]);
								break;
							}
						}
						if(!found)
						{
							envp[i] = strdup(token[args]);
							envp[i+1] = 0;
							printf("The environment variable envp[%d] = %s\n", i, envp[i]);
						}

						found=0;
						for(i=0; lclvariables[i]!=NULL && i!=argSize; i++)
						{
							if(strcmp_till_char(token[args], lclvariables[i], '='))
							{
								lclvariables[i] = strdup(token[args]);
								found=1;
								printf("The local variable is found so it became lclvariable[%d]:   %s\n",i, lclvariables[i]);
								break;
							}
						}
						if(!found)
						{
							lclvariables[i]=strdup(token[args]);
							lclvariables[i+1]=0;
							printf("The local variable variable[%d] = %s \n", i, lclvariables[i]);
						}

					}
				}
			}
		}
	}

	return 0;
}

int strcmp_till_char(const char *str1,const char *str2, char delim)
{
	for(int i=0; str1[i]!=delim; i++)
		if( str1[i] != str2[i] ) return 0;

	return 1;
}
