#ifndef _COMMAND_
#define _COMMAND_

#define COMMAND_RETRIEVEALL 0
#define COMMAND_RETRIEVE	1
#define COMMAND_UPDATE		2

int parse_command(int argc , char *argv[]);

#endif /*_COMMAND_*/
