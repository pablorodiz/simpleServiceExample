/*
 ============================================================================
 Name        : database.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : Simple database for values using a text file. Biggest problem
  	  	  here is it must be thread safe. Code based on example here:
  	  	  http://stackoverflow.com/questions/14903555/c-thread-safe-logging-to-a-file
  	  	  Added flush before releasing the lock on the file to avoid problem
  	  	  pointed in the comments to the example, and removed duplicate file
  	  	  pointers.
 ============================================================================
 */

//TODO - Database file can grow to infinite size. Use a more realistic database system

#include <database.h>
#include <stdio.h>	//Files
#include <stdlib.h>	//Files
#include <unistd.h>	//usleep
#include <string.h> //strlen
#include <errno.h> 	//Error numbers
#include <time.h>	//time

//Check if database file exists
int database_check_if_file_exist(void)
{
	FILE *fp;
	fp = fopen(DATABASE_FILE, "wx");
	if(fp) {
		perror("Database file does not exist");
		fclose(fp);
		remove(DATABASE_FILE);
		return 1;
	}
	else return 0;
}

//Lock the database file (with timeout)
int database_lock_file(FILE *fp)
{
	int error, timeout;

	error = lockf(fileno(fp), F_TLOCK, 0);
	while( error == EACCES || error == EAGAIN)
	{
	  //sleep for a bit
	  usleep(LOCKED_FILE_RETRY_TIME);

	  //Increment timeout
	  timeout += LOCKED_FILE_RETRY_TIME;

	  //Check for time out
	  if(timeout > LOCKED_FILE_TIMEOUT)
	  {
		 perror("Unable to get a lock on database file");
	     return 1;
	  }

	  //Retry the lock operation
	  error = lockf(fileno(fp), F_TLOCK, 0);
	}
	return 0;
}

//Unlock the database file
void database_unlock_file(FILE *fp)
{
	lockf(fileno(fp), F_ULOCK, 0);
}

//Add a key - value pair to the database
int database_store_value(char *name, char* value)
{
	FILE *data;

	//Get current time for data time stamp
	char text_date[100];
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(text_date, sizeof(text_date)-1, "%d%m%Y-%H:%M:%S ", t);

	//Create new data entry
	int new_data_len = strlen(text_date)+strlen(name)+strlen(value)+3;
	char new_data[new_data_len];
	strncpy(&new_data[0], text_date, strlen(text_date));
	strncpy(&new_data[strlen(text_date)], name, strlen(name));
	new_data[strlen(text_date)+strlen(name)]=' ';
	strncpy(&new_data[strlen(text_date)+strlen(name)+1], value, strlen(value));
	new_data[new_data_len-2]='\n';
	new_data[new_data_len-1]='\0';

	//make file (fails if file exists)
	data = fopen(DATABASE_FILE, "wx");

	//Close the file if a new one was created
	if(data)
	   fclose(data);

	//Open file in update mode (it must exist for this)
	data = fopen(DATABASE_FILE, "rb+");

	//Lock file
	if (database_lock_file(data)!=0) return 1;

	//Print data entry
	fseek(data, 0, SEEK_END);
	fwrite((const void*) new_data, 1, new_data_len-1, data);

	//Unlock the file
	fflush(data);
	database_unlock_file(data);

	//Close file
	fclose(data);
	return 0;
}

//Retrieve the last value of the element with the given name
int database_retrieve_value(char *name, char* value, unsigned int size)
{

	FILE *data;

	//Check if database file exists
	if(database_check_if_file_exist()!=0) return 1;

	//Open file in read mode
	data = fopen(DATABASE_FILE, "r");

	//Lock file
	if (database_lock_file(data)!=0) return 1;

	//Search for the last occurrence of the requested value
	char last_value[size];
	last_value[0] = '\0';
	while (1)
	{
		//Read one line of the file
		//TODO Fixed buffer size can be too small
		char buffer[256];
		if (!fgets(buffer, sizeof buffer, data)) break;
		fseek(data, 1, SEEK_CUR);

		//Parse line content
		//TODO Fixed buffer overflow problem when read strings are bigger than corresponding buffer
		char date[100], key[100], val[size];
		sscanf(buffer, "%s %s %s", date, key, val);

		//Check if key name is the same we are looking for, and if so update value
		if(key!=NULL && strcmp(key, name)==0 && strlen(val)<size) strcpy(last_value, val);
	}

	//Unlock the file
	database_unlock_file(data);

	//Close file
	fclose(data);

	//Return found value if correct
	if(strcmp(last_value, "")==0 || strlen(last_value)>size) return 1;
	strcpy(value, last_value);
	return 0;
}

//Search for name value in list and return pointer to it
t_pair database_check_name_in_list(t_pair list, char *name)
{
	if(list==NULL) return NULL;
	if(strcmp(list->name, name)==0) return list;
	return database_check_name_in_list(list->next, name);
}

//Retrieve all keys in the database with their corresponding value
t_pair database_retrieve_all(void)
{

	FILE *data;

	//Check if database file exists
	if(database_check_if_file_exist()!=0) return (t_pair)NULL;

	//Open file in read mode
	data = fopen(DATABASE_FILE, "r");

	//Lock file
	if (database_lock_file(data)!=0) return (t_pair)NULL;

	//Search for the last occurrence of every value
	t_pair head=NULL,tail=NULL;
	while (1)
	{
		//Read one line
		//TODO Fixed buffer size can be too small
		char buffer[256];
		if (!fgets(buffer, sizeof buffer, data)) break;
		fseek(data, 1, SEEK_CUR);

		//Parse line content
		//TODO Fixed buffer overflow problem when read strings are bigger than corresponding buffer
		char date[100], key[100], val[100];
		sscanf(buffer, "%s %s %s", date, key, val);

		//Check if element is already in the list
		t_pair exists = NULL;
		t_pair new = NULL;
		exists = database_check_name_in_list(head, key);
		if(exists!=NULL){
			//If key already in list update value
			free(exists->value);
			exists->value = malloc(strlen(val)+1);
			strcpy(exists->value, val);
		}
		else
		{
			//If key not in the list create new node for it
			new = (t_pair) malloc(sizeof(struct pair));
			new->name = malloc(strlen(key)+1);
			strcpy(new->name, key);
			new->value = malloc(strlen(val)+1);
			strcpy(new->value, val);
			new->next = NULL;
			if(head==NULL)
			{
				head = new;
				tail = head;
			}
			else
			{
				tail->next = new;
				tail = new;
			}
		}
	}

	//Unlock the file
	database_unlock_file(data);

	//Close file
	fclose(data);

	//Return found values list
	return head;
}

//Free memory used by elements list
void database_retrieve_free(t_pair list)
{
	if(list->next!=NULL) database_retrieve_free(list->next);
	free(list->name);
	free(list->value);
	free(list);
}





