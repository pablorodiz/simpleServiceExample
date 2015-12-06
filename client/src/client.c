/*
 ============================================================================
 Name        : client.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : Client example that can retrieve or update values to service
 ============================================================================
 */

#include <client.h>
#include <string.h>    	//strlen
#include <utils.h>
#include <communication.h>
#include <command.h>
#include <storage.h>

//Connect to server
int client_connect(char *server_id)
{
	return com_connect(SERVER_ADDRESS);
}

//Disconnect form server
void client_disconnect(int connection_id)
{
	com_disconnect(connection_id);
}

//Send message to server
int client_send_message(int connection_id, char *buffer, int buffer_size)
{
	return com_send_data(connection_id, buffer, buffer_size);
}

//Receive message from server
int client_receive_message(int connection_id, char *buffer, int buffer_size)
{
	return com_receive_data(connection_id, buffer, buffer_size);
}

//Create message for retrieve all data available
//message and message_len pointers must point to variables where to receive message data
//message must be freed after use
int client_create_retrieve_all_message(char **message, int *message_len)
{
    *message_len = RETRIEVE_ALL_LEN+1;
	*message = utils_malloc(*message_len);
	if(*message!=NULL) memcpy(*message, retrieve_all, *message_len);
	else return 1;
	return 0;
}

//Create XML message for RETRIEVE operation
//message and message_len pointers must point to variables where to receive message data
//message must be freed after use
int client_create_retrieve_message(char **msg, int *message_len, char *data[], int num_data)
{
	int i, j, retrieve_i, msg_i, key_i;
	char *message;
	//Determine message length
	*message_len = RETRIEVE_LEN + (KEY_LEN * (num_data)) + 1;
	for(i=0; i<num_data; i++) *message_len += strlen(data[i]);
	//Create message
	message = utils_malloc(*message_len);
	if(message==NULL) return 1;
	retrieve_i=0;
	msg_i=0;
	//Open parent tag
	while(retrieve[retrieve_i]!='#') {
		message[msg_i++] = retrieve[retrieve_i++];
	}
	retrieve_i++;
	//Add key tag for each requested value
	for(j=0; j<num_data; j++)
	{
		//Open key tag
		key_i=0;
		while(key[key_i]!='#') message[msg_i++] = key[key_i++];
		key_i++;
		int arg_i = 0;
		//Add text content for key tag
		while((data[j])[arg_i]!='\0')
		{
			message[msg_i++] = (data[j])[arg_i++];
		}
		//Close key tag
		while(key[key_i]!='\0')	message[msg_i++] = key[key_i++];
	}
	//Close parent tag
	while(retrieve[retrieve_i]!='\0') message[msg_i++] = retrieve[retrieve_i++];
	//End string
	message[msg_i] = '\0';
	//utils_log(message);
	*msg = message;
	return 0;
}

//Create XML message for UPDATE operation
//message and message_len pointers must point to variables where to receive message data
//message must be freed after use
//data array must contain pairs of name and value
int client_create_update_message(char **msg, int *message_len, char *data[], int num_data)
{
	int i, j, update_i, msg_i;
	char *message;
	*message_len = UPDATE_LEN + (VALUE_LEN * (num_data)) + 1;
	for(i=0; i<num_data; i+=2)
	{
		*message_len += 2 * strlen(data[i]); 	//name
		*message_len += strlen(data[i+1]);		//value
	}
	//Create message
	message = utils_malloc(*message_len);
	if (message==NULL) return 1;
	update_i=0;
	msg_i=0;
	//Open parent tag
	while(update[update_i]!='#') message[msg_i++] = update[update_i++];
	update_i++;
	//Add key tag for each requested value
	for(j=0; j<num_data; j+=2)
	{
		//Open value tag
		message[msg_i++] = '<';
		int name_i = 0;
		//Add key name
		while((data[j])[name_i]!='\0')
		{
			message[msg_i++] = (data[j])[name_i++];
		}
		//End open key
		message[msg_i++]='>';
		//Add key value
		int value_i = 0;
		while((data[j+1])[value_i]!='\0')
		{
			message[msg_i++] = (data[j+1])[value_i++];
		}
		//Start close key
		message[msg_i++] = '<';
		message[msg_i++] = '/';
		//Add key name to close key
		name_i = 0;
		while((data[j])[name_i]!='\0')
		{
			message[msg_i++] = (data[j])[name_i++];
		}
		//Close key tag
		message[msg_i++] = '>';
	}
	//Close parent tag
	while(update[update_i]!='\0') message[msg_i++] = update[update_i++];
	//End string
	message[msg_i] = '\0';
	//utils_log(message);
	*msg = message;
	return 0;
}

