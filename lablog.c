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
	char ** usernames = open_info_file(tempInfoFile, infoFile, &numUsernames);
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
char ** open_info_file(FILE * tempInfoFile, FILE * infoFile, int * numUsernamesDest){
	// check if temporary info file already exists
	if(access(TEMPINFOFILE, F_OK) == false){
		// TODO
		fprintf(stderr, "temporary file was found at start of program, implement this recovery\n");
	}

	char ** usernamesDestination;
	infoFile = fopen(INFOFILE, "r");
	if(infoFile == NULL){ // if the info file doesn't exist
		fprintf(stderr, "File \"%s\" not found, creating file...\n", INFOFILE);

		infoFile = fopen(INFOFILE, "w"); // create the info file
		if(infoFile == NULL){
			fprintf(stderr, "Couldn't create %s\n", INFOFILE);
			exit(1);
		}

		printf("Please provide a username: ");
		char * providedUsername = malloc(USERNAMEBUFFERSIZE);
		fgets(providedUsername, USERNAMEBUFFERSIZE, stdin);

		fputs("# usernames\n", infoFile);
		fputs("1\n", infoFile);
		fputs(providedUsername, infoFile);
		fputs("\n", infoFile);
		fclose(infoFile);

		printf("%s created. Please try your previous command again.\n", INFOFILE);
		exit(0);
	} else {
		// read the usernames comment line in the info file
		char buffer[USERNAMEBUFFERSIZE + 1] = "\0";
		char * fileLine = fgets(buffer, USERNAMEBUFFERSIZE, infoFile);
		if(fileLine == NULL)
			file_read_error(INFOFILE);

		// get the number of usernames from the info file
		fileLine = fgets(buffer, USERNAMEBUFFERSIZE, infoFile);
		if(fileLine == NULL)
			file_read_error(INFOFILE);
		int numUsernames = atoi(buffer);
		*numUsernamesDest = numUsernames;

		// get the usernames from the info file and place them into usernamesDestination[]
		usernamesDestination = malloc((numUsernames + 1) * sizeof(char *)); // + 1 to alloc space if a new username is registered
		for(int i = 0; i < numUsernames; i++){
			if(fgets(buffer, USERNAMEBUFFERSIZE, infoFile) == NULL)
				file_read_error(INFOFILE);

			char * username = malloc(USERNAMEBUFFERSIZE * sizeof(char));

			strcpy(username, buffer);
			usernamesDestination[i] = username;

			// remove the '\n' from the username string
			int j = 0;
			while(username[j] != '\n')
				j++;
			username[j] = '\0';
		}

		// TODO read past next line to set up for the next read
	}

	// copy info file into temp infofile
	rewind(infoFile);
	char buffer[100] = ""; // TODO change this magic number to a constant
	tempInfoFile = fopen(TEMPINFOFILE, "w");
	while(fgets(buffer, 100, infoFile) != NULL)
		fputs(buffer, tempInfoFile);

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

void file_read_error(char * fileName){
	fprintf(stderr, "Error reading %s file\n", fileName);
	exit(0);
}

void print_help_menu(){
	printf("help menu\n"); // TODO
	exit(0);
}
