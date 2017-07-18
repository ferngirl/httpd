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
#include<unistd.h>
#include<sys/stat.h>
#include<strings.h>
#include<sys/sendfile.h>

#define SIZE 1024
#define NOTICE 1
#define WARNINIG 2
#define FATAL 3

void* handler_request(void* arg);
void echo_error(int fd,int num_error);
int echo_www(int fd,const char* path,int size);
void echo_error(int fd,int num_error);
void printf_log(const char* msg,int level);
int exe_cgi(int fd,const char* method,\
		     const char* path,const char* query_string);
int startup(const char* ip,int port);

#endif
