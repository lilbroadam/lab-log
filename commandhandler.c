#include "lablog.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

void write_drive_info(FILE *, char **, int);
void elapsed_time(char *, char *, char *);
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

// TODO reject request when not logged in
// TODO handle trying to drive a user who isn't logged in
// TODO remove multiple drivers, change it to only 1 driver
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

// export the data from the given source file (info.lablog) to <outputFileName>.txt
// the given source file should already be opened when this function is called
void export(FILE * source, char * outputFileName){
	if(access(outputFileName, F_OK) == false){ // see if the given file name already exists
		printf("The given file already exists. Do you want to overwrite it? (Y/N): ");
		char promptBuffer[2] = "\0";
		fgets(promptBuffer, 2, stdin);

		if(strcmp("Y", promptBuffer) == 0 || strcmp("y", promptBuffer) == 0){
			// TODO
		}else if(strcmp("N", promptBuffer) == 0 || strcmp("n", promptBuffer) == 0){
			// TODO
		}else{
			printf("Please enter Y or N\n");
			// TODO loop
		}
	}

	// TODO add .txt to end of outputFileName
	FILE * outputFile = fopen(outputFileName, "w+"); // w+ to start writing the file from scratch
	// TODO check if file pointer points to null

	skip_username_info(source, NULL);

	char * token;
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";
	char startTime[FILELINEBUFFERSIZE] = "\0";
	char endTime[FILELINEBUFFERSIZE] = "\0";
	char elapsedTime[FILELINEBUFFERSIZE] = "\0";
	fgets(fileLineBuffer, FILELINEBUFFERSIZE, source); // read the "# logs" comment
	while(fgets(fileLineBuffer, FILELINEBUFFERSIZE, source) != NULL){ // loop through each session
		// print start date and time
		strcpy(startTime, fileLineBuffer);
		token = strtok(fileLineBuffer, " \n"); // TODO make this delimiter a constant
		fprintf(outputFile, "%s, ", token);
		token = strtok(NULL, " \n");
		fprintf(outputFile, "%s - ", token);

		// print end time
		fgets(fileLineBuffer, FILELINEBUFFERSIZE, source);
		strcpy(endTime, fileLineBuffer);
		token = strtok(fileLineBuffer, " \n"); // skip end date
		token = strtok(NULL, " \n");
		fprintf(outputFile, "%s ", token);

		// print loop through logged in users for this session // TODO this doesn't print well with 2 or less users
		fgets(fileLineBuffer, FILELINEBUFFERSIZE, source);
		token = strtok(fileLineBuffer, " \n");
		int numUsers = atoi(token);
		for(int i = 0; i < numUsers - 1; i++){
			token = strtok(NULL, " \n");
			fprintf(outputFile, "%s, ", token);
		}
		token = strtok(NULL, " \n");
		fprintf(outputFile, "and %s, ", token);

		elapsed_time(startTime, endTime, elapsedTime);
		fprintf(outputFile, "%s\n", elapsedTime); // print elapsed time for current session

		fprintf(outputFile, "Driver order and time length:\n");
		fgets(fileLineBuffer, FILELINEBUFFERSIZE, source); // get driver name
		while(strcmp(fileLineBuffer, "\n") != 0){ // loop through drivers in the current session
			token = strtok(fileLineBuffer, " \n");
			fprintf(outputFile, "%s, ", token);


			fgets(fileLineBuffer, FILELINEBUFFERSIZE, source); // get drive start time
			strcpy(startTime, fileLineBuffer);

			fgets(fileLineBuffer, FILELINEBUFFERSIZE, source); // get drive end time
			strcpy(endTime, fileLineBuffer);

			elapsed_time(startTime, endTime, elapsedTime);
			fprintf(outputFile, "%s\n", elapsedTime);  // print elapsed time for current driver

			fgets(fileLineBuffer, FILELINEBUFFERSIZE, source); // get driver name
		}

		fprintf(outputFile, "\n");
	}

	fclose(outputFile);
}

