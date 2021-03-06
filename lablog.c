#include "lablog.h"
#include "commandhandler.h"
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

	infoFile = fopen(INFOFILE, "r");
	tempInfoFile = fopen(TEMPINFOFILE, "w");
	// TODO check if file pointer points to null


	// FIXME might not need these after all
	// logFile = fopen(LOGFILE, "r");
	// tempLogFile = fopen(TEMPLOGFILE, "r");

	bool updateInfoFile = true;

	if(strcmp("login", argv[1]) == 0){
		if(argv[2] == NULL)
			print_help_menu();

		// TODO check that all given usernames are registered usernames
		int numLoginUsernames = argc - 2;
		char ** loginUsernames = malloc(numLoginUsernames * sizeof(char *));
		for(int i = 2; i < argc; i++)
			loginUsernames[i - 2] = argv[i];

		login(infoFile, tempInfoFile, loginUsernames, numLoginUsernames, usernames, numUsernames);
	}else if(strcmp("logout", argv[1]) == 0){
		logout(infoFile, tempInfoFile);
	}else if(strcmp("drive", argv[1]) == 0){
		int numDriverUsernames = argc - 2;
		char ** driverUsernames = malloc(numDriverUsernames * sizeof(char *));
		for(int i = 2; i < argc; i++)
			driverUsernames[i - 2] = argv[i];

		drive(infoFile, tempInfoFile, driverUsernames, numDriverUsernames);
	}else if(strcmp("break", argv[1]) == 0){

	}else if(strcmp("print", argv[1]) == 0){ // print log to console

	}else if(strcmp("export", argv[1]) == 0){ // print log to file
		export(infoFile, argv[2]);
		updateInfoFile = false; // TODO change how this problem is dealt with
	}else if(strcmp("config", argv[1]) == 0){

	}else{
		// TODO close temp info file
		print_help_menu();
	}

	// TODO need to free malloc'ed usernames

	cleanup_files(infoFile, tempInfoFile, updateInfoFile, logFile, tempLogFile);
}


// open the info file, get the usernames from the info file, store the number of usernames in numUsernameDest,
// return a pointer to a string array of usernames, close the info file
char ** get_usernames(int * numUsernamesDest){
	// TODO this check should be done before this function is called
	// check if temporary info file already exists
	if(access(TEMPINFOFILE, F_OK) == false){ // TODO implement recovery (?)
		fprintf(stderr, "Unexpectedly found %s at start of program, aborting program\n", TEMPINFOFILE);
		exit(0);
	}


	infoFile = fopen(INFOFILE, "r");
	if(infoFile == NULL) // if the info file doesn't exist
		info_file_not_found();

	// skip the "# usernames" comment line in the info file
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
void cleanup_files(FILE * infoFile, FILE * tempInfoFile, bool renameInfoFile, FILE * logFile, FILE * tempLogFile){
	if(renameInfoFile)
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

// TODO maybe change this function name to create_info_file()?
// TODO might not need usernames in the info file after all?
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
	// there's an extra \n here from providedUsername
	fputs("# logs\n", infoFile);

	fclose(infoFile);

	printf("%s created. Please try your previous command again.\n", INFOFILE);
	exit(0);
}

// TODO add another string to provide a better description of the error
void file_read_error(char * fileName){
	fprintf(stderr, "Error reading %s, aborting program\n", fileName);

	// TODO need to handle intermittent temp file

	exit(0);
}

void print_help_menu(){
	printf("lablog help menu:\n");
	printf("-----------------\n");
	printf("- lablog login [<username>]\n");
	printf("  + login the specified users at the current time\n");
	printf("\n");
	printf("- lablog logout\n");
	printf("  + logout the currently logged in users at the current time\n");
	printf("\n");
	printf("- lablog drive [<username>]\n");
	printf("  + log the given users as starting to drive at the current time\n");
	printf("  + any driver driving prior to this command will have their driver end time marked as the current time\n");
	printf("\n");
	// printf("- lablog break <username>\n");
	// printf("  + toggle the specified user taking a break\n");
	// printf("\n");
	// printf("- lablog print\n");
	// printf("  + print the current log to the console\n");
	// printf("\n");
	printf("- lablog export <filename>\n");
	printf("  + export the log to <filename>.txt\n");
	printf("\n");
	// printf("- lablog config [setting]\n");
	// printf("  + change differ settings\n"); // TODO update this as config becomes more defined
	// printf("\n");
	exit(0);
}
