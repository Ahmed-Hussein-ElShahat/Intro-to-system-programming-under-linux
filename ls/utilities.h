#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <math.h>

struct inode{
        char *fname;
        struct stat statinfo;
};
struct formatParam{
                nlink_t links;
                off_t size;
		int uidlen;
		int gidlen;
		int namelen;
		int inode_no_len;
};

struct stat getstat(char *fname);
void show_stat_info(struct stat info, struct formatParam bestparam);
void filetype(int mode, char *str);
int alphacomapre(const void *p1, const void *p2);
int modificationtimecompare(const void *p1, const void *p2);
int accesstimecompare(const void *p1, const void *p2);
int changetimecompare(const void *p1, const void *p2);
int mylog(long long x);

