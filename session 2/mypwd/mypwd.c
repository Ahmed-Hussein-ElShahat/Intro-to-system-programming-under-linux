#include <stdio.h>
#include <unistd.h>

int main()
{
	char buff[200];
	char *ret_ptr;
	ret_ptr = getcwd(buff, 200);
	printf("%s\n", buff);	
	return 0;
}
