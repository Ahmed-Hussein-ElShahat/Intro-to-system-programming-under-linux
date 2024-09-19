#include "utilities.h"

extern int errno;
char *fileColor;
void parseMode(int mode, char *str);
int mylog(long long x){
	int n=0;
	while(x/10 != 0){
		n++;
		x/=10;
	}
	return n;
}
int timespec_diff(struct timespec x, struct timespec y){
	if(x.tv_sec - y.tv_sec != 0) return y.tv_sec - x.tv_sec;
	return y.tv_nsec - x.tv_nsec;
}

struct stat getstat(char * fname){
	struct stat info;
	int rv = lstat(fname, &info);
	if (rv == -1){
		perror("stat failed");
		exit(1);
	}
	return info;
}
void show_stat_info(struct stat info, struct formatParam bestparam){
	long secs = info.st_mtime;
	char *time = ctime(&secs);
	time = &time[4];
	time[strlen(time)-9] = 0;

	char str[10];
	parseMode(info.st_mode, str);
	printf("%s ", str);
	printf("%*ld ", mylog(bestparam.links) + 1, info.st_nlink);

	errno = 0;
	struct passwd * pwd = getpwuid(info.st_uid);
	if (pwd == NULL){
		if (errno == 0)
			printf("%-*d ", bestparam.uidlen, info.st_uid);         
		else 
			perror("getpwuid failed\n");
	}
	else 
		printf("%-*s ", bestparam.uidlen, pwd->pw_name);

	struct group * grp = getgrgid(info.st_gid);
	errno = 0;
	if (grp == NULL){
		if (errno == 0)
			printf("%-*d ", bestparam.gidlen, info.st_gid);
		else
			perror("getgrgid failed\n");
	}else
		printf("%-*s ", bestparam.gidlen, grp->gr_name);

	printf("%*ld ", mylog(bestparam.size) + 1, info.st_size);
	printf("%s ", time);
}

int alphacomapre(const void *p1, const void *p2){
	return strcmp( ((struct inode*)p1)->fname, ((struct inode*)p2)->fname);
}
int modificationtimecompare(const void *p1, const void *p2){
	return timespec_diff(((struct inode*)p1)->statinfo.st_mtim, ((struct inode*)p2)->statinfo.st_mtim);
}
int accesstimecompare(const void *p1, const void *p2){
	return timespec_diff(((struct inode*)p1)->statinfo.st_atim, ((struct inode*)p2)->statinfo.st_atim);
}
int changetimecompare(const void *p1, const void *p2){
	return timespec_diff(((struct inode*)p1)->statinfo.st_ctim, ((struct inode*)p2)->statinfo.st_ctim);
}

void parseMode(int mode, char *str){
	strcpy(str, "----------");
	filetype(mode, str);

	str = &str[1];	
	//owner  permissions
	if((mode & 0000400) == 0000400) str[0] = 'r';
	if((mode & 0000200) == 0000200) str[1] = 'w';
	if((mode & 0000100) == 0000100) str[2] = 'x';
	//group permissions
	if((mode & 0000040) == 0000040) str[3] = 'r';
	if((mode & 0000020) == 0000020) str[4] = 'w';
	if((mode & 0000010) == 0000010) str[5] = 'x';
	//others  permissions
	if((mode & 0000004) == 0000004) str[6] = 'r';
	if((mode & 0000002) == 0000002) str[7] = 'w';
	if((mode & 0000001) == 0000001) str[8] = 'x';
	//special  permissions
	if((mode & 0004000) == 0004000){  //set user id
		if(str[2] != 'x') str[2] = 's';	
		else str[2] = 'S';
	}
	if((mode & 0002000) == 0002000){ //set group id
		if(str[5] != 'x') str[5] = 's';
		else str[5] = 'S';
	}
	if((mode & 0001000) == 0001000){ //sticky bit
		if(str[8] != 'x') str[8] = 't';
		else str[8] = 'T';
	}

}

void filetype(int mode, char *str){
	char c = '-';
	if ((mode &  0170000) == 0010000){      //FIFO
		c ='l';
		fileColor = "\033[33;40m";
	}
	else if ((mode &  0170000) == 0020000){ //character device
		c ='c';
		fileColor = "\033[1;33;40m";
	}
	else if ((mode &  0170000) == 0040000){ //directory
		c ='d';
		fileColor = "\033[1;34m";
	}
	else if ((mode &  0170000) == 0060000){ //block device
		c ='b';
		fileColor = "\033[1;33;40m";
	}
	else if ((mode &  0170000) == 0100000){ //regular file
		if((mode & 0000100) == 0000100 || (mode & 0000010) == 0000010 || (mode & 0000001) == 0000001) fileColor = "\033[1;32m";
		else fileColor = "\033[1;0m";
	}
	else if ((mode &  0170000) == 0120000){ //symbolic link
		c ='l';
		fileColor = "\033[1;36m";
	}
	else if ((mode &  0170000) == 0140000){ //socket
		c ='l';
		fileColor = "\033[1;35m";
	}
	else{
		printf("Unknwon mode\n");
		exit(-1);
	}

	if((mode & 0004000) == 0004000){  //set user id
		fileColor = "\033[37;41m";
	}
	if((mode & 0002000) == 0002000){ //set group id
		fileColor = "\033[30;43m";
	}
	if((mode & 0001000) == 0001000){ //sticky bit
		fileColor = "\033[30;42m";
	}

	if(str != NULL) str[0] = c;
}