//Parse server answer
int client_parse_server_answer(char *server_reply, int receive_len)
{
	//utils_log("Server reply :");
	//utils_log(server_reply);
	//TODO I am not checking if answer is valid XML, just look for tag names and values
	int i=0;
	int first_tag = 0;
	int tag_start = 0;
	int value_start = 0;
	char *name=NULL, *value=NULL;
	while(i<receive_len)
	{
		//Look for tag starts
		if(server_reply[i]=='<')
		{
			tag_start = i+1;
			//Tag start is also value end if there is a previously stored value start
			if((i-value_start)>1 && value_start!=0)
			{
				char *val = utils_malloc((i-value_start)+1);
				if(val==NULL) {
					if(name!=NULL) utils_free(name);
					if(value!=NULL) utils_free(value);
					return 1;
				}
				memcpy(val, &server_reply[value_start],i-value_start);
				val[i-value_start]='\0';
				if(value==NULL) value = val;
				else utils_free(value);
				value_start = 0;
			}
		}
		//Look for tag ends
		if(server_reply[i]=='>')
		{
			//Discard root tag
			if(first_tag==1)
			{
				//Discard closing tags
				if(server_reply[tag_start]!='/')
				{
					char *tag = utils_malloc((i-tag_start)+1);
					if(tag==NULL) {
						if(name!=NULL) utils_free(name);
						if(value!=NULL) utils_free(value);
						return 1;
					}
					memcpy(tag, &server_reply[tag_start],i-tag_start);
					tag[i-tag_start]='\0';
					if(name==NULL) name = tag;
					else utils_free(tag);
					//Tag end is also the start for its value
					value_start = i+1;
				}
				else
				{
					//Closing tag, so we should have a complete key-value pair stored
					if(name!=NULL && value!=NULL)
					{
						//Store value if any
						storage_store_value(name, value);
					}
					if(name!=NULL)
					{
						utils_free(name);
						name = NULL;
					}
					if(value!=NULL)
					{
						utils_free(value);
						value = NULL;
					}
				}
			}
			else
			{
				first_tag = 1;
			}
		}
		i++;
	}
	return 0;
}


#ifdef TEST
int main(int argc , char *argv[])
{
	int command = parse_command(argc, argv);

	do
	{
		char *message=NULL;
		int message_len=0;

		if(command==COMMAND_UPDATE) utils_log("Send update");
		//Create client messages
		switch (command)
		{
			case COMMAND_RETRIEVEALL: //Create XML message for RETRIEVE all values operation
				client_create_retrieve_all_message(&message, &message_len);
				break;
			case COMMAND_RETRIEVE: //Create XML message for RETRIEVE operation
				//TODO requested value names get from command line. In real case should be hard-coded
				client_create_retrieve_message(&message, &message_len, &argv[2], argc-2);
				break;
			case COMMAND_UPDATE: //Create XML message for UPDATE operation
				//TODO updated values and names get from command line. In real case names should be hard-coded and values read from sensors
				client_create_update_message(&message, &message_len, &argv[2], argc-2);
				break;
			default:
				return 1;
		}
		if(message!=NULL)
		{
			//Connect to server
			int conn_id = client_connect(SERVER_ADDRESS);

			//Communicate with server
			if(conn_id>=0)
			{
				//Send message to server
				client_send_message(conn_id, message, message_len);
				utils_free(message);

				//Receive the reply from the server if any
				if(command!=COMMAND_UPDATE)
				{
					//TODO Fixed size receive buffer. Might give problems with long answers
					char *server_reply = utils_malloc(RECEIVE_BUFFER_SIZE);
					if(server_reply!=NULL)
					{
						int receive_len = client_receive_message(conn_id, server_reply, RECEIVE_BUFFER_SIZE);
						if(receive_len < 0)
						{
							utils_log("recv failed");
						}
						else
						{
							client_parse_server_answer(server_reply, receive_len);
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

			client_disconnect(conn_id);
			if(command!=COMMAND_UPDATE) return 0;
		}
		if(command!=COMMAND_UPDATE) return 1;
		else
		{
			utils_log("Go to sleep 15 minutes");
			utils_usleep(UPDATE_TIME);
		}
	} while (command==COMMAND_UPDATE);
	return 0;
}
#endif /*TEST*/
