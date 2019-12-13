#include <stdio.h>
#include <stdlib.h>

#define INFOFILE "info.lablog"
#define TEMPINFOFILE "info.lablog.tmp"
#define LOGFILE "log.lablog"
#define TEMPLOGFILE "log.lablog.tmp"
#define USERNAMEBUFFERSIZE 30

void open_info_file(FILE *, char[]);
void write_default_username(FILE *, char[]);
void cleanup_files(FILE *, FILE *, FILE *, FILE *);

void file_read_error(char * fileName);
void print_help_menu();
