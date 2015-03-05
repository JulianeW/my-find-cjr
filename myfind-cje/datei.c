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

const char *prgname = NULL;

/*
 * ------------------------------------------------------------- prototypes--
 */

void checkFile(const char *file);
void noparam(void);
static void correctusage(void);
void ls(const char *file);
char *modifytime(time_t ftime);
void checkpermissions(mode_t st_mode, char *mode);
void check_type(const char * pattern, struct stat statbuf, const char * filename);
void check_path(const char * pattern, const char * pathname, const char * filename);


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

/*
 * ------------------------------------------------------------- functions --
 */

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

	char fileprm[10];
	fileprm = "----------";
	struct stat lsstat;

	checkpermissions(lsstat.st_mode, fileprm);

	fprintf(stdout, "%ld\t%ld\t%ld\t%ld\n%s",
			(long)lsstat.st_ino,
			(long)lsstat.st_blocks/2,
			(long)lsstat.st_nlink,
			(long)lsstat.st_gid,
			modifytime(lsstat.st_mtime), file);


}

void noparam(void)
{
	/* if no parameter is given, the current directory with subdirectory is printeed */



}

char *modifytime(time_t ftime)
{
	struct tm *time;
	static char filetime[30];

	time=localtime(&ftime);
	strftime(filetime, sizeof filetime, "%B %d %H:%M ", time);

	    return(filetime);
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

	if (st_mode & S_IFREG)
		mode[0] = '-';
	else if (st_mode & S_IFDIR)
		mode[0] = 'd';
	else if (st_mode & S_IRUSR)
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
 * \param *pattern
 * \param stat statbuf
 * \param filename
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */

void check_type(const char * pattern, struct stat statbuf, const char * filename)
{
	char *type_chars = "bcdflps"; /* bcdflps represent the first letter of the valid types that can be found with myfind/find */
	char *help_chars = NULL; /* used for strchr */

	help_chars = strchr(type_chars, pattern[0]); /* The strchr() function returns a pointer to the first occurrence of
		the character c in the string s. It returns a pointer to the matched character or NULL if the character is not found. */
/*
	if(strlen(pattern) == 1 && (help_chars != NULL))
	{
				if ((strcmp(pattern, "b") == 0 && S_ISBLK(statbuf.st_mode))
				printf("%s"\n", filename);
		else if ((strcmp(pattern, "c") == 0 && S_ISCHR(statbuf.st_mode))
				printf("%s"\n", filename);
		else if ((strcmp(pattern, "d") == 0 && S_ISDIR(statbuf.st_mode))
				printf("%s"\n", filename);
		else if ((strcmp(pattern, "f") == 0 && S_ISREG(statbuf.st_mode))
				printf("%s"\n", filename);
		else if ((strcmp(pattern, "l") == 0 && S_ISLNK(statbuf.st_mode))
				printf("%s"\n", filename);
		else if ((strcmp(pattern, "p") == 0 && S_ISFIFO(statbuf.st_mode))
				printf("%s"\n", filename);
		else if ((strcmp(pattern, "s") == 0 && S_ISSOCK(statbuf.st_mode))
				printf("%s"\n", filename);
	}
	else
	{
		fprintf(stderr, "INSERT TEXT (CHECK FIND)\n");
		exit(EXIT_FAILURE);
	}
}
auskommentiert wegen Syntaxerror */
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
 * \param *pattern
 * \param pathname
 * \param filename
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */

void check_path(const char * pattern, const char * pathname, const char * filename)
{
	if(fnmatch(pattern, pathname, 0) == 0) /* The fnmatch() function checks whether the string argument matches the pattern argument. */
	{
		printf("%s"\n, filename);
	}

}


