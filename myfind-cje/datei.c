/**
 * @file myfind.c
 * Betriebssysteme Myfind File
 * Beispiel 1
 *
 * @author Claudia Baierl ic14b003 <claudia.baierl@technikum-wien.at>
 * @author Rene Elsner ic14b016 <rene.elsner@technikum-wien.at>
 * @author Juliane Winkler ic14b059 <juliane.winkler@technikum-wien-at>
 * @date 2015/03/12
 *
 * @version 1
 *
 * @todo Finish programming.
 * @todo Review it for missing error checks.
 * @todo Review it and check the source against the rules at
 *       https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/c-rules.html
 * @todo Double check for "forbidden" functions (Minuspunkte!): gets(), scanf(), fscanf(), atoi(), atol()
 * @todo Unify function names, parameter names, etc.
 * @todo Einrückungen beachten.
 * @todo Run test scripts.
 * @todo Finalize Doxygen.
 *
 * @beachte Kommentar am Kopf des Moduls: Name des Moduls, Name und Matrikelnummer des Autors, Zweck des Moduls, Nummer des Beispiels (nur im Modul des Hauptprogramms)
 * @beachte Modulkopf im Makefile inkludieren
 * @beachte Im fehlerfreien Fall muss das Programm mit dem Exit-Code EXIT_SUCCESS (==0) terminieren, im Fehlerfall mit EXIT_FAILURE.
 * @beachte Funktionen, die keine Werte zurückliefern, sind als VOID zu deklarieren.
 * @beachte Fehlermeldungen sind auf die Standardfehlerausgabe) (stderr) zu schreiben und haben den Programmnamen (argv[0]) zu enthalten.
 * @beachte Konstantendefinitionen sind groß (z.B. MAXBUFFER) zu schreiben.
 * @beachte Namen von Variablen sind klein (current_buffer) zu schreiben, kein CamelCase.
 * @beachte Wichtige Konstanten sollen symbolische Namen erhalten (Makrodefinitionen!)
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>
#include <fnmatch.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <unistd.h>

/** ---------------------------------------------- typedefs--
 *
 */

typedef enum {
	NAME,
	USER,
	TYPE,
	PRINT,
	LS,
	NOUSER,
	PATH,
	ARGUMENT
} Parameter;


/*
 * --------------------------------------------------------------- globals --
 */
const char *prgname = NULL;
static int params_number = 0;

/*
 * ------------------------------------------------------------- prototypes--
 */

void check_name(const char *file);
void noparam(void);
static void correctusage(void);
void ls(const char *file);
char *modifytime(time_t ftime);
char *checkpermissions(mode_t st_mode);
int check_type(const char * parms, struct stat buffer);
int check_path(const char * parms, const char * dir_name);
int check_no_user(struct stat statbuf);
int check_user(const char * user, struct stat statbuf);
long string_change(const char * value);
void do_dir(const char * dir_name, const char * const * parms);
void do_file(const char * dir_name, const char * const * parms);
void check_file_parameter(char *parms[], int params_number, int *param_array[]);


/**
 *
 * \brief
 *
 * @todo dir_name "." wenn kein Pfad eingegeben, ansonten Pfad aus argv[1]
 *
 * \param argc the number of arguments
 * \param argv the arguments itselves (including the program name in argv[0])
 *
 * \return always "success"
 * \retval 0 always
 *
 */
int main(int argc, char* argv[])
{
	prgname = argv[0];
	params_number = argc-1;
	const char * dir_name = "."; /* current directory is used when no directory is entered */
	int param_array[params_number];
	int i = 0;
	int j = 0;

	check_file_parameter(argv[], argc, &param_array);


  	

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

	for (i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "help") == 0)
		{
			correctusage();
			exit(1);
		}
	}

	check_name(argv[1]);



	return EXIT_SUCCESS;

}

/*
 * ------------------------------------------------------------- functions --
 */
 
/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param *parms[]
 * \param params_number
 * \param *param_array
 *
 */ 
 
