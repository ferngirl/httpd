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
		}
		buf[i++] = c;
	}
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

void* handler_request(void* arg)
{
	int fd = (int)arg;
	int ret = -1;

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
   int i=0,j=0;
   if(get_line(fd,buff,sizeof(buff))<0)
   {
       printf_log("get request line error",FATAL);//error
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
   while(j<sizeof(url) && i<sizeof(buff)){//get url
	   url[j++] = buff[i++];
   }
   url[j] = 0;

   printf("method :%s, url :%s",method,url);

   

#endif

}
