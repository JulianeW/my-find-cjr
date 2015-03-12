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
#include <ctype.h>

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


typedef struct parms
{	Parameter predicate;
	char * pattern;
	struct parms *next;
} parms;

/*
 * --------------------------------------------------------------- globals --
 */
const char *prgname = NULL;
static int params_number = 0;

/*
 * ------------------------------------------------------------- prototypes--
 */

int check_name(const char *file, const char * pattern);
static void correctusage(void);
void ls(const char *file, struct stat *lsstat);
char *modifytime(time_t ftime);
char *checkpermissions(mode_t st_mode);
int check_type(const char * parms, struct stat * buffer);
int check_path(const char * parms, const char * dir_name);
int check_no_user(struct stat *buffer);
int check_user(const char * parms, struct stat *buffer);
long string_change(const char * value);
void do_dir(const char * dir_name, parms *used_parms);
void do_file(const char * file_name, parms *used_params);
int p_print(const char *file_name);
int is_dir (const char * path);
parms * check_parameter(int argc, char * argv[]);



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
	prgname = argv[0]; /* Programmname wird an globale Variable übergeben */
	params_number = argc-1;
	const char * dir_name = "."; /* current directory is used when no directory is entered */
	parms *used_parms;


	/* check_file_parameter(argv, params_number, param_array); */


	if (argc == 1)
	{
		correctusage();
		exit(1);
	}
	else if (argc > 1)
	{

		used_parms = check_parameter(argc, argv);
		if (used_parms == NULL)
		{
			printf("No Parameter found.\n");
			correctusage();
			exit(1);
		}


		if (is_dir(argv[1]))
		{
			printf("directory\n");
			do_dir(argv[1], used_parms);
		}
		else do_file(argv[1], used_parms);
	}





	return EXIT_SUCCESS;
}

/*
 * ------------------------------------------------------------- functions --
 */
 

/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param *dir_name
 * \param *parms
 *
 */

parms * check_parameter(int argc, char * argv[])
{

	parms *current = NULL;
	parms *start = NULL;
	parms *new = NULL;

	int i = 2;

	for (; i < argc; i++)
	{
		if (strncmp(argv[i], "-name", 5) == 0)
		{
			/* Increment da Name für Analyse übergeben werden muss */
			i++;
			new = (parms *) malloc(sizeof(parms));
			if (start == NULL)
			{
				start = new;
				current = new;
			}
			else
			{
				current->next = new;
				current = new;
			}
			new->predicate = NAME;
			new->pattern = argv[i];
		}
		else if (strncmp(argv[i], "-ls", 3) == 0)
		{
			new = (parms *) malloc(sizeof(parms));
				if (start == NULL)
				{
					start = new;
					current = new;
				}
				else
				{
					current->next = new;
					current = new;
				}
				new->predicate = LS;
		}

		else if (strncmp(argv[i], "-print", 6) == 0)
		{	new = (parms *) malloc(sizeof(parms));
			if (start == NULL)
			{
				start = new;
				current = new;
			}
			else
			{
				current->next = new;
				current = new;
			}
			new->predicate = PRINT;
		}
		else
			return NULL;
	}

	if (start == NULL)
	{
		new = (parms *) malloc(sizeof(parms));
		if (start == NULL)
		{
			start = new;
			current = new;
		}
		else
		{
			current->next = new;
			current = new;
		}
		new->predicate = PRINT;
	}

	return start;


}

void do_dir(const char * dir_name, parms *used_parms)
{
	 const struct dirent *d;
	 DIR *dir = opendir(dir_name);
	 char path[PATH_MAX];

	 if (dir == NULL)
	 {
	     fprintf(stderr, "%s: %s: %s\n", prgname, dir_name, strerror(errno));
	     return;
	 }

	 while((d = readdir(dir)) != NULL)
	 {
	     if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
	     {
	         continue;
	     }

	     snprintf(path, PATH_MAX, "%s/%s", dir_name, d->d_name);

	     if (is_dir(dir_name))
	     {
	       	do_dir(path, used_parms);
	     }

	     else do_file(path, used_parms);

	  }

   closedir(dir);
}


/**
 *
 * \brief
 *
 * @todo restliche Funktionen einbauen
 * @todo kann -print überhaupt noch weitere Argumente haben? wenn nicht, dann sollte
 * 		print vielleicht als erstes gecheckt werden und gar nicht erst in die for-Schleife rein
 * 		gegangen werden
 *
 * \param *parms
 * \param dir_name
 *
 */
 
