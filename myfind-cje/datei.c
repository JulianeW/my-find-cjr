/**
 * @file datei.c
 * Betriebssysteme Myfind File
 * Beispiel 1
 *
 * @author Claudia Baierl ic14b003 <claudia.baierl@technikum-wien.at>
 * @author Rene Elsner ic14b016 <rene.elsner@technikum-wien.at>
 * @author Juliane Winkler ic14b059 <juliane.winkler@technikum-wien-at>
 * @date 2015/03/13
 *
 * @version 1
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
#include <error.h>
#include <time.h>
#include <malloc.h>
#include <fnmatch.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <libgen.h>
#include <stdarg.h>

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
	PATH
} Parameter;


typedef struct parms
{	Parameter predicate;
	char * pattern;
	struct parms * next;
} parms;

/*
 * --------------------------------------------------------------- globals --
 */
const char * prgname = NULL;
static int params_number = 0;

/*
 * ------------------------------------------------------------- prototypes--
 */

int check_name(const char * file, const char * pattern);
static void correctusage(void);
void ls(const char * file);
char * modifytime(time_t ftime);
char * checkpermissions(mode_t st_mode);
int check_type(const char * parms, struct stat * buffer);
int check_path(const char * parms, const char * dir_name);
int check_no_user(struct stat * buffer);
int check_user(const char * parms, struct stat * buffer);
long string_change(const char * value);
void do_dir(const char * dir_name, parms * used_params);
void do_file(const char * file_name, parms * used_params);
void read_params(const char * file_name, parms * used_params);
int p_print(const char * file_name);
int is_dir (const char * path);
parms * check_parameter(int argc, char * argv[]);
void printf_handling(char * format, ...);


/**
 *
 * \brief Program that imitates find
 *
 * This is the entry point of normal C programs.
 *
 *
 * \param argc the number of arguments
 * \param argv the arguments (including the program name in argv[0])
 *
 * \return EXIT_SUCCESS
 *
 */

int main(int argc, char* argv[])
{
	prgname = argv[0]; /* program name is passed to global variable prgname */
	params_number = argc-1; /* number of parameters is passed to global variable params_number */
	parms * used_params;

	/* if no parameter was entered, call correctusage and terminate */
	if (argc == 1)
	{
		correctusage();
		exit(1);
	}
	else if (argc > 1)
	{
		/*check parameters and save them in linked list */
		used_params = check_parameter(argc, argv);

		/* If no parameters were found and saved in the linked list, program terminates */
		if (used_params == NULL)
		{
			printf_handling("No Parameter found.\n");
			correctusage();
			exit(1);
		}

		if (is_dir(argv[1]))
		{
			do_dir(argv[1], used_params);
		}
		else do_file(argv[1], used_params);
	}


	return EXIT_SUCCESS;
}

/*
 * ------------------------------------------------------------- functions --
 */
 

/**
 *
 * \brief Function to create a linked list with the parameters
 *
 * \param argc
 * \param argv
 *
 * \return linked list
 *
 */

