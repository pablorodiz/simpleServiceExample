/*
 ============================================================================
 Name        : utils.c
 Author      : Pablo Rodiz Obaya
 Version     : 1.0
 Description : Utility functions to hide system calls and make easy porting
 	 	 	the client software to embedded platforms
 ============================================================================
 */

#include <utils.h>
#include <stdio.h> 		//puts
#include <stdlib.h>    	//malloc
#include <sys/types.h>

void *utils_malloc (int size)
{
	return malloc(size);
}

void utils_free(void *ptr)
{
	free(ptr);
}

void utils_log(const char *msg)
{
	puts(msg);
}
