#include "lablog.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void write_drive_info(FILE *, char **, int);
void skip_username_info(FILE *, FILE *);
void copy_line(FILE *, FILE *);
void write_current_datetime(FILE *);


// the given infoFile and tempFile should already be open when this function is called
// the given usernames to login should already be confirmed to be registeredUsernames when this function is called
void login(FILE * infoFile, FILE * tempFile, char * loginUsernames[], int numLoginUsernames,
		char * registeredUsernames[], int numRegisteredUsernames){

	// forward past the username info
	skip_username_info(infoFile, tempFile);

	// read the "# logs" comment line in the info file
	copy_line(infoFile, tempFile);

	// for each session logged,
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";
	while(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) != NULL){ // while we haven't reached the end of the file
		fputs(fileLineBuffer, tempFile);

		// check to make sure the current session isn't already logged in
		fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read end time
		if(strcmp("_\n", fileLineBuffer) == 0){ // TODO change this symbol to a constant
			printf("Previous session must be logged out before logging in.\n");
			fclose(infoFile);
			fclose(tempFile);
			remove(TEMPINFOFILE);
			exit(0);
		}
		fputs(fileLineBuffer, tempFile);

		copy_line(infoFile, tempFile); // read users in session

		fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read users driving
		while(strcmp(fileLineBuffer, "\n") != 0){
			fputs(fileLineBuffer, tempFile);
			copy_line(infoFile, tempFile); // reading driving start time
			copy_line(infoFile, tempFile); // reading driving end time

			fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read users driving
		}
		fputs(fileLineBuffer, tempFile);
	}

	// find the next empty line and go to the line after it

	// write the log in info for this session to the tempFile // TODO move this into its own function
	write_current_datetime(tempFile); // <date> <time>
	fputs("_\n", tempFile); // write a "_" while lab session hasn't ended
	fprintf(tempFile, "%d ", numLoginUsernames);
	for(int i = 0; i < numLoginUsernames; i++) // this prints an extra space after the last username
		fprintf(tempFile, "%s ", loginUsernames[i]); // but it shouldn't matter
	fputs("\n", tempFile);
	fputs("\n", tempFile);

}

// the given infoFile and tempFile should already be open when this function is called
// this function will logout all the users in the already logged in session (end the current session)
// TODO how to prevent double logout?
void logout(FILE * infoFile, FILE * tempFile){
	// forward past the username info
	skip_username_info(infoFile, tempFile);

	// read the "# logs" comment line in the info file
	copy_line(infoFile, tempFile);

	// for each session logged,
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";
	while(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) != NULL){ // while we haven't reached the end of the file
		fputs(fileLineBuffer, tempFile);

		// TODO check to make sure the current session isn't already logged out

		fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read end time
		if(strcmp("_\n", fileLineBuffer) == 0) // replace blank end time with current time // TODO change this symbol to a constant
			write_current_datetime(tempFile);
		else
			fputs(fileLineBuffer, tempFile);

		copy_line(infoFile, tempFile); // read users in session

		fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read users driving
		while(strcmp(fileLineBuffer, "\n") != 0){
			fputs(fileLineBuffer, tempFile);

			copy_line(infoFile, tempFile); // reading driving start time

			// reading driving end time
			if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
				file_read_error(INFOFILE);
			if(strcmp("_\n", fileLineBuffer) == 0){ // end the remaining driver
				write_current_datetime(tempFile);
			}else
				fputs(fileLineBuffer, tempFile);

			fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read users driving
		}
		fputs(fileLineBuffer, tempFile);
	}

}

