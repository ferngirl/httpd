#ifndef _HTTP_H_
#define _HTTP_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<fcntl.h>

#define SIZE 1024
#define NOTICE 0
#define WARNINIG 1
#define FATAL 2

static void usage(const char* proc);
void* handler_request(void* arg);

#endif
