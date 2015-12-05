#include <command.h>
#include <string.h>
#include <stdio.h>

//TODO parse keys to retrieve or update. For simplicity here we only check base command
//     and correctness of the number off parameters. Client takes key names and values
//     directly from argc and argv.

int parse_command(int argc , char *argv[])
{
	if(argc<2)
	{
		puts("Error, no command provided. Please use: client {retrieve|update} {[key_name]...|key_name key_value [key_name key_value]...}");
	}
	else
	{
		if(strcmp(argv[1], "retrieve")==0)
		{
			if(argc==2) return COMMAND_RETRIEVEALL;
			else {
				return COMMAND_RETRIEVE;
			}
		}
		else if(strcmp(argv[1], "update")==0)
		{
			if(argc==2) return -1;
			else {
				if(argc%2==0) return COMMAND_UPDATE;
				puts("Incorrect number of parameters. Please use: client update key_name kay_value [key_name kay_value]...");
				return -1;
			}
		} else puts("Error, wrong command provided. Please use: client {retrieve|update} {[key_name]...|key_name key_value [key_name key_value]...}");
	}
	return -1;
}
