bin=httpd
cc=gcc
obj=http.o main.o
LDFLAGS=-lpthread
DEBUG = -D_DEBUG_

$(bin):$(obj)
	gcc -o $@ $^ $(LDFLAGS)

%.o:%.c
	gcc -c $< #$(DEBUG);

.PHONY:clean
clean:
	rm -f $(bin) *.o
