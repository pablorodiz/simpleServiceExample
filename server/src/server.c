/*
 ============================================================================
 Name        : server.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : Server handles multiple clients using threads
    based on example on https://gist.github.com/silv3rm00n/5821760
	Solved race condition by adding malloc'ed buffer for sock
	descriptor and memory allocation problem by adding pthread_detach,
	as explained in the comments to the original example
 ============================================================================
 */

#include <stdio.h>
#include <string.h>    	//strlen
#include <stdlib.h>    	//strlen
#include <sys/socket.h>
#include <arpa/inet.h> 	//inet_addr
#include <pthread.h> 	//for threading , link with lpthread
#include <protocol.h>

//TODO Service starts and never closes. It would be nice to manage Linux signals for correct termination

//the thread function
void *connection_handler(void *);

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        perror("Error, server could not create socket");
        return 1;
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 6423 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Error, bind to interface and port failed");
        return 1;
    }

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Server ready. Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
    	int *sock_buffer;
    	sock_buffer = malloc(1);
    	*sock_buffer =  client_sock;

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) sock_buffer) < 0)
        {
        	free(sock_buffer);
            perror("Error. Server could not create thread for incoming connection");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);

        //Make sure thread memory is freed when connection is closed
        pthread_detach(thread_id);
        puts("New client connected");
    }

    if (client_sock < 0)
    {
        perror("Error. Server accept failed");
        return 1;
    }

    return 0;
}

/*
 * This function will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    //Free socket descriptor buffer
    free(socket_desc);
    int read_size;
    char client_message[2000];

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Add end of string marker
		client_message[read_size] = '\0';


		puts("Data received:");
		puts(client_message);

		//Call protocol handler to decode message and act properly
		int res = protocol_decode_message(client_message, read_size, sock);

		//clear the message buffer
		memset(client_message, 0, 2000);
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
}