void drive(FILE * infoFile, FILE * tempFile, char ** driverUsernames, int numDriverUsernames){
	// forward past the username info
	skip_username_info(infoFile, tempFile);

	// read the "# logs" comment line in the info file
	copy_line(infoFile, tempFile);

	// for each session logged,
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";
	bool foundLoggedInLog = false;
	while(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) != NULL){ // while we haven't reached the end of the file
		fputs(fileLineBuffer, tempFile);

		fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read end time
		if(strcmp("_\n", fileLineBuffer) == 0) // TODO change this symbol to a constant
			foundLoggedInLog = true;
		fputs(fileLineBuffer, tempFile);

		copy_line(infoFile, tempFile); // read users in this session

		bool foundLoggedInDriver = false;
		fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read users driving
		while(strcmp(fileLineBuffer, "\n") != 0){ // read driving logs in this session
			fputs(fileLineBuffer, tempFile);

			copy_line(infoFile, tempFile); // read driving start time

			// read driving end time
			if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
				file_read_error(INFOFILE);
			if(strcmp("_\n", fileLineBuffer) == 0){ // if this driver is driving, end it // TODO change this symbol to a constant
				foundLoggedInDriver = true;
				write_current_datetime(tempFile); // write the end time for the previous driver
				write_drive_info(tempFile, driverUsernames, numDriverUsernames); // write the info for the new driver
			}else
				fputs(fileLineBuffer, tempFile);

			fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile); // read users driving
		}

		if(!foundLoggedInDriver & foundLoggedInLog) // if this is the first driver of the log that is currently logged in
			write_drive_info(tempFile, driverUsernames, numDriverUsernames);

		fputs(fileLineBuffer, tempFile); // write the last thing read from the info file to the temp file

	}

}

void write_drive_info(FILE * destination, char ** driverUsernames, int numDriverUsernames){
	for(int i = 0; i < numDriverUsernames; i++) // write users driving
		fprintf(destination, "%s ", driverUsernames[i]);
	fprintf(destination, "\n");

	// write driving start time
	write_current_datetime(destination);

	// writing driving end time (place holder)
	fprintf(destination, "_\n"); // TODO change this symbol to a constant
}

void toggleBreak(){

}

void print(){

}

void report(){

}

void config(){

}

// the given infoFile and tempFile should already be open at the start of this function
// this function will copy all the username info from infoFile into tempFile, including
// the following blank line. The infoFile's cursor will be in front of "# logs" at the end
// of this function
void skip_username_info(FILE * infoFile, FILE * tempFile){
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";

	// skip the usernames comment line in the info file
	copy_line(infoFile, tempFile);

	// get the number of usernames from the info file
	if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
		file_read_error(INFOFILE);
	int numUsernames = atoi(fileLineBuffer);
	fputs(fileLineBuffer, tempFile);

	// get the usernames from the info file and place them into an array to return
	for(int i = 0; i < numUsernames; i++)
		// read the next username
		copy_line(infoFile, tempFile);

	// skip the blank line after username info
	copy_line(infoFile, tempFile);

	// at this point, the cursor of infoFile will be in front of "# logs"
}

// source and destination should be opened when this function is called
// copy a line from the source file to the destination file
void copy_line(FILE * source, FILE * destination){
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";

	if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, source) == NULL)
		file_read_error(INFOFILE);
	fputs(fileLineBuffer, destination);
}

// source must already be open when this function is called
// datetime will be written in the format mm/dd/yyyy hh:mmxm\n
// this function was written while referencing https://www.techiedelight.com/print-current-date-and-time-in-c/
void write_current_datetime(FILE * source){ // TODO change this variable name to destination
	time_t dateAndTime;
	time(&dateAndTime);
	struct tm * timeStruct = localtime(&dateAndTime);
	int hours, minutes, day, month, year;
	hours = timeStruct->tm_hour;
	minutes = timeStruct->tm_min;
	day = timeStruct->tm_mday;
	month = timeStruct->tm_mon + 1;
	year = timeStruct->tm_year+ 1900;

	// printf("%02d/%02d/%02d ", month, day, year);
	fprintf(source, "%02d/%02d/%02d ", month, day, year);
	if(hours < 12)
		// printf("%02d:%02dam\n", hours, minutes);
		fprintf(source, "%02d:%02dam\n", hours, minutes);
	else
		fprintf(source, "%02d:%02dpm\n", hours - 12, minutes);
		// printf("%02d:%02dpm\n", hours - 12, minutes);
}
