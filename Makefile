all: clean client server

client:
	gcc UDPclient01.c -g -o client

server:
	gcc UDPserver01.c -g -o server

clean:
	rm -rf ./client ./server