parms * check_parameter(int argc, char * argv[])
{

	/* Initialize Pointer for linked list */
	parms * current = NULL;
	parms * start = NULL;
	parms * new = NULL;
	int i = 2;
	int j = 0;

	if (strncmp(argv[1], "-", 1) == 0)
	{
		printf_handling("First argument needs to be a path.\n");
		correctusage();
		exit(1);
	}

	/* counter variable = 2 as parameter passing only begins at argc = 2 - all parameters are checked */

	for (; i < argc; i++)
	{
		/* checking all parameters given from user for correctness and if so, saving them in linked list */
		if (strncmp(argv[i], "-name", 5) == 0)
		{
			/* Increment i as parameter is needed to check with name */
			i++;
			new = (parms *) malloc(sizeof(parms));
			if (new == NULL)
			{
				printf_handling("Allocation failed.\n");
				free(new);
				exit(1);
			}
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
			if (argv[i] == NULL)
			{
				printf("Missing argument.");
				exit(1);
			}
			new->pattern = argv[i];
		}
		else if (strncmp(argv[i], "-ls", 3) == 0)
		{
			new = (parms *) malloc(sizeof(parms));
			if (new == NULL)
			{
				printf_handling("Allocation failed.\n");
				free(new);
				exit(1);
			}
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
		{
			new = (parms *) malloc(sizeof(parms));
			if (new == NULL)
			{
				printf_handling("Allocation failed.\n");
				free(new);
				exit(1);
			}
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
		else if (strncmp(argv[i], "-user", 5) == 0)
		{
			/* Increment i as parameter is needed to check user and saved in the structure */
			i++;
			new = (parms *) malloc(sizeof(parms));
			if (new == NULL)
			{
				printf_handling("Allocation failed.\n");
				free(new);
				exit(1);
			}
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
			new->predicate = USER;
			if (argv[i] == NULL)
			{
				printf("Missing argument.");
				exit(1);
			}
			new->pattern = argv[i];
		}
		else if (strncmp(argv[i], "-nouser", 7) == 0)
		{
			new = (parms *) malloc(sizeof(parms));
			if (new == NULL)
			{
				printf_handling("Allocation failed.\n");
				free(new);
				exit(1);
			}
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
			new->predicate = NOUSER;
		}
		else if (strncmp(argv[i], "-path", 5) == 0)
		{
			/* Increment i as path is needed to be compared */
			i++;
			new = (parms *) malloc(sizeof(parms));
			if (new == NULL)
			{
				printf_handling("Allocation failed.\n");
				free(new);
				exit(1);
			}
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
			new->predicate = PATH;
			if (argv[i] == NULL)
			{
				printf("Missing argument.");
				exit(1);
			}
			new->pattern = argv[i];
		}
		else if (strncmp(argv[i], "-type", 5) == 0)
		{
			/* Increment i as type is needed to check against the file type */
			i++;
			new = (parms *) malloc(sizeof(parms));
			if (new == NULL)
			{
				printf_handling("Allocation failed.\n");
				free(new);
				exit(1);
			}
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
			new->predicate = TYPE;

			if (argv[i] == NULL)
			{
				printf("Missing argument.\n");
				exit(1);
			}

			/* checking if correct length and correct parameter was passed from user, otherwise terminate */
			j = strlen(argv[i]);
			if (j != 1)
			{
				printf("Argument is too long.\n");
				exit(1);
			}

			new->pattern = argv[i];
			if (strchr(new->pattern, 'b') == NULL && strchr(new->pattern, 'c') == NULL && strchr(new->pattern, 'd') == NULL && strchr(new->pattern, 'f') == NULL && strchr(new->pattern, 'l') == NULL && strchr(new->pattern, 'p') == NULL && strchr(new->pattern, 's') == NULL)
			{
				printf("Wrong parameter.\n");
				exit(1);
			}
		}
		else
			return NULL;
	}

	/* if no parameter was passed call print function */
	if (start == NULL)
	{
		new = (parms *) malloc(sizeof(parms));
		if (new == NULL)
		{
			printf_handling("Allocation failed.\n");
			free(new);
			exit(1);
		}
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
	free(new);
}

/**
 * \brief Function to check if the current file has the necessary parameters and print it
 *
 * \param file_name
 * \param used_params
 */

void read_params(const char * file_name, parms * used_params)
{
	parms * current_param = used_params;
	int success = 1;
	struct stat current_file;
	int printing = 0;

	lstat(file_name, &current_file);
	if (lstat(file_name, &current_file) == -1)
	{
		fprintf(stderr, "%s : %s", file_name, strerror(errno));
		exit(1);
	}

	/* checking linked list as long as it is filled and no function terminates the process, due to mismatch */
	while (current_param != NULL && success == 1)
	{
		switch(current_param->predicate)
		{
			case NAME:
			{
				success = check_name(file_name, current_param->pattern);
				break;
			}
			case PRINT:
			{
				success = p_print(file_name);
				printing++;
				break;

			}
			case LS:
			{
				ls(file_name);
				/* Increment to check if print function is needed */
				printing++;
				break;
			}
			case TYPE:
			{
				success = check_type(current_param->pattern, &current_file);
				break;
			}
			case NOUSER:
			{
				success = check_no_user(&current_file);
				break;
			}
			case PATH:
			{
				success = check_path(current_param->pattern, file_name);
				break;
			}
			case USER:
			{
				success = check_user(current_param->pattern, &current_file);
				break;
			}
			default: printf("Unknown predicate.\n");

		}

		current_param = current_param->next;
	}

	/* Call print function if no ls and no print is given */
	if (printing == 0 && success == 1)
		p_print(file_name);

}
/**
 * \brief Function to open directories recursively
 *
 * \param dir_name
 * \param used_params
 *
 */

void do_dir(const char * dir_name, parms * used_params)
{
	 const struct dirent * d;
	 DIR * dir = opendir(dir_name);
	 char path[PATH_MAX];

	 /* Call read_params so directories are handled */
	 read_params(dir_name, used_params);

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

	     if (is_dir(path))
	     {
	    	 do_dir(path, used_params);

	     }
	     else do_file(path, used_params);


	  }

   closedir(dir);
}

/**
 *
 * \brief Function to check each file in directory
 *
 * \param file_name
 * \param used_params
 *
 */
 
void do_file(const char * file_name, parms *used_params)
{
	read_params(file_name, used_params);
}

/**
 *
 * \brief Function to check if entered name exists
 *
 * \param file
 * \param pattern
 *
 * \returns 1 if match is successful
 *
 */

int check_name(const char * file, const char * pattern)
{
	char * final_name = NULL;
	char * name;
	int success = 0;

	name = strdup(file);
	final_name = basename(name);

	if (fnmatch(final_name, pattern, 0) == 0)
		success = 1;

	return success;

}

/**
 *
 * \brief Function to print out i-node information in -ls mode
 *
 * \param file
 *
 */

void ls(const char * file)
{

	/* necessary structs for all information needed in ls */
	struct stat lsstat;
	lstat(file, &lsstat);
	struct group *mygroup = NULL;
	struct passwd *mypw = NULL;

	/* filling structs with the file information and getting owner and group information */
	mygroup = getgrgid(lsstat.st_gid);
	mypw = getpwuid(lsstat.st_uid);

	/* calling necessary functions and printing all information asked for in ls */
	printf_handling("%ld\t%ld\t%s\t%ld\t%s\t%s\t%s\t%s\n",
			(long)lsstat.st_ino,
			(long)lsstat.st_blocks/2,
			checkpermissions(lsstat.st_mode),
			(long)lsstat.st_nlink,
			mypw->pw_name,
			mygroup->gr_name,
			modifytime(lsstat.st_mtime), file);

	/* checking for symbolic links */
	if (S_ISLNK(lsstat.st_mode))
	{
		char * link;
		int len = 0;

		link = (char*) malloc(sizeof(char));
		if (link == NULL)
			{ printf_handling("Allocation not possible.\n"); exit(1); }

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
 * \brief Function to modify time in requested format
 *
 * \param ftime
 *
 * \return string with formated date and time
 *
 */

char * modifytime(time_t ftime)
{
	struct tm * time;
	static char filetime[30];

	time=localtime(&ftime);
	strftime(filetime, sizeof filetime, "%B %d %H:%M ", time);

	    return(filetime);
}

/**
 *
 * \brief Function that prints the correct usage of myfind when a wrong parameter is entered
 *
 */
static void correctusage(void)
{
	printf("Correct use of: %s\n -user: searches entries of certain users\n -name: searches entries with a given name\n"
			"-type (bcdpfls): searches for certain type\n"
			"-print: prints name of entry\n"
			"-ls: prints certain information of file\n"
			"-nouser: searches entries with nouser\n"
			"-path: searches entries with given path\n"
			"For further information do find --help\n"
			, prgname);

}

/**
 *
 * \brief Function to check what kind of permission is given for each file and directory
 *
 * \param st_mode
 *
 * \return string with filled in permission
 *
 */

char * checkpermissions(mode_t st_mode)
{
	static char mode[11];
	strcpy (mode, "----------");

	/* checking file type */
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

	/* checking file permissions */
	if (st_mode & S_IRUSR)
		mode[1] = 'r';
	if (st_mode & S_IWUSR)
		mode[2] = 'w';
	if (st_mode & S_IXUSR)
	{
		mode[3] = 'x';
		if(st_mode & S_ISUID)
			mode[3] = 's';
		else
			mode[3] = 'x';
	}
	else if (st_mode & S_ISUID)
		mode[3] = 'S';
	if (st_mode & S_IRGRP)
		mode[4] = 'r';
	if (st_mode & S_IWGRP)
		mode[5] = 'w';
	if (st_mode & S_IXGRP)
	{
		mode[6] = 'x';
		if(st_mode & S_ISGID)
			mode[6] = 's';
		else
			mode[6] = 'x';
	}
	else if(st_mode & S_ISGID)
		mode[6] = 'S';
	if (st_mode & S_IROTH)
		mode[7] = 'r';
	if (st_mode & S_IWOTH)
		mode[8] = 'w';
	if (st_mode & S_IXOTH)
	{
		mode[9] = 'x';
		if (st_mode & S_ISVTX)
			mode[9] = 't';
		else
			mode[9] = 'x';
	}
	else if (st_mode & S_ISVTX)
		mode[9] = 'T';

	return mode;
}

/**
 *
 * \brief Function to check file matches the entered type
 *
 * \param parms
 * \param buffer
 *
 * \return 1 for successful match
 * \return 0 for unsuccessful match
 */

int check_type(const char * parms, struct stat * buffer)
{
		int success = 0;

		if (strcmp(parms, "b") == 0 && S_ISBLK(buffer->st_mode))
			success = 1;
		if (strcmp(parms, "c") == 0 && S_ISCHR(buffer->st_mode))
			success = 1;
		if (strcmp(parms, "d") == 0 && S_ISDIR(buffer->st_mode))
			success = 1;
		if (strcmp(parms, "f") == 0 && S_ISREG(buffer->st_mode))
			success = 1;
		if (strcmp(parms, "l") == 0 && S_ISLNK(buffer->st_mode))
			success = 1;
		if (strcmp(parms, "p") == 0 && S_ISFIFO(buffer->st_mode))
			success = 1;
		if (strcmp(parms, "s") == 0 && S_ISSOCK(buffer->st_mode))
			success = 1;

		return success;

}

/**
 *
 * \brief Function to check if file is a directory
 *
 * \param path
 *
 * \return directory
 * \return -1 if lstat failed
 *
 */

int is_dir (const char * path)
{
	struct stat mystat;

	if (lstat(path, &mystat) == -1)
	{
		fprintf(stderr, "%s: %s \n", path, strerror(errno));
		return -1;
	}

	return(S_ISDIR(mystat.st_mode));
}


/**
 *
 * \brief Function to check if file matches the path
 *
 * \param parms
 * \param dir_name
 *
 * \return 1 for successful match
 * \return 0 for unsuccesful match
 */

int check_path(const char * parms, const char * dir_name)
{

	if (fnmatch(parms, dir_name, 0) == 0)
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

int check_no_user(struct stat * buffer)
{
	if (getpwuid(buffer->st_uid) == NULL)
		return 1;
	else
		return 0;
}

/**
 *
 * \brief Function to check if valid user name was given and if file is of this user
 *
 *  Checks for digits-only; if UID, check file_info
 *  If not digits-only, checks if valid user name; if yes, check file_info
 *
 * \param parms
 * \param buffer
 *
 * \return 1 for successful match
 * \return 0 for unsuccessful match
 */

int check_user(const char * parms, struct stat * buffer)
{
	int i = 0;
	int uid = 0;
	int user_length = strlen(parms);

	for(i=0; i<= user_length; i++) /* check if user id is digits only */
	{
		if (!isdigit(parms[i]))
			break;
	}
		if (i == user_length) /* if successful, check UID */
		{
			if ((uid = string_change(parms)) > -1)
			{
				if (uid == buffer->st_uid) return 1;
			}
			return 0;
		}

		else /* no UID */
		{
			struct passwd * userpwd;

			userpwd = getpwnam(parms);

			if (userpwd == NULL)
			{
				fprintf(stderr, "%s: `%s' is not the name of a known user.\n", prgname, parms);
				exit(1);
			}

			else /* checks if valid username */
			{
				if (userpwd->pw_uid == buffer->st_uid) return 1;
				else return 0;
			}


		}

	return 0;
}

/**
 *
 * \brief Function that converts string to long value
 *
 * \param *value String to convert
 *
 * \return -1 for error
 * \return lvalue for success
 *
 */

long string_change(const char * value)
{
	char * ep;
	long lvalue;
	errno = 0;

	lvalue = strtol(value, &ep, 10);

	if (value[0] == '\0')
		return -1;
	if (* ep != '\0')
		return -1;
	if (errno == ERANGE && lvalue == (LONG_MAX || lvalue == LONG_MIN))
	{
		errno=0;
		return -1;
	}

	if (lvalue < 0)
		return -1;

	return lvalue;
}

/**
 *
 * \brief Function to print
 *
 * \param file_name
 *
 * return 1 on success
 *
 */

int p_print(const char * file_name)
{
	printf_handling("%s\n", file_name);
	return 1;
}

/**
 *
 * \brief Function error handling of printf
 *
 * \param format
 * \param ...
 *
 */

void printf_handling(char * format, ...)
{
	va_list args;

	va_start(args, format);
	if (vprintf(format, args) < 0) error(1, 1, "%d", errno);
	va_end(args);
}




