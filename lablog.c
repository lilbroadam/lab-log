#include "lablog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char* argv[]){
	if(argc < 2){
		printHelpMenu();
		exit(0);
	}


	if(strcmp("login", argv[1]) == 0){
		printf("login\n");
	}else if(strcmp("logout", argv[1]) == 0){
		printf("logout\n");
	}else if(strcmp("break", argv[1]) == 0){
		printf("break\n");
	}else if(strcmp("print", argv[1]) == 0){
		printf("print\n");
	}else if(strcmp("report", argv[1]) == 0){
		printf("report\n");
	}else if(strcmp("config", argv[1]) == 0){
		printf("config\n");
	}else if(strcmp("-h", argv[1]) == 0){

	}else{

	}

	// char *optarg;
	// int optind;
	// while(argc, argv){
	//
	// }
}
