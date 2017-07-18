#include"http.h"

int startup(const char* ip,int port)//create listen_socket-bind-listen
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
		return -1;
	int op=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&op,sizeof(op));
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
		return -2;
	if(listen(sock,10)<0)
		return -3;
	return sock;
}

static int get_line(int fd,char* buf,int len)//get a line message  unify CELF"\r\n"--->'\n'
{
	char c='\0';
	int i=0;
	while(c!='\n' && i<len-1)
	{
		ssize_t s =  recv(fd,&c,1,0);
		if(s>0){
			if(c=='\r'){//\r \r\n  \rabc
				recv(fd,&c,1,MSG_PEEK);
				if(c=='\n'){
					recv(fd,&c,1,0);//\r\n--->\n
				}else{
					c='\n';//\r \rabc--->\n
				}
			}
			buf[i++] = c;
		}
	}
	buf[i] = '\0';
	return i;
}

void printf_log(const char* msg,int level)//print log message  wo should print message to log
{
	const char* level_msg[] = {
	    	"NOTICE",\
			"WARNING",\
			"FATAL"
	};
	printf("{%s}{%s}\n",msg,level_msg[level-1]);//need exchange
}

void echo_error(int fd,int num_error)
{

}

void drop_header(int fd)
{
	char buff[SIZE];
	int ret=-1;
	do{
	ret = get_line(fd,buff,sizeof(buff));
	}while(ret>0 && strcmp(buff,"\n"));
}

int echo_www(int fd,const char* path,int size)
{
	drop_header(fd);
	int new_fd = open(path,O_RDONLY);
	if(new_fd<0){
		printf_log("resurce not found\n",FATAL);
		return 404;
	}
	const char* echo_line = "HTTP/1.0 200 OK\r\n";
	send(fd,echo_line,strlen(echo_line),0);
	const char* blank_line = "\r\n";
	send(fd,blank_line,strlen(blank_line),0);

	if(sendfile(fd,new_fd,NULL,size)<0){
		printf_log("send msg have a problem\n",FATAL);
		return 501;
	}
	close(new_fd);
}

int exe_cgi(int fd,const char* method,\
		    const char* path,const char* query_string)
{
	int connect_len = -1;
	char METHOD[SIZE/10];
	char QUERY_STRING[SIZE];
	char CONNECT_LENGTH[SIZE];
	if(strcasecmp(method,"GET")==0){
		drop_header(fd);
	}else{
		int ret=-1;
		char buff[SIZE];
		do{
			ret = get_line(fd,buff,SIZE);
			if(strncasecmp(buff,"Connect-Length: ",16)==0){
				connect_len = atoi(&buff[16]);
			}
		}while(ret>0 && strcmp(buff,"\r\n"));

		if(connect_len<0){
			printf_log("get connect_length failed\n",FATAL);
			echo_error(fd,404);
			return -1;
		}
	}

//	send(fd,type,strlen(tyConnect-Type:text/html;charset=ISO-8859-1\r\n"),0);
	const char* echo_line = "HTTP/1.0 200 OK\r\n";
	send(fd,echo_line,strlen(echo_line),0);
	const char* Type = "Connect_Type:text/html;charset=ISO-8859-1\r\n";
	send(fd,Type,strlen(Type),0);
	const char* blank_line = "\r\n";
	send(fd,blank_line,strlen(blank_line),0);

	int input[2];
	int output[2];

	if(pipe(input)<0){
		echo_error(fd,501);
		return -2;
	}
	if(pipe(output)<0){
		echo_error(fd,501);
		return -3;
	}

	int id =fork();
	if(id<0){
		echo_error(fd,501);
		return -4;
	}else if(id==0){//child
		close(input[1]);
		close(output[0]);

	   sprintf(METHOD,"METHOD=%s",method);
	   putenv(METHOD);
	   printf("method:%s\n",method);
	   printf("METHOD:%s\n",METHOD);
	   if(strcasecmp(method,"GET")==0){
		   sprintf(QUERY_STRING,"QUERY_STRING=%s",query_string);
		   putenv(QUERY_STRING);
		   printf("%s\n",QUERY_STRING);
	   }else if(strcasecmp(method,"POST")==0){
		   sprintf(CONNECT_LENGTH,"CONNECT_LENGTH=%d",connect_len);
		   putenv(CONNECT_LENGTH);
	   }
	    dup2(input[0],0);
	    dup2(output[1],1);

		execl(path,path,NULL);
		exit(1);

	}else{//father
		close(input[0]);
		close(output[1]);

		if(strcasecmp(method,"POST")==0){
			char c='\0';
			int i=0;
			for(;i<connect_len;++i){
				recv(fd,&c,1,0);
				write(input[1],&c,1);
			}
		}

		while(1){
			char c='\0';
			ssize_t s=read(output[0],&c,1);
			if(s>0){
				write(fd,&c,1);
			}else{
				break;
			}
		}

		close(input[1]);
		close(output[0]);
	}
}


