/*
 ============================================================================
 Name        : client.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : client example using sockets based on example client on
  	  	  http://www.binarytides.com/server-client-example-c-sockets-linux/
 ============================================================================
 */

//#include <stdio.h> 		//printf
#include <string.h>    	//strlen
#include <utils.h>
#include <communication.h>
#include <command.h>

#define RECEIVE_BUFFER_SIZE	2000

//Constant XML prototype strings
const char retrieve_all[] = "<retrieve/>";
const char retrieve[] = "<retrieve>#</retrieve>";
const char key[] = "<key>#</key>";
const char update[] = "<update>#</update>";
const char value[] = "<#>*</#>";
//XML prototypes lenghts
#define RETRIEVE_ALL_LEN 	11
#define RETRIEVE_LEN		21
#define KEY_LEN				11
#define UPDATE_LEN			17
#define VALUE_LEN			5

int main(int argc , char *argv[])
{
	int command = parse_command(argc, argv);

	char *message=NULL;
	int message_len=0;

	//Create client messages
	switch (command)
	{
		case COMMAND_RETRIEVEALL:
	        message_len = RETRIEVE_ALL_LEN+1;
			message = utils_malloc(message_len);
			if(message!=NULL) memcpy(message, retrieve_all, message_len);
			break;
		case COMMAND_RETRIEVE:
			//Determine message length
			message_len = RETRIEVE_LEN + (KEY_LEN * (argc-2)) + 1;
			int i;
			for(i=2; i<argc; i++) message_len += strlen(argv[i]);
			//Create message
			message = utils_malloc(message_len);
			int retrieve_i=0;
			int msg_i;
			//Open parent tag
			while(retrieve[retrieve_i]!='#')
			{
				message[msg_i++] = retrieve[retrieve_i++];
			}
			retrieve_i++;
			int j;
			//Add key tag for each requested value
			for(j=2; j<argc; j++)
			{
				//Open key tag
				int key_i=0;
				while(key[key_i]!='#')
				{
					message[msg_i++] = key[key_i++];
				}
				key_i++;
				int arg_i = 0;
				//Add text content for key tag
				while((argv[j])[arg_i]!='\0')
				{
					//TODO requested value names get from command line. In real case should be hard-coded
					message[msg_i++] = (argv[j])[arg_i++];
				}
				//Close key tag
				while(key[key_i]!='\0')
				{
					message[msg_i++] = key[key_i++];
				}
			}
			//Close parent tag
			while(retrieve[retrieve_i]!='\0')
			{
				message[msg_i++] = retrieve[retrieve_i++];
			}
			//End string
			message[msg_i] = '\0';
			//utils_log(message);
			break;
		case COMMAND_UPDATE:
			break;
		default:
			return 1;
	}
	if(message!=NULL)
	{
		//Connect to server
		int sock = com_connect("127.0.0.1");

		//Communicate with server
		if(sock>=0)
		{
			//Send some data
			com_send_data(sock, message, message_len);
			utils_free(message);

			//Receive the reply from the server
			if(command!=COMMAND_UPDATE)
			{
				char *server_reply = utils_malloc(RECEIVE_BUFFER_SIZE);
				if(server_reply!=NULL)
				{
					if(com_receive_data(sock, server_reply, RECEIVE_BUFFER_SIZE)!=0)
					{
						utils_log("recv failed");
					}
					else
					{
						utils_log("Server reply :");
						utils_log(server_reply);
					}
				}
				else
				{
					utils_log("Out of memory");
					return 1;
				}
			}
		}
		else utils_free(message);

		com_disconnect(sock);
	    return 0;
	}
    return 1;
}
