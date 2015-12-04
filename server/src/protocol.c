/*
 ============================================================================
 Name        : protocol.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : Simple protocol for updating and retrieving data based on XML
 	 	 messages.
 ============================================================================
 */

#include <protocol.h>
#include <stdio.h>
#include <string.h>    	//strlen
#include <stdlib.h>    	//strlen
#include <unistd.h>    	//write
#include <ezxml.h>		//XML parsing
#include <database.h>	//Database functions

int protocol_decode_message(char *msg, int msg_len, int sock)
{
	ezxml_t parsed_xml = ezxml_parse_str(msg, msg_len);
	if(parsed_xml!=NULL)
	{
		if(ezxml_name(parsed_xml)!=NULL)
		{
			//Update message
			if(strcmp(ezxml_name(parsed_xml), "update")==0)
			{
				ezxml_t child = parsed_xml->child;
				while(child!=NULL)
				{
					//Valid key value pair found, store in the database
					printf("Key: %s - Value: %s\n", child->name, child->txt);
					if(database_store_value(child->name, child->txt)!=0) perror("Impossible to store value");
					child = child->sibling;
				}
			}
			//Retrieve message
			else if(strcmp(ezxml_name(parsed_xml), "retrieve")==0)
			{
				ezxml_t child = ezxml_child(parsed_xml, "key");
				t_pair list = NULL;
				if(child==NULL)
				{
					//Request all values from database
					list = database_retrieve_all();
					t_pair v = list;
					while (v!=NULL)
					{
						printf("Retrieved Key: %s - Value: %s\n", v->name, v->value);
						v=v->next;
					}
				}
				else while(child!=NULL)
				{
					//Request current value from database
					char value[100];
					if(strcmp(child->name, "key")!=0 || database_retrieve_value(child->txt, value, sizeof value)!=0) perror("Impossible to retrieve value for key");
					else
					{
						printf("Retrieved Key: %s - Value: %s\n", child->txt, value);
					}

					child = child->next;
				}

				//TODO Create xml response

				//free structure and list space if any
				if(list!=NULL) database_retrieve_free(list);

				//TODO prettify xml string

				//TODO Send the response back to the client

			}
			else puts("Wrong input, unknown operation");
		}
		else puts("Wrong input, operation not valid");
		ezxml_free(parsed_xml);
	}
	else
	{
		puts("Wrong input, not valid XML");
	}
	return 0;
}
