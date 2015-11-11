All: Server Client
Server: Server.c
	gcc Server.c -o Server -lpthread
Client: Client.c
	gcc Client.c -o Client -lpthread