void do_file(const char * file_name, parms *used_params)
{
	parms *current_param = used_params;
	int success = 1;

	while (current_param != NULL && success == 1)
	{
		switch(current_param->predicate)
		{
			case NAME:
			{
				printf("Name übergeben.\n");/* Call Check_name */
				success = check_name(file_name, current_param->pattern);
				break;
			}
			case PRINT:
			{
				success = p_print(file_name);
				break;

			}
			default: printf("Unknown predicate.\n");

		}
		current_param = current_param->next;

	}

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

int check_name(const char *file, const char * pattern)
{
	printf("Ich checke pattern: %s.\n", pattern);
	return 1;

}


/**
 *
 * \brief Function to check what kind of parameter is used
 *
 * \param *file
 *
 */

void ls(const char *file, struct stat *lsstat)
{

	/** necessary structs for all information needed in ls */
	struct group *mygroup;
	struct passwd *mypw;

	/** filling structs with the file information and getting owner and group information */
	mygroup = getgrgid(lsstat->st_gid);
	mypw = getpwuid(lsstat->st_uid);

	/** calling necessary functions and printing all information asked for in ls */
	fprintf(stdout, "%ld\t%ld\t%s\t%ld\n%s\t%s\t%s\t%s\n",
			(long)lsstat->st_ino,
			(long)lsstat->st_blocks/2,
			checkpermissions(lsstat->st_mode),
			(long)lsstat->st_nlink,
			mypw->pw_name,
			mygroup->gr_name,
			modifytime(lsstat->st_mtime), file);

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
 * \param stat buffer
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */

int check_type(const char * parms, struct stat *buffer)
{

	char *type_chars = "bcdpfls"; /* bcdpfls represent the first letter of the valid types that can be found with myfind/find */
	char *help_chars = NULL; /* used for strchr */

	help_chars = strchr(type_chars, parms[0]); /* The strchr() function returns a pointer to the first occurrence of
		the character c in the string s. It returns a pointer to the matched character or NULL if the character is not found. */

	if (strlen(parms) == 1 && (help_chars != NULL))
	{
		if (strcmp(parms, "b") == 0 && S_ISBLK(buffer->st_mode))
			return 1;
		else if (strcmp(parms, "c") == 0 && S_ISCHR(buffer->st_mode))
			return 1;
		else if (strcmp(parms, "d") == 0 && S_ISDIR(buffer->st_mode))
			return 1;
		else if (strcmp(parms, "f") == 0 && S_ISREG(buffer->st_mode))
			return 1;
		else if (strcmp(parms, "l") == 0 && S_ISLNK(buffer->st_mode))
			return 1;
		else if (strcmp(parms, "p") == 0 && S_ISFIFO(buffer->st_mode))
			return 1;
		else if (strcmp(parms, "s") == 0 && S_ISSOCK(buffer->st_mode))
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

int is_dir (const char * path)
{
	struct stat mystat;
	lstat(path, &mystat);

	if(S_ISDIR(mystat.st_mode))
		return 1;
	return 0;
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

/**
 *
 * \brief Function to check if file has no user
 *
 * \param buffer
  *
 * \return 1 for file without user
 * \return 0 for unsuccessful match
 */

int check_no_user(struct stat *buffer)
{
	if(getpwuid(buffer->st_uid)==NULL)
		return 1;
	else
		return 0;
}

/**
 *
 * \brief Function to check if valid username was given and if file is of this user
 *
 * \param *parms
 * \param buffer
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */

int check_user(const char * parms, struct stat *buffer)
{
	int i = 0;
	int uid = 0;
	int user_length = strlen(parms);

	for(i=0; i<= user_length; i++)
	{
		if(!isdigit(parms[i]))
			break; /* check nur Zahlen? */
	}
		if(i == user_length)
		{
			if((uid = string_change(parms)) > -1)
			{
				if(uid == buffer->st_uid) return 1;
			}
			return 0;
		}

		else
		{
			struct passwd * userpwd;

			if((userpwd = (parms))==NULL)
				fprintf(stdout, "User not found.\n");

			else
			{
				if(userpwd->pw_uid == buffer->st_uid) return 1;
				else return 0;
			}


		}

	return 0;
}

/**
 *
 * \brief Function
 *
 * @todo Beschreibung! Was macht diese Funktion?
 * \param *value
 *
 * \return -1 for error
 * \return lvalue for success
 */

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
int p_print(const char *file_name)
{
	printf("%s", file_name);
	return 1;
}




