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
					//printf("Key: %s - Value: %s\n", child->name, child->txt);
					if(database_store_value(child->name, child->txt)!=0) perror("Impossible to store value");
					child = child->sibling;
				}
			}
			//Retrieve message
			else if(strcmp(ezxml_name(parsed_xml), "retrieve")==0)
			{
				ezxml_t response = ezxml_new("status");
				ezxml_t child = ezxml_child(parsed_xml, "key");
				t_pair list = NULL;
				if(child==NULL)
				{
					//Request all values from database
					list = database_retrieve_all();
					if(list!=NULL)
					{
						t_pair v = list;
						while (v!=NULL)
						{
							//Add node to XML structure
							//printf("Retrieved Key: %s - Value: %s\n", v->name, v->value);
							ezxml_t new_response_child = ezxml_add_child_d(response, v->name, 6);
							new_response_child = ezxml_set_txt_d(new_response_child, v->value);
							v = v->next;
						}
						database_retrieve_free(list);
					}
				}
				else while(child!=NULL)
				{
					//Request current value from database
					char value[100];
					if(strcmp(child->name, "key")!=0 || database_retrieve_value(child->txt, value, sizeof value)!=0) perror("Impossible to retrieve value for key");
					else
					{
						//Add node to XML structure
						//printf("Retrieved Key: %s - Value: %s\n", child->txt, value);
						ezxml_t new_response_child = ezxml_add_child_d(response, child->txt, 6);
						new_response_child = ezxml_set_txt_d(new_response_child, value);
					}
					child = child->next;
				}

				//Create xml string and free structure
				char *xml_resp = ezxml_toxml(response);
				ezxml_free(response);

				//Send the response back to the client
		        write(sock , xml_resp , strlen(xml_resp));

		        //Free xml string space
				free(xml_resp);
			}
			else
			{
				perror("Wrong input, unknown operation");
				//Free XML parsed structure
				ezxml_free(parsed_xml);
				//Return error
				return 1;
			}
		}
		else
		{
			perror("Wrong input, operation not valid");
			//Free XML parsed structure
			ezxml_free(parsed_xml);
			//Return error
			return 1;
		}
		//Free XML parsed structure
		ezxml_free(parsed_xml);
	}
	else
	{
		perror("Wrong input, not valid XML");
		//Return error
		return 1;
	}
	return 0;
}
