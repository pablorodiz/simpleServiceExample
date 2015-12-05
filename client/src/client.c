/*
 ============================================================================
 Name        : client.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : client example using sockets based on example client on
  	  	  http://www.binarytides.com/server-client-example-c-sockets-linux/
 ============================================================================
 */

#include<stdio.h> 		//printf
#include<string.h>    	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h> 	//inet_addr

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
    	perror("Could not create socket");
    	return 1;
    }
    //puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 6423 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    //puts("Connected\n");

    //keep communicating with server
    while(1)
    {
        strcpy(message, "<retrieve/>");

        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
        else
        {
        	puts("Server reply :");
        	puts(server_reply);
        	break;
        }
    }

    shutdown(sock, 2);
    return 0;
}
