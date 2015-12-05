#ifndef _COMMUNICATION_
#define _COMMUNICATION_

#define COMMUNICATION_PORT 6423

//Connect to server
int com_connect(const char *ip_address);

//Send data to server
int com_send_data(int sock, const char *msg, int msg_len);

//Receive a data from the server
int com_receive_data(int sock, char *buffer, int buffer_len);

//Disconnect from server
void com_disconnect(int sock);

#endif /*_COMMUNICATION_*/