void* handler_request(void* arg)
{
	int fd = (int)arg;
	int ret = -1;
	int num_error = -1;
	int cgi = 0;
	char* query_string;

#ifdef _DEBUG_
//printf("debug\n");//get all message before '\n'
   char buff[SIZE];
   do
   {
	   ret = get_line(fd,buff,sizeof(buff));
	   printf("%s",buff);
   }while(ret>0 && strcmp(buff,"\n"));
#else
	//printf("release\n");//get methord and url
   char buff[SIZE];
   char method[SIZE/10];
   char url[SIZE];
   char path[SIZE];
   int i=0,j=0;
   if(get_line(fd,buff,sizeof(buff))<0)
   {
	   num_error = 501;
       printf_log("get request line error",FATAL);//error
	   goto end;
   }

   while(j<sizeof(method)-1 && i<sizeof(buff)\
		        && !isspace(buff[i])){//get method
	   method[j++] = buff[i++];
   }
   method[j] = 0;

   while(isspace(buff[i]) && i<sizeof(buff)){//delete blank
	   i++;
   }
   j=0;
   while(j<sizeof(url) && i<sizeof(buff) && \
		   !isspace(buff[i])){//get url
	   url[j] = buff[i];
	   j++;
	   i++;
   }
   url[j] = 0;

   printf("method :%s, url :%s\n",method,url);

   if(strcasecmp(method,"GET") && strcasecmp(method,"POST"))//strcasecmp success-0 error-!0
   {
	   num_error = 501;
	   printf_log("method is not ok\n",FATAL);
	   goto end;
   }

   if(strcasecmp(method,"POST")==0)//POST 
	   cgi=1;
   query_string = url;
   while(*query_string != 0)
   {
	   if(*query_string == '?'){//have paramatre
		   cgi=1;
		   *query_string = 0;//separation paramatre and resource
		   query_string++;
		   break;
	   }
	   query_string++;
   }

   sprintf(path,"wwwroot%s",url);

   if(path[strlen(path)-1] == '/'){
	   strcat(path,"index.html");
   }else{
	 //  strcat(path,url);
   }
   printf("path %s\n",path);

   struct stat st;
   if(stat(path,&st)<0){
	   perror("stat");
	   printf_log("path not found",FATAL);
	   num_error = 404;
	   goto end;
   }else if(S_ISDIR(st.st_mode)){
	   strcat(path,"/index.html");
   }else{
	   if((st.st_mode & S_IXUSR)||(st.st_mode & S_IXGRP)||(st.st_mode & S_IXOTH)){
		   cgi=1;//???
	   }
   }


   if(cgi){//cgi=1
	   exe_cgi(fd,method,path,query_string);
   }else{//normal
	   num_error = echo_www(fd,path,st.st_size);
   }

#endif
end:
   echo_error(fd,num_error);
   close(fd);

}