// this function written with Mara Manskie
// given a start and end time in the format mm/dd/yyyy hh:mmxm, store the elasped
// time as a string without a new line character at the end in destination
void elapsed_time(char * start, char * end, char * destination){
	// printf("Start: %sEnd: %s", start, end);

	// if no end time is provided, elapsed time is null
	if(strcmp(end, "_\n") == 0){ // TODO change this symbol to a constant
		strcpy(destination, "(null)");
		printf("Warning: trying to export while still logged in, unable to calculate some elapsed times\n");
		return;
	}

	int startHour, startMinute, endHour, endMinute, i = 0;
 	int startMonth, startDay, startYear, endMonth, endDay, endYear = 0;// , endHour, endMinute;
	int totalHours, totalMinutes = 0;
	int tempStart, tempEnd, temp = 0;
	char res[10], tempString[4];
	float decimals;

	startHour = atoi(start + 11);
	startMinute = atoi(start + 14);
	startMonth = atoi(start);
	startDay = atoi(start + 3);
	startYear = atoi(start + 6);

	endHour = atoi(end + 11);
	endMinute = atoi(end + 14);
	endMonth = atoi(end);
	endDay = atoi(end + 3);
	endYear = atoi(end + 6);

	if(strcmp(start + 16, "pm\n") == 0 && startHour != 12)
    	startHour = startHour + 12;
	if(strcmp(start + 16, "am\n") == 0 && startHour == 12)
		startHour = 0;
	if(strcmp(end + 16, "pm\n") == 0 && endHour != 12)
    	endHour = endHour + 12;
	if(strcmp(end + 16, "am\n") == 0 && endHour == 12)
		endHour = 0;

  	if(endHour >= startHour){
      	totalHours = endHour - startHour;
  	}
	//edge case for if start and end times are 12 hours apart
	else if(startHour - endHour == 12 || endHour - startHour == 12){
		totalHours = (24 - startHour) + endHour;
	}
	else if (startHour > endHour){
		//totalHours = abs(startHour - endHour);
		totalHours = ((24 - startHour) + endHour - 1);
	}

	if(endHour > startHour && startMinute > endMinute)
		totalHours--;

	tempStart = 60 - startMinute;
	tempEnd = 60 - endMinute;

	if(tempStart >= tempEnd){
		totalMinutes = tempStart - tempEnd;
	}
	else if (tempEnd > tempStart){
		totalMinutes = 60 - (tempEnd - tempStart);
	}

	// printf("Total Hours: %d\n", totalHours);
	// printf("Total Min: %d\n\n", totalMinutes);

	if(totalHours > 0){
		sprintf(destination, "%.2f hours", totalHours + totalMinutes / 60.0);
	} else {
		sprintf(destination, "%d minutes", totalMinutes);
	}

}

void config(){

}

// the given infoFile and tempFile should already be open at the start of this function
// this function will copy all the username info from infoFile into tempFile, including
// the following blank line. The infoFile's cursor will be in front of "# logs" at the end
// of this function. If tempFile is null, the data from infoFile won't be copied to tempFile
// TODO make this function cleaner
void skip_username_info(FILE * infoFile, FILE * tempFile){
	char fileLineBuffer[FILELINEBUFFERSIZE] = "\0";

	if(tempFile != NULL){
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

	}else{
		// skip the usernames comment line in the info file
		if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
			file_read_error(INFOFILE);

		// get the number of usernames from the info file
		if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
			file_read_error(INFOFILE);
		int numUsernames = atoi(fileLineBuffer);

		// get the usernames from the info file and place them into an array to return
		for(int i = 0; i < numUsernames; i++)
			// read the next username
			if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
				file_read_error(INFOFILE);

		// skip the blank line after username info
		if(fgets(fileLineBuffer, FILELINEBUFFERSIZE, infoFile) == NULL)
			file_read_error(INFOFILE);
	}

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
	if(hours < 12){
		// printf("%02d:%02dam\n", hours, minutes);
		if(hours == 0)
			hours = 12;
		fprintf(source, "%02d:%02dam\n", hours, minutes);
	}else{
		if(hours == 12)
			hours += 12;
		fprintf(source, "%02d:%02dpm\n", hours - 12, minutes);
	}
}
