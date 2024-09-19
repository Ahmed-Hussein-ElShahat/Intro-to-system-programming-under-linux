#include <unistd.h>
#include <ctype.h>
#include "utilities.h"
#include <sys/param.h>
#include <sys/ioctl.h>
#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')

extern int errno;
extern char *fileColor;
int files_in_single_line;
char lformat=0,
     all=0,
     tsort=0,
     access_sort=0,
     lchange_sort=0,
     inode_no=0,
     dir_ord=0,
     own_dir=0,
     single_line=0;

struct formatParam bestparam = {0, 0, 0, 0, 0};

void changeformatparams(struct stat info);
void lsformat(struct inode, struct formatParam bestparam, char fileflag);
void do_ls(char * dir);
void lssort(struct inode *statArr, int arrlen);
	static void             /* Print "usage" message and exit */
usageError(char *progName, char *msg, int opt)
{
	if (msg != NULL && opt != 0)
		fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
	fprintf(stderr, "Usage: %s [-p arg] [-x]\n", progName);
	exit(EXIT_FAILURE);
}
char *getpathname(char *fname, char *dir){
	char *pathname = malloc( (strlen(dir) + strlen(fname) + 2) * sizeof(char));
	if (pathname == NULL && errno!=0){
		printf("Couldn't allocate memory");
		exit(errno);
	}

	strcpy(pathname, dir);
	if(pathname[strlen(dir) - 1] != '/'){
		pathname[strlen(dir)] = '/';
		pathname[strlen(dir)+1] = 0;
	}
	return strcat(pathname, fname);
}

char *filename(char *path, char fileflag){
	//filelfag points to the argument that was given by user so we should use 0 if we want to get the actual filename (as if the user entered a file)
	char *fname;
	if(!own_dir && !fileflag){
		fname = strrchr(path, '/');
		if (fname == NULL) fname = path;
		else if(strlen(fname) != 1) fname++;
	}
	else fname = path;
	return fname;
}
int isfile(char *fname){
	struct stat filestat= getstat(fname);
	return  (!S_ISDIR(filestat.st_mode) && !S_ISLNK(filestat.st_mode)) || (S_ISLNK(filestat.st_mode) && lformat);
}
int cmpstringp(const void *p1, const void *p2)
{
	/* The actual arguments to this function are "pointers to
	   pointers to char", but strcmp(3) arguments are "pointers
	   to char", hence the following cast plus dereference */
	char p1_fileflag = isfile( *(char **) p1),
	     p2_fileflag = isfile( *(char **) p2);

	if(p1_fileflag && !p2_fileflag) return -1;
	else if(!p1_fileflag && p2_fileflag) return 1;
	else return strcmp(*(const char **) p1, *(const char **) p2);
}

struct inode getinfo(char * fname, char *dir)
{
	char *pathname = getpathname(fname, dir);
	struct stat info = getstat(pathname);

	struct inode inodeInfo = {
		pathname,
		info
	};
	return inodeInfo;
}

int adjust_filename_len(int output_len){
	//get terminal width
       	struct winsize sz;
        ioctl( 0, TIOCGWINSZ, &sz );

	if(output_len < 0 || output_len-2 > sz.ws_col) return sz.ws_col/(bestparam.namelen + bestparam.inode_no_len + 3);
	else return bestparam.namelen=0;
}
//function to determine whether to go to a newline
void isnewline(int n){
	if(files_in_single_line != 0 && (n+1)%files_in_single_line==0 ){
		printf("\n");
	}
	else printf("  ");
}

