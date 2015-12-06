/*
 ============================================================================
 Name        : storage.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : Mock-up of storage service. It just receive values and print
  	  	  them to standard output and returns always NULL when a value is
  	  	  requested
 ============================================================================
 */

#include <storage.h>
#include <stdio.h>		//TODO printf, used only to mock up storage through printing to standard output

int storage_store_value (char *name, char *value) {
	printf("%s=%s\n", name, value);
	return 0;
}

char *storage_retrieve_value (char *name) {
	return NULL;
}
