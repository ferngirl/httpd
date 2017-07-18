#include<stdlib.h>
#include<stdio.h>
#include<string.h>


void math_cgi(char buff[])
{
	char* argv[3];
	int i=0;
	char* start = buff;
	while(*start)
	{
		if(*start=='='){
			start++;
			argv[i++] = start;
			start++;
			continue;
		}
		if(*start=='&'){
			*start='\0';
		}
		start++;
	}
	argv[i] = NULL;
	int data1 = atoi(argv[0]);
	int data2 = atoi(argv[1]);

	printf("<html>\n");
	printf("<body>\n");
	printf("<h1>\n");
	printf("%d+%d=%d\n",data1,data2,data1+data2);
	printf("%d-%d=%d\n",data1,data2,data1-data2);
	printf("%d*%d=%d\n",data1,data2,data1*data2);
}
int main()
{
//	printf("<html><h1>hello math-bin</h1></html>");
//	char* arg_string = NULL;
//	char* method = NULL;
//	char* connect_length = NULL;
	char info[1024];
//	int i=0;
//
//	method=getenv("METHOD");
//	printf("method:%s\n",method);
//	arg_string = getenv("QUERY_STRING");
//	printf("arg-string %s\n",arg_string);
//	connect_length = getenv("CONNECT_LENGTH");
//	printf("connect_length %s\n",connect_length);
//		if(!method){
//			printf("get method are error\n");
//		return -1;
//	}
//	if(method && strcasecmp(method,"GET")==0){
//		arg_string = getenv("QUERY_STRING");
//		if(!arg_string){
//			printf("get method GET are error\n");
//			return -2;
//		}
//		strcpy(info,arg_string);
//	}else if(strcasecmp(method,"POST")==0){
//		char c='\0';
//		connect_length = getenv("CONNECT_LENGTH");
//		for(;i<atoi(connect_length);++i){
//			recv(0,&c,1,0);
//			info[i++] = c;
//		}
//		info[i] = '\0';
//	}else{
//		printf("get method error\n");
//		return -3;
//	}
	math_cgi(info);
	return 0;
}
