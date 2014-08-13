# Alexander Schöberl 0942320
# 2048

CC = gcc
bin = ./bin/
src = ./src/

all: $(bin)2048-client $(bin)2048-server

#Server
$(bin)2048-server: $(bin)2048-server.o $(bin)send_receive.o $(bin)calc.o $(bin)common.o
	$(CC) -o $(bin)2048-server $(bin)2048-server.o $(bin)send_receive.o $(bin)calc.o $(bin)common.o -lsem182 -lm

$(bin)2048-server.o: $(src)2048-server.c
	$(CC) -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -g -c -o $(bin)2048-server.o $(src)2048-server.c

$(bin)send_receive.o: $(src)send_receive.c
	$(CC) -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -g -c -o $(bin)send_receive.o $(src)send_receive.c

$(bin)calc.o: $(src)calc.c
	$(CC) -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -g -c -o $(bin)calc.o $(src)calc.c

#Client
$(bin)2048-client: $(bin)2048-client.o $(bin)interface.o $(bin)draw.o $(bin)common.o
	$(CC) -o $(bin)2048-client $(bin)2048-client.o $(bin)interface.o $(bin)draw.o $(bin)common.o -lsem182

$(bin)2048-client.o: $(src)2048-client.c
	$(CC) -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -g -c -o $(bin)2048-client.o $(src)2048-client.c

$(bin)interface.o: $(src)interface.c
	$(CC) -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -g -c -o $(bin)interface.o $(src)interface.c

$(bin)draw.o: $(src)draw.c
	$(CC) -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -g -c -o $(bin)draw.o $(src)draw.c	

#Common
$(bin)common.o: $(src)common.c
	$(CC) -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -g -c -o $(bin)common.o $(src)common.c

ipcrm:
	ipcrm -M 0x61030692
	ipcrm -S 0x63030692
	ipcrm -S 0x62030692
	ipcrm -S 0x64030692

clean:
	rm -r bin
	mkdir bin