void check_file_parameter(char *parms[], int params_number, int *param_array[])
{
	int i = 1;

	if(strcmp(parms[i][0], "-") == 0)
	{
		for(i = 1; i <= params_number; i++)
		{

			if(strcmp(parms[i], "-name") == 0) *param_array[i-1] = 0;
			else if (strcmp(parms[i], "-user") == 0) *param_array[i-1] = 1;
			else if(strcmp(parms[i], "-type") == 0) *param_array[i-1] = 2;
			else if(strcmp(parms[i], "-print") == 0) *param_array[i-1] = 3;
			else if(strcmp(parms[i], "-ls") == 0) *param_array[i-1] = 4;
			else if(strcmp(parms[i], "-nouser") == 0) *param_array[i-1] = 5;
			else if(strcmp(parms[i], "-path") == 0) *param_array[i-1] = 6;
			else
			{
				correctusage();
				exit(1);
			}
		}
	}
	else *param_array[i-1] = 7;

}

/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param *parms[]
 * \param params_number
 * \param *param_array
 *
 */

void do_dir(const char * dir_name, const char * const * parms)
{
	 const int dir_length = strlen(dir_name);
	 const struct dirent *dirent;
	 DIR *dir = opendir(dir_name);
	 char *comp_name = NULL;
	 int comp_size = 0;

	 if (dir == NULL)
	 {
	     fprintf(stderr, "%s: %s: %s\n", prgname, dir_name, strerror(errno));
	     return;
	 }

	 while((dirent = readdir(dir)) != NULL)
	 {
	     int new_length;
	     if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
	     {
	         continue;
	     }
	     new_length = dir_length + 1 + strlen(dirent->d_name) + 1; /* including the terminating '\0' */
	     if (new_length > comp_size) {
	        comp_size = new_length;
	        comp_name = realloc(comp_name, comp_size);
	        if (comp_name == NULL) {
	           fprintf(stderr, "Out of memory!\n");
	           exit(1);
	           }
	        }
	        sprintf(comp_name, "%s/%s", dir_name, dirent->d_name);
	        do_file(comp_name, parms);
	    }
	    free(comp_name);
	    closedir(dir);
	}


/**
 *
 * \brief
 *
 * @todo do_dir einbauen
 * @todo restliche Funktionen einbauen
 * @todo kann -print überhaupt noch weitere Argumente haben? wenn nicht, dann sollte
 * 		print vielleicht als erstes gecheckt werden und gar nicht erst in die for-Schleife rein
 * 		gegangen werden
 *
 * \param *parms
 * \param dir_name
 * \param param_array
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */
 
void do_file(const char * dir_name, const char * const * parms)
{
	int i = 0;
	struct stat buffer; /* new structure for lstat */
	int not_found = 0;

	if(parms[1][0] == "-") i = 1;
	else i = 2;

	for(i; i <= params_number; i++)
	{
		if(lstat(dir_name, &buffer) == 0) /** lstat: on success, zero is returned */
		{
			if(strcmp(parms[i], "-user") == 0) /* strcmp: on success, zero is returned */
			{
				/* enter USER FUNCTION */
			}
			else if(strcmp(parms[i], "-name") == 0)
			{
				/* enter NAME FUNCTION */
			}
			else if(strcmp(parms[i], "-type") == 0)
			{
				if(check_type(parms[i+1], buffer) == 1) /* Parameter nach path wird überprüft */
				{
					i++; /* spring zum nächsten - */
				}
				else
				{
					not_found++;
				}
			}
			else if(strcmp(parms[i], "-print") != 0)
			{
				not_found++; /* Ändern */
			}
			else if(strcmp(parms[i], "-ls") == 0)
			{
				ls(&dir_name);
			}
			else if(strcmp(parms[i], "-nosuer")== 0)
			{
				/* ENTER NOUSER FUNCTION */
			}
			else if(strcmp(parms[i], "-path") == 0)
			{
				if(check_path(parms[i+1], dir_name) == 1) /* Parameter nach path wird überprüft */
				{
					i++;
				}
				else
				{
					not_found++;
				}
			}
			}
			else
			{
				/* some kind of error handling */
			}
		}

	if (not_found == 0) /* 0: alle Parameter gefunden */
	{
		fprintf("%s\n", dir_name);
	}



	/* find some way to  check if directory and open do_dir */

}

