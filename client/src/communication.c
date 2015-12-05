/*
 ============================================================================
 Name        : communication.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : Communication functions to hide system implementation details
  	  	  and make easy porting	the client software to embedded platforms.
  	  	  Implementation based on example client on
  	  	  http://www.binarytides.com/server-client-example-c-sockets-linux/
 ============================================================================
 */

#include <communication.h>
#include <stdio.h> 		//printf
#include <string.h>    	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h> 	//inet_addr
#include <utils.h>

//Connect to server
int com_connect(const char *ip_address)
{
	int sock;
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
    	utils_log("Could not create socket");
    	return -1;
    }

    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_family = AF_INET;
    server.sin_port = htons( COMMUNICATION_PORT );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
    	utils_log("connect failed");
        return -1;
    }
    return sock;
}

//Send data to server
int com_send_data(int sock, const char *msg, int msg_len)
{
    if( send(sock , msg , msg_len , 0) < 0)
    {
      	utils_log("Send failed");
      	return 1;
    }
    return 0;
}

//Receive a data from the server
int com_receive_data(int sock, char *buffer, int buffer_len)
{
    if( recv(sock , buffer , buffer_len , 0) < 0)
    {
       	utils_log("receive failed");
        return 1;
    }
    else
    {
    	return 0;
    }
}

//Disconnect from server
void com_disconnect(int sock)
{
    shutdown(sock, 2);
}

