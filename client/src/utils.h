#ifndef _UTILS_
#define _UTILS_

void *utils_malloc (int size);
void utils_free(void *ptr);
void utils_log(const char *msg);
void utils_usleep(long usec);

#endif /*_UTILS_*/