int main(int argc, char* argv[]){
	//the allowed options -latucifd1
	int opt;
	while ((opt = getopt(argc, argv, ":latucifd1")) != -1) {
		if (opt == '?' || opt == ':')
			printf("; optopt =%3d (%c)\n", optopt, printable(optopt));

		switch (opt) {
			case 'l': lformat++;        	break;
			case 'a': all++;            	break;
			case 't': tsort++;          	break;
			case 'u': access_sort++; 	break;
			case 'c': lchange_sort++;	break;
			case 'i': inode_no++;     	break;
			case 'f': dir_ord++;        	break;
			case 'd': own_dir++;        	break;
			case '1': single_line++;   	break;
			case ':': usageError(argv[0], "Missing argument\n", optopt);
			case '?': usageError(argv[0], "Unrecognized option\n", optopt);
			default:  printf("Unexpected case in switch()\n"); return -1;
		}
	}

	if (optind == argc && !own_dir)
		do_ls(".");
	else{
		if(optind == argc) argv[argc++] = ".";
		struct inode *statArr = NULL;
		unsigned int arrlen = 0;
		int i=optind-1;
		qsort(&argv[i +1], argc - optind, sizeof(char *), cmpstringp);

		int no_of_files=0;
		while(++i<argc){
			if(isfile(argv[i])) no_of_files++;
			else break;
		}

		i = optind-1;
		int output_len=0;
		while(++i < argc){
			struct stat filestat= getstat(argv[i]);
			if(own_dir || ( !S_ISDIR(filestat.st_mode) && !S_ISLNK(filestat.st_mode)) || (S_ISLNK(filestat.st_mode) && lformat))
			{
				char *fname = strdup(argv[i]);
				struct inode info = {
					fname,
					getstat(fname)
				};
				void *ptr = realloc(statArr, (arrlen + 1)*sizeof(struct inode));
				if (ptr == NULL && errno!=0){
					printf("Couldn't allocate memory");
					exit(errno);
				}

				statArr = ptr;
				statArr[arrlen] = info;
				if(lformat) changeformatparams(statArr[arrlen].statinfo);
				else{
					if(inode_no) {
						output_len += mylog(statArr[arrlen].statinfo.st_ino)+2;
						bestparam.inode_no_len = bestparam.inode_no_len >= mylog(statArr[arrlen].statinfo.st_ino)+1 ? bestparam.inode_no_len : mylog(statArr[arrlen].statinfo.st_ino)+1;
					}
					int fname_len = strlen( filename(argv[i], 0)) ;
					output_len += fname_len+2;
					bestparam.namelen = bestparam.namelen >= fname_len ? bestparam.namelen : fname_len;
				}
				arrlen++;
			}
			else{ 
				//display the directories
				if(optind != argc-1)printf("Directory listing of %s:\n", argv[i]);
				do_ls(argv[i]);
			}

			//display the files
			if(!own_dir && i-optind+1 == no_of_files  && !S_ISDIR(filestat.st_mode)){
				lssort(statArr, arrlen);
				files_in_single_line = adjust_filename_len(output_len);
				for(int i=0; i<arrlen; i++){
					lsformat(statArr[i], bestparam, 1);
					if(!lformat && !single_line && i != arrlen-1) isnewline(i);
					free(statArr[i].fname);
				}
				if(!lformat && !single_line)printf("\n");
				free(statArr);
				if(arrlen != argc-optind)printf("\n");
			}

			if(i + 1 != argc && !own_dir && S_ISDIR(filestat.st_mode))
				printf("\n");
		}
		//dislay if own_dir flag is set
		if(own_dir){
			lssort(statArr, arrlen);
			files_in_single_line = adjust_filename_len(output_len);
			for(int i=0; i<arrlen; i++){
				lsformat(statArr[i], bestparam, 1);
				if(!lformat && !single_line && i != arrlen-1) isnewline(i);
				free(statArr[i].fname);
			}
			if(!lformat && !single_line)printf("\n");
			free(statArr);	
		}
	}
	return 0;
}


void do_ls(char * dir)
{
	bestparam.namelen = 0;
	struct dirent * entry;
	DIR * dp = opendir(dir);
	if (dp == NULL){
		fprintf(stderr, "Cannot open directory:%s\n",dir);
		return;
	}

	errno = 0;

	struct inode *statArr = NULL;
	unsigned int arrlen = 0;
	int output_len=0;
	while((entry = readdir(dp)) != NULL)
	{
		if(entry == NULL && errno != 0){
			perror("readdir failed");
			exit(errno);
		}

		if(!all && !dir_ord && entry->d_name[0] == '.') continue;

		errno = 0;
		void *ptr = realloc(statArr, (arrlen + 1)*sizeof(struct inode));
		if (ptr == NULL){
			printf("Couldn't allocate memory");
			exit(errno);
		}

		statArr = (struct inode *)ptr;
		statArr[arrlen] = getinfo(entry->d_name, dir);
		if(lformat) changeformatparams(statArr[arrlen].statinfo);
		else{
			if(inode_no) {
				output_len += mylog(statArr[arrlen].statinfo.st_ino)+2;
				bestparam.inode_no_len = bestparam.inode_no_len >= mylog(statArr[arrlen].statinfo.st_ino)+1 ? bestparam.inode_no_len : mylog(statArr[arrlen].statinfo.st_ino)+1;
			}
			int fname_len = strlen( filename(statArr[arrlen].fname, 0));
			output_len += fname_len + 2;
			bestparam.namelen = bestparam.namelen >= fname_len ? bestparam.namelen : fname_len;
		}
		arrlen++;
	}

	lssort(statArr, arrlen);
	files_in_single_line = adjust_filename_len(output_len);	
	for(int i=0; i<arrlen; i++){
		lsformat(statArr[i], bestparam, 0);
		if(!lformat && !single_line && i != arrlen-1) isnewline(i);
		free(statArr[i].fname);
	}
	if(!lformat && !single_line)printf("\n");
	free(statArr);
	closedir(dp);
}


