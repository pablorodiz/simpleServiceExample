
#ifndef _DATABASE_
#define _DATABASE_

#define DATABASE_FILE "database.txt"

#define LOCKED_FILE_RETRY_TIME 	250000
#define LOCKED_FILE_TIMEOUT		1500000

typedef struct pair *t_pair;
struct pair {
	char *name;     //Key name
	char *value;    //Value of the key
	t_pair next;	//Next list element
};

int database_store_value(char *name, char* value);
int database_retrieve_value(char *name, char* value, unsigned int size);
t_pair database_retrieve_all(void);
void database_retrieve_free(t_pair list);

#endif /*_DATABASE_*/
