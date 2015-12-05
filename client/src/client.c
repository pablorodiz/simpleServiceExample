/*
 ============================================================================
 Name        : client.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : client example using sockets based on example client on
  	  	  http://www.binarytides.com/server-client-example-c-sockets-linux/
 ============================================================================
 */

#include <stdio.h> 		//printf
#include <string.h>    	//strlen
#include <utils.h>
#include <communication.h>

int main(int argc , char *argv[])
{
    int sock = com_connect("127.0.0.1");

    //keep communicating with server
    while(sock>=0)
    {
    	char message[12];
        strcpy(message, "<retrieve/>");

        //Send some data
        com_send_data(sock, message, strlen(message));

        //Receive a reply from the server
        char server_reply[2000];

        if(com_receive_data(sock, server_reply, 2000)!=0)
        {
        	utils_log("recv failed");
            break;
        }
        else
        {
        	utils_log("Server reply :");
        	utils_log(server_reply);
        	break;
        }
    }

    com_disconnect(sock);
    return 0;
}
