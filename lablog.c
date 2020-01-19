#include "lablog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>

FILE * infoFile = NULL;
FILE * tempInfoFile = NULL;
FILE * logFile = NULL;
FILE * tempLogFile = NULL;

int main(int argc, char* argv[]){
	// display help menu if no arguments are provided
	if(argc < 2)
		print_help_menu();


	int numUsernames;
	char ** usernames = get_usernames(&numUsernames);
	printf("%s %s\n", usernames[0], usernames[1]);

	// FIXME might not need these after all
	logFile = fopen(LOGFILE, "r");
	tempLogFile = fopen(TEMPLOGFILE, "r");

	if(strcmp("login", argv[1]) == 0){
		if(argv[2] == NULL)
			print_help_menu();

		// login(argv[2], usernames, numUsernames);
	}else if(strcmp("logout", argv[1]) == 0){

	}else if(strcmp("drive", argv[1]) == 0){

	}else if(strcmp("break", argv[1]) == 0){

	}else if(strcmp("print", argv[1]) == 0){

	}else if(strcmp("report", argv[1]) == 0){

	}else if(strcmp("config", argv[1]) == 0){

	}else{
		print_help_menu();
	}

	// TODO need to free malloc'ed usernames

	cleanup_files(infoFile, tempInfoFile, logFile, tempLogFile);
}


// open the info file, get the usernames from the info file, store the number of usernames in numUsernameDest,
// return a pointer to a string array of usernames, close the info file
char ** get_usernames(int * numUsernamesDest){
	// check if temporary info file already exists
	if(access(TEMPINFOFILE, F_OK) == false){ // TODO implement recovery (?)
		fprintf(stderr, "Unexpectedly found %s at start of program\nAborting program\n", TEMPINFOFILE);
		exit(0);
	}


	infoFile = fopen(INFOFILE, "r");
	if(infoFile == NULL) // if the info file doesn't exist
		info_file_not_found();

	// skip the usernames comment line in the info file
	char usernameBuffer[USERNAMEBUFFERSIZE + 1] = "\0";
	if(fgets(usernameBuffer, USERNAMEBUFFERSIZE, infoFile) == NULL)
		file_read_error(INFOFILE);

	// get the number of usernames from the info file
	if(fgets(usernameBuffer, USERNAMEBUFFERSIZE, infoFile) == NULL)
		file_read_error(INFOFILE);
	int numUsernames = atoi(usernameBuffer);
	*numUsernamesDest = numUsernames;

	// get the usernames from the info file and place them into an array to return
	char ** usernames = malloc((numUsernames + 1) * sizeof(char *)); // + 1 to alloc space if a new username is
	for(int i = 0; i < numUsernames; i++){
		// read the next username
		if(fgets(usernameBuffer, USERNAMEBUFFERSIZE, infoFile) == NULL)
			file_read_error(INFOFILE);

		// malloc space for the read username and put a pointer to it in usernames[]
		char * username = malloc(USERNAMEBUFFERSIZE * sizeof(char));
		strcpy(username, usernameBuffer);
		usernames[i] = username;

		// remove the '\n' from the username string
		char * newLineChar = strchr(username, '\n');
		if(newLineChar == NULL)
			file_read_error(INFOFILE);
		*newLineChar = '\0';
	}

	fclose(infoFile);

	return usernames;
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
	// an extra \n is here for some reason
	fputs("# logs\n", infoFile);
	fputs("\n", infoFile); // FIXME this extra \n might not be needed

	printf("%s created. Please try your previous command again.\n", INFOFILE);
	exit(0);
}

void file_read_error(char * fileName){
	fprintf(stderr, "Error reading %s, aborting program\n", fileName);
	exit(0);
}

void print_help_menu(){
	printf("lablog help menu:\n");
	printf("-----------------\n");
	printf("- lablog login <username>\n");
	printf("  + log the login of the specified user\n");
	printf("\n");
	printf("- lablog logout <username>\n");
	printf("  + log the logout of the specified user\n");
	printf("\n");
	printf("- lablog drive <username>\n");
	printf("  + log the specified user as starting to drive\n");
	printf("\n");
	printf("- lablog break <username>\n");
	printf("  + toggle the specified user taking a break\n");
	printf("\n");
	printf("- lablog print\n");
	printf("  + print the current log\n");
	printf("\n");
	printf("- lablog report\n");
	printf("  + export the log into a txt file\n");
	printf("\n");
	printf("- lablog config [setting]\n");
	printf("  + change differ settings\n"); // TODO update this as config becomes more defined
	printf("\n");
	exit(0);
}
