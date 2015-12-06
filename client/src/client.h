#ifndef _CLIENT_
#define _CLIENT_

#define TEST

#define RECEIVE_BUFFER_SIZE	2000

#define SERVER_ADDRESS	"127.0.0.1"

//Update every 15 minutes
#define UPDATE_TIME	900000000

//Constant XML prototype strings
const char retrieve_all[] = "<retrieve/>";
const char retrieve[] = "<retrieve>#</retrieve>";
const char key[] = "<key>#</key>";
const char update[] = "<update>#</update>";
//const char value[] = "<#>*</#>";

//XML prototype lengths
#define RETRIEVE_ALL_LEN 	11
#define RETRIEVE_LEN		21
#define KEY_LEN				11
#define UPDATE_LEN			17
#define VALUE_LEN			5

int client_connect(char *server_id);
void client_disconnect(int connection_id);
int client_create_retrieve_all_message(char **message, int *message_len);
int client_create_retrieve_message(char **msg, int *message_len, char *data[], int num_data);
int client_create_update_message(char **msg, int *message_len, char *data[], int num_data);
int client_send_message(int connection_id, char *buffer, int buffer_size);
int client_receive_message(int connection_id, char *buffer, int buffer_size);

#endif /*_CLIENT_*/