/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param *parms[]
 * \param params_number
 * \param *param_array
 *
 */

void check_name(const char *file)
{

	struct stat mystat;

	/* Fehlerbehandlung und Fehlerausgabe über errno */
	if(lstat(file,&mystat) == -1)
	{
	fprintf(stderr, "mystat() failed..\n");
	fprintf(stderr, "Fehler: %s\n", strerror(errno));	
	
	exit(EXIT_FAILURE);
	}

	
	lstat(file, &mystat);
	 
	if (S_ISREG(mystat.st_mode)) printf("%s\n", file );
	else if (S_ISDIR(mystat.st_mode)) printf("%s \n", file);

}

/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param *file
 *
 */

void ls(const char *file)
{

	/** necessary structs for all information needed in ls */
	struct stat *lsstat;
	struct group *mygroup;
	struct passwd *mypw;

	/** filling structs with the file information and getting owner and group information */
	lstat(file, &lsstat);
	mygroup = getgrgid(&lsstat->st_gid);
	mypw = getpwuid(&lsstat->st_uid);

	/** calling necessary functions and printing all information asked for in ls */
	fprintf(stdout, "%ld\t%ld\t%s\t%ld\n%s\t%s\t%s\t%s\n",
			(long)lsstat->st_ino,
			(long)lsstat->st_blocks/2,
			checkpermissions(&lsstat->st_mode),
			(long)lsstat->st_nlink,
			mypw->pw_name,
			mygroup->gr_name,
			modifytime(&lsstat->st_mtime), file);

	/* symbolic links */
	if (S_ISLNK(lsstat->st_mode))
	{
		char *link;
		int len = 0;

		link = (char*) malloc(sizeof(char));
		if (link == NULL)
			{ printf("Allocation not possible.\n"); exit(1); }

		if ((len = readlink(file, link, sizeof(link)-1)) != -1)
		{
	   		link[len] = '\0';
			printf("%s", link);
		}
		else
		{
			fprintf(stderr, "%s\n", strerror(errno));
		}

		free(link);
	}

}

void noparam(void)
{
	/** if no parameter is given, the current directory with subdirectory is printeed */
	/** INSERT print function */


}

/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param ftime
 *
 */

char *modifytime(time_t ftime)
{
	struct tm *time;
	static char filetime[30];

	time=localtime(&ftime);
	strftime(filetime, sizeof filetime, "%B %d %H:%M ", time);

	    return(filetime);
}

/** no parameter needed for this function */

