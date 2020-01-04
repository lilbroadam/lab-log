#include "lablog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

FILE * infoFile = NULL;
FILE * tempInfoFile = NULL;
FILE * logFile = NULL;
FILE * tempLogFile = NULL;

int main(int argc, char* argv[]){
	// display help menu if no arguments are provided
	if(argc < 2)
		print_help_menu();


	char username[USERNAMEBUFFERSIZE];
	open_info_file(tempInfoFile, infoFile, username);
	tempInfoFile = fopen(TEMPINFOFILE, "w");
	write_default_username(tempInfoFile, username);

	logFile = fopen(LOGFILE, "r");
	tempLogFile = fopen(TEMPLOGFILE, "r");

	if(strcmp("login", argv[1]) == 0){

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

// open the info file and return the default username read from the file
void open_info_file(FILE * tempInfoFile, FILE * infoFile, char usernameDestination[]){
	tempInfoFile = fopen(TEMPINFOFILE, "r");
	if(tempInfoFile != NULL){
		// TODO
		fprintf(stderr, "temporary file was found at start of program, implement this recovery\n");
		fclose(tempInfoFile);
	}

	infoFile = fopen(INFOFILE, "r");
	char defaultUsername[USERNAMEBUFFERSIZE] = "";
	if(infoFile == NULL){ // if the info file doesn't exi
		fprintf(stderr, "File \"%s\" not found, creating file...\n", INFOFILE);

		infoFile = fopen(INFOFILE, "w"); // create the info file
		if(infoFile == NULL){
			fprintf(stderr, "Couldn't open %s\n", INFOFILE);
			exit(1);
		}

		printf("Please provide a default username: ");
		fgets(usernameDestination, USERNAMEBUFFERSIZE, stdin);
	} else {
		// read the first comment line in the info file
		char * fileLine = fgets(defaultUsername, USERNAMEBUFFERSIZE, infoFile);
		if(fileLine == NULL)
			file_read_error(INFOFILE);

		// get the default user name from the info file and output it
		fileLine = fgets(defaultUsername, USERNAMEBUFFERSIZE, infoFile);
		if(fileLine == NULL)
			file_read_error(INFOFILE);
		strcpy(usernameDestination, fileLine);

		// TODO read past next line to set up for the next read
	}

	// PICKUP copy info file into temp infofile
}

void write_default_username(FILE * outputFile, char username[]){
	fprintf(outputFile, "# default username\n");
	fprintf(outputFile, "%s", username);
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
