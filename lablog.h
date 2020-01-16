#include <stdio.h>
#include <stdlib.h>

#define INFOFILE "info.lablog"
#define TEMPINFOFILE "info.lablog.tmp"
#define LOGFILE "log.lablog"
#define TEMPLOGFILE "log.lablog.tmp"
#define USERNAMEBUFFERSIZE 30

char ** get_usernames(int *);
void write_default_username(FILE *, char[]);
void cleanup_files(FILE *, FILE *, FILE *, FILE *);

void info_file_not_found();
void file_read_error(char * fileName);
void print_help_menu();
