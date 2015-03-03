#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>

const char *prgname = NULL;

void checkFile(const char *file);
void noparam(void);
static void correctusage(void);
void ls(const char *file);
void modifytime(const char *file, struct stat time);
void checkpermissions(mode_t st_mode, char *mode);


/* void do_dir(const char * dir_name, const char * const * parms); */			/* Rene - erfolgreich? */
/*void do_file(const char * dir_name, const char * const * parms); */		/* Rene - erfolgreich? */



int main(int argc, char* argv[])
{
	prgname = argv[0];
	/* const char * const *paramlist = (const char * const *)&argv[1];*/

	if (argc == 1)
	{
		noparam();
	}
	else if (argc == 2)
	{
		if (argv[2] == 0)/* (argv[2] != '-print')*/
			correctusage();
		else /* es werden alle Einträge des aktuellen Directories ausgegeben*/

		/* fprintf(stderr, "zu wenig Argumente..\n"); */
		return EXIT_FAILURE;
	}

	printf("Parameter 1: %s\n", argv[1]);
	
	printf("Anzahl der Argumente: %d\n", argc);
	printf("committest");

	checkFile(argv[1]);

	return EXIT_SUCCESS;
}

void checkFile(const char *file)
{

	struct stat mystat;
	/*char *error; */

	if(lstat(file,&mystat) == -1)
	{
	 fprintf(stderr, "mystat() failed..\n");
	fprintf(stderr, "Fehler: %s\n", strerror(errno));	
/*error = strerror(errno);
	perror(error);*/	
	
	exit(EXIT_FAILURE);
	 }

	
	lstat(file, &mystat);

	
	 
	if (S_ISREG(mystat.st_mode)) printf("File: %s\n", file );
	else if (S_ISDIR(mystat.st_mode)) printf("Directory: %s \n", file);

}

void ls(const char *file)
{

	struct stat lsstat;
	fprintf(stdout, "%ld\t%ld\t%ld\n", (long)lsstat.st_ino, (long)lsstat.st_blksize, (long)lsstat.st_nlink);
	fprintf(stdout, "%ld\t%s\t%s\n", (long)lsstat.st_gid, modifytime(file, lsstat.st_mtime), file);


}

void noparam(void)
{
	/* if no parameter is given, the current directory with subdirectory is printeed */



}

void modifytime(const char *file, struct stat time)
{
	char timeformated[150];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);

	if (tmp == NULL) {
	               perror("localtime");
	               exit(EXIT_FAILURE);
	           }

	           if (strftime(timeformated, sizeof(timeformated), file, tmp) == 0) {
	               fprintf(stderr, "Function strftime returned 0");
	               exit(EXIT_FAILURE);
	           }

	           printf("%s\n", timeformated);
	           exit(EXIT_SUCCESS);
}

static void correctusage(void)
{
	fprintf("Correct use of: %s\n"
			"-user: searches entries of certain users\n"
			"-name: searches entries with a given name\n"
			"-type (bcdpfls): searches for certain type"
			"-print: prints name of entry"
			"-ls: prints certain information of file"
			"-nouser: searches entries with nouser"
			"-path: searches entries with given path"
			"For further information do find --help"
			, prgname);

}

void checkpermissions(mode_t st_mode, char *mode)
{

	/* UND-Verknüfung */
}