void lsformat(struct inode info, struct formatParam bestparam, char fileflag)
{
	if(inode_no) printf("%*ld ", bestparam.inode_no_len, info.statinfo.st_ino);

	//deternmine the name of the file
	char *fname = filename(info.fname, fileflag);

	if(lformat){
		show_stat_info(info.statinfo, bestparam);
		if(S_ISLNK(info.statinfo.st_mode)){
			struct stat symbinfo;
			int rv = stat(info.fname, &symbinfo);

			errno=0;
			char *symname = malloc(info.statinfo.st_size+1);
			if(symname == NULL && errno != 0){
				printf("Couldn't allocate memory");
				exit(errno);
			}
			int n = readlink(info.fname, symname, info.statinfo.st_size);
			symname[info.statinfo.st_size] = 0;
			if (rv == -1){
				fileColor = "\033[1;31;40m";
			}
			printf("%s%s\033[1;0m",fileColor, fname);
			if(rv != -1){
				filetype(symbinfo.st_mode, NULL);
				char *extension = strrchr(symname, '.');
				if(extension != NULL && strcmp(extension, ".swp") == 0) fileColor = "\033[1;30m";
			}
			printf(" -> %s%s\033[1;0m", fileColor, symname);
			free(symname);
		}
		else {
			char *extension = strrchr(fname, '.');
			if(extension != NULL && strcmp(extension, ".swp") == 0) fileColor = "\033[1;30m";
			printf("%s%s\033[1;0m",fileColor, fname);
		}
		printf("\n");
	}
	else{
		if(S_ISLNK(info.statinfo.st_mode)){
			struct stat symbinfo;
			int rv = stat(info.fname, &symbinfo);
			if (rv == -1){
				fileColor = "\033[1;31;40m";
			}
			else{
				fileColor = "\033[1;36m";
			}
		}
		else {
			filetype(info.statinfo.st_mode, NULL);
			char *extension = strrchr(fname, '.');
			if(extension != NULL && strcmp(extension, ".swp") == 0) fileColor = "\033[1;30m";
		}

		if(!single_line) {
			printf("%s%-s\033[1;0m", fileColor, fname);
			if(files_in_single_line != 0)printf("%*s", bestparam.namelen-(int)strlen(fname), "");
		}
		else printf("%s%s\033[1;0m\n", fileColor, fname);
	}

}

void lssort(struct inode *statArr, int arrlen){
	if(tsort) qsort(statArr, arrlen, sizeof(struct inode), modificationtimecompare);
	if(access_sort)  qsort(statArr, arrlen, sizeof(struct inode), accesstimecompare);
	if(lchange_sort) qsort(statArr, arrlen, sizeof(struct inode), changetimecompare);
	if(!tsort && !access_sort && !lchange_sort && !dir_ord) qsort(statArr, arrlen, sizeof(struct inode), alphacomapre);
}
void changeformatparams(struct stat info){
	bestparam.inode_no_len = bestparam.inode_no_len >= mylog(info.st_ino) + 1 ? bestparam.inode_no_len : mylog(info.st_ino)+1;
	bestparam.links = bestparam.links > info.st_nlink ? bestparam.links : info.st_nlink;
	bestparam.size =  bestparam.size > info.st_size ? bestparam.size: info.st_size;

	errno = 0;
	struct passwd * pwd = getpwuid(info.st_uid);
	if (pwd == NULL){
		if (errno == 0)
			bestparam.uidlen = bestparam.uidlen > mylog(info.st_uid) + 1 ? bestparam.uidlen : mylog(info.st_uid);

		else
			perror("getpwuid failed\n");
	}
	else
		bestparam.uidlen = bestparam.uidlen > strlen(pwd->pw_name) ? bestparam.uidlen : strlen(pwd->pw_name);

	struct group * grp = getgrgid(info.st_gid);
	errno = 0;
	if (grp == NULL){
		if (errno == 0)
			bestparam.uidlen = bestparam.gidlen > mylog(info.st_gid) + 1 ? bestparam.gidlen : mylog(info.st_gid);
		else
			perror("getgrgid failed\n");
	}else
		bestparam.gidlen = bestparam.gidlen > strlen(grp->gr_name) ? bestparam.gidlen : strlen(grp->gr_name);



}
