#include "lablog.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void skip_username_info(FILE *, FILE *);
void copy_line(FILE *, FILE *);
void write_current_datetime(FILE *);

// TODO change to login multiple usernames at once
// the given infoFile and tempFile should already be open when this function is called
// the given usernames to login should already be confirmed to be registeredUsernames when this function is called
void login(FILE * infoFile, FILE * tempFile, char * loginUsernames[], int numLoginUsernames,
		char * registeredUsernames[], int numRegisteredUsernames){
	// TODO place holder to make sure function is being called; delete this print later
	printf("logging in: ");
	for(int i = 0; i < numLoginUsernames; i++)
		printf("%s, ", loginUsernames[i]);
	printf("\n");

	// forward past the username info
	skip_username_info(infoFile, tempFile);

	// read the "# logs" comment line in the info file
	copy_line(infoFile, tempFile);

	// for each session logged,
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";
	// if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
	// 	file_read_error(INFOFILE);
	while(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) != NULL){ // while next line isn't blank // TODO this might need to be while the next 2 lines isn't blank
		fputs(fileLineBuffer, tempFile);
		// check to make sure the current session isn't already logged in
		// check that there is an end time
		copy_line(infoFile, tempFile); // read end time
		copy_line(infoFile, tempFile); // read users in session

		// if not, print that the session must be logged out first and exit program

		fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile);
		while(strcmp(fileLineBuffer, "\n") != 0){
			fputs(fileLineBuffer, tempFile);
			copy_line(infoFile, tempFile);
			copy_line(infoFile, tempFile);

			fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile);
		}
		fputs(fileLineBuffer, tempFile);
	}

	// find the next empty line and go to the line after it

	// write the log in info for this session to the tempFile
	write_current_datetime(tempFile); // <date> <time>
	fputs("_\n", tempFile); // write a "_" while lab session hasn't ended
	fprintf(tempFile, "%d ", numLoginUsernames);
	for(int i = 0; i < numLoginUsernames; i++) // this prints an extra space after the last username
		fprintf(tempFile, "%s ", loginUsernames[i]); // but it shouldn't matter
	fputs("\n", tempFile);
	fputs("\n", tempFile);


}
void logout(){

}

void drive(){

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
void write_current_datetime(FILE * source){
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