static void correctusage(void)
{
	printf("Correct use of: %s\n"
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

/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param st_mode
 *
 */

char * checkpermissions(mode_t st_mode)
{
	static char mode[11];
	strcpy (mode, "----------");

	if (st_mode & S_IFREG)
		mode[0] = '-';
		else if (S_ISCHR(st_mode))
			mode[0] = 'c';
		else if (S_ISBLK(st_mode))
			mode[0] = 'b';
		else if (S_ISLNK(st_mode))
			mode[0] = 'l';
		else if (st_mode & S_IFDIR)
			mode[0] = 'd';
		else if (S_ISFIFO(st_mode))
			mode[0] = 'p';
		else if(S_ISSOCK(st_mode))
			mode[0] = 's';

	if (st_mode & S_IRUSR)
		mode[1] = 'r';
	else if (st_mode & S_IWUSR)
		mode[2] = 'w';
	else if (st_mode & S_IXUSR)
		mode[3] = 'x';
	else if (st_mode & S_IRGRP)
		mode[4] = 'r';
	else if (st_mode & S_IWGRP)
		mode[5] = 'w';
	else if (st_mode & S_IXGRP)
		mode[6] = 'x';
	else if (st_mode & S_IROTH)
		mode[7] = 'r';
	else if (st_mode & S_IWOTH)
		mode[8] = 'w';
	else if (st_mode & S_IXOTH)
		mode[9] = 'x';

	return mode;
}

/**
 *
 * \brief Function to check if a valid file type was entered and if there are any files of this type
 *
 * @todo Parameternamen?
 * @todo Fehlermeldung in find(1) checken (Fehler wenn falscher Dateityp, Fehler wenn nichts gefunden)
 * @todo einfachere Umsetzung mit Switch?
 * @todo Umsetzung ohne type_chars und help_chars?
 * @todo möglicherweise überprüfen, ob überhaupt ein valider Datentyp eingegeben wurde, bevor in Funktion gegangen wird?!
 *
 * \param *parms
 * \param stat statbuf
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */

int check_type(const char * parms, struct stat buffer)
{

	char *type_chars = "bcdpfls"; /* bcdpfls represent the first letter of the valid types that can be found with myfind/find */
	char *help_chars = NULL; /* used for strchr */

	help_chars = strchr(type_chars, parms[0]); /* The strchr() function returns a pointer to the first occurrence of
		the character c in the string s. It returns a pointer to the matched character or NULL if the character is not found. */

	if (strlen(parms) == 1 && (help_chars != NULL))
	{
		if (strcmp(parms, "b") == 0 && S_ISBLK(buffer.st_mode))
			return 1;
		else if (strcmp(parms, "c") == 0 && S_ISCHR(buffer.st_mode))
			return 1;
		else if (strcmp(parms, "d") == 0 && S_ISDIR(buffer.st_mode))
			return 1;
		else if (strcmp(parms, "f") == 0 && S_ISREG(buffer.st_mode))
			return 1;
		else if (strcmp(parms, "l") == 0 && S_ISLNK(buffer.st_mode))
			return 1;
		else if (strcmp(parms, "p") == 0 && S_ISFIFO(buffer.st_mode))
			return 1;
		else if (strcmp(parms, "s") == 0 && S_ISSOCK(buffer.st_mode))
			return 1;
		else
			return 0;
	}
	else
	{

		fprintf(stderr, "find: Unknown argument to -type: %s\n", parms);
		exit(EXIT_FAILURE);

	}
}


/**
 *
 * \brief Function to check if a valid path was entered and if the path matches match
 *
 * @todo Ausgabe der Dateien wo? in Funktion, außerhalb der Funktion? vorerst wird nur gecheckt; wenn Rückgabe der Ergebnisse in void umwandeln?
 * @todo Parameternamen?
 * @todo Fehlermeldung in find(1) checken
 * @todo möglicherweise überprüfen, ob überhaupt ein valider path eingegeben wurde, bevor in Funktion gegangen wird?! gibt es überhaupt einen invalid path?
 * @todo fnmatch hat Flags, brauche ich die?
 * @todo Brauche ich hier überhaupt eine Funktion?
 * @todo bei path wird gesamter Pfad ausgegeben; Ausgabe muss noch geändert werden
 *
 * \param *parms
 * \param dir_name
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */

int check_path(const char * parms, const char * dir_name)
{

	if(fnmatch(parms, dir_name, 0) == 0) /* The fnmatch() function checks whether the string argument matches the parms argument. */
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int check_no_user(struct stat statbuf)
{
	if(getpwuid(statbuf.st_uid)==NULL)
		return 1;
	else
		return 0;
}

int check_user(const char * user, struct stat statbuf)
{
	int i = 0;
	int uid = 0;
	int user_length = strlen(user);

	for(i=0; i<= user_length; i++)
	{
		if(!isdigit(user[i]))
			break; /* check nur Zahlen? */
	}
		if(i == user_length)
		{
			if((uid = string_change(user)) > -1)
			{
				if(uid == statbuf.st_uid) return 1;
			}
			return 0;
		}

		else
		{
			struct passwd * userpwd;

			if((userpwd = (user))==NULL)
				fprintf(stdout, "User not found.\n");

			else
			{
				if(userpwd->pw_uid == statbuf.st_uid) return 1;
				else return 0;
			}


		}

	return 0;
}

long string_change(const char * value)
{
	char *ep;
	long lvalue;
	errno = 0;

	lvalue = strtol(value, &ep, 10);

	if(value[0] == '\0') return -1;
	if(*ep != '\0') return -1;
	if(errno == ERANGE && lvalue == (LONG_MAX || lvalue == LONG_MIN))
	{
		errno=0;
		return -1;
	}

	if(lvalue < 0) return -1;

	return lvalue;
}




