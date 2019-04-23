CC = gcc
CFLAGS = -Wall -O2

maExec: 
	$(CC) $(CFLAGS) ma.c -o ma ma.o

serverExec:
	$(CC) $(CFLAGS) server.c -o server server.o

clienteExec: 
	$(CC) $(CFLAGS) cliente.c -o cliente cliente.o

exec: 
	$(CC) $(CFLAGS) -c ma.c cliente.c servidor.c

clean:
	rm ma server cliente