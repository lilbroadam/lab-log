#include "lablog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>

void login(char *, char *[], int);

FILE * infoFile = NULL;
FILE * tempInfoFile = NULL;
FILE * logFile = NULL;
FILE * tempLogFile = NULL;

int main(int argc, char* argv[]){
	// display help menu if no arguments are provided
	if(argc < 2)
		print_help_menu();


	int numUsernames;
	char ** usernames = open_and_read_info_file(tempInfoFile, infoFile, &numUsernames);
	logFile = fopen(LOGFILE, "r");
	tempLogFile = fopen(TEMPLOGFILE, "r");

	if(strcmp("login", argv[1]) == 0){
		if(argv[2] == NULL)
			print_help_menu();

		login(argv[2], usernames, numUsernames);
	}else if(strcmp("logout", argv[1]) == 0){

	}else if(strcmp("drive", argv[1]) == 0){

	}else if(strcmp("break", argv[1]) == 0){

	}else if(strcmp("print", argv[1]) == 0){

	}else if(strcmp("report", argv[1]) == 0){

	}else if(strcmp("config", argv[1]) == 0){

	}else{
		print_help_menu();
	}

	cleanup_files(infoFile, tempInfoFile, logFile, tempLogFile);
}

void login(char * username, char * usernames[], int numUsernames){



}



// open the info file and temp info file, return the number of usernames, and pass an array of usernames from the info file
// TODO the signature of this function has a bad design; make it better
char ** open_and_read_info_file(FILE * tempInfoFile, FILE * infoFile, int * numUsernamesDest){
	// check if temporary info file already exists
	if(access(TEMPINFOFILE, F_OK) == false){ // TODO implement recovery (?)
		fprintf(stderr, "Unexpectedly found %s at start of program\nAborting program\n", TEMPINFOFILE);
		exit(0);
	}


	infoFile = fopen(INFOFILE, "r");
	if(infoFile == NULL) // if the info file doesn't exist
		info_file_not_found();

	// read the usernames comment line in the info file
	char usernameBuffer[USERNAMEBUFFERSIZE + 1] = "\0";
	if(fgets(usernameBuffer, USERNAMEBUFFERSIZE, infoFile) == NULL)
		file_read_error(INFOFILE);

	// get the number of usernames from the info file
	if(fgets(usernameBuffer, USERNAMEBUFFERSIZE, infoFile) == NULL)
		file_read_error(INFOFILE);
	int numUsernames = atoi(usernameBuffer);
	*numUsernamesDest = numUsernames;

	// get the usernames from the info file and place them into usernamesDestination[]
	char ** usernamesDestination = malloc((numUsernames + 1) * sizeof(char *)); // + 1 to alloc space if a new username is registered
	for(int i = 0; i < numUsernames; i++){
		if(fgets(usernameBuffer, USERNAMEBUFFERSIZE, infoFile) == NULL)
			file_read_error(INFOFILE);

		char * username = malloc(USERNAMEBUFFERSIZE * sizeof(char));
		strcpy(username, usernameBuffer);
		usernamesDestination[i] = username;

		// remove the '\n' from the username string
		char * newLineChar = strchr(username, '\n');
		if(newLineChar == NULL)
			file_read_error(INFOFILE);
		*newLineChar = '\0';
	}

	// TODO read past next line to set up for the next read

	// copy info file into temp infofile
	// TODO might have to only copy the username part and not the following time log info in order to register a new username
	rewind(infoFile);
	char readFileBuffer[100] = ""; // TODO change this magic number to a constant
	tempInfoFile = fopen(TEMPINFOFILE, "w");
	while(fgets(readFileBuffer, 100, infoFile) != NULL)
		fputs(readFileBuffer, tempInfoFile);

	return usernamesDestination;
}

// close file pointers, overwrite old .lablog files, delete temp files
void cleanup_files(FILE * infoFile, FILE * tempInfoFile, FILE * logFile, FILE * tempLogFile){
	rename(TEMPINFOFILE, INFOFILE);

	if(infoFile != NULL)
		fclose(infoFile);
	if(tempInfoFile != NULL)
		fclose(tempInfoFile);
	if(logFile != NULL)
		fclose(logFile);
	if(tempLogFile != NULL)
		fclose(tempLogFile);

	remove(TEMPINFOFILE);
	remove(TEMPLOGFILE);
}

void info_file_not_found(){
	fprintf(stderr, "File \"%s\" not found, creating file...\n", INFOFILE);

	FILE * infoFile = fopen(INFOFILE, "w"); // create the info file
	if(infoFile == NULL){
		fprintf(stderr, "Couldn't create %s\n", INFOFILE);
		exit(1);
	}

	// TODO should an initial username be requested when creating the info file?
	// or just let the user create one manually?
	printf("Please provide a username: ");
	char * providedUsername = malloc(USERNAMEBUFFERSIZE);
	fgets(providedUsername, USERNAMEBUFFERSIZE, stdin);

	fputs("# usernames\n", infoFile);
	fputs("1\n", infoFile);
	fprintf(infoFile, "%s\n", providedUsername);
	fclose(infoFile);

	printf("%s created. Please try your previous command again.\n", INFOFILE);
	exit(0);
}

void file_read_error(char * fileName){
	fprintf(stderr, "Error reading %s file\n", fileName);
	exit(0);
}

void print_help_menu(){
	printf("help menu\n"); // TODO
	exit(0);
}
