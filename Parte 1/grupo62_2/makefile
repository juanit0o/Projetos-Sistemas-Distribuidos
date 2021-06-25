#Grupo 62: Pedro Afonso Neves fc46430
#Ricardo Calçado fc46382
#Tiago Ferreira fc47895


CC=gcc
INCLUDE = Include/
OBJ = Object/
SRC = Source/
BIN = Binary/
FLAG = gcc -g -Wall -I Include/ -c

all : data.o entry.o table.o message.o network_client.o table-client.o table-server.o test_message.o table-client table-server test_message

data.o: $(INCLUDE)data.h
	$(FLAG) $(SRC)data.c -o $(OBJ)data.o

entry.o: $(INCLUDE)entry.h $(INCLUDE)data.h
	$(FLAG) $(SRC)entry.c -o $(OBJ)entry.o

table.o: $(INCLUDE)table-private.h $(INCLUDE)table.h $(INCLUDE)entry.h $(INCLUDE)data.h
	$(FLAG) $(SRC)table.c -o $(OBJ)table.o

message.o: $(INCLUDE)message.h $(INCLUDE)message-private.h $(INCLUDE)table.h $(INCLUDE)data.h
	$(FLAG) $(SRC)message.c -o $(OBJ)message.o

network_client.o: $(INCLUDE)network_client.h $(INCLUDE)network_client-private.h $(INCLUDE)message.h $(INCLUDE)message-private.h $(INCLUDE)inet.h
	$(FLAG) $(SRC)network_client.c -o $(OBJ)network_client.o

test_message.o: $(INCLUDE)message.h $(INCLUDE)message-private.h $(INCLUDE)table-private.h $(INCLUDE)table.h $(INCLUDE)data.h $(INCLUDE)entry.h
	$(FLAG) $(SRC)test_message.c -o $(OBJ)test_message.o

table-client.o: $(INCLUDE)network_client.h $(INCLUDE)network_client-private.h $(INCLUDE)table.h $(INCLUDE)table-private.h $(INCLUDE)message.h $(INCLUDE)message-private.h $(INCLUDE)entry.h $(INCLUDE)data.h
	$(FLAG) $(SRC)table-client.c -o $(OBJ)table-client.o

table-server.o: $(INCLUDE)table.h $(INCLUDE)table-private.h $(INCLUDE)message.h $(INCLUDE)message-private.h $(INCLUDE)inet.h $(INCLUDE)network_client-private.h $(INCLUDE)data.h $(INCLUDE)entry.h
	$(FLAG) $(SRC)table-server.c -o $(OBJ)table-server.o

test_message: $(OBJ)test_message.o $(OBJ)message.o $(OBJ)data.o $(OBJ)table.o $(OBJ)entry.o
	$(CC) $(OBJ)test_message.o $(OBJ)message.o $(OBJ)data.o $(OBJ)table.o $(OBJ)entry.o -o $(BIN)test_message

table-client: $(OBJ)table-client.o $(OBJ)network_client.o $(OBJ)message.o $(OBJ)data.o $(OBJ)table.o $(OBJ)entry.o
	$(CC) $(OBJ)table-client.o $(OBJ)network_client.o $(OBJ)message.o $(OBJ)data.o $(OBJ)table.o $(OBJ)entry.o -o $(BIN)table-client

table-server: $(OBJ)table-server.o $(OBJ)message.o $(OBJ)table.o $(OBJ)network_client.o $(OBJ)data.o $(OBJ)entry.o
	$(CC) $(OBJ)table-server.o $(OBJ)message.o $(OBJ)table.o $(OBJ)network_client.o $(OBJ)data.o $(OBJ)entry.o -o $(BIN)table-server

clean:
	rm $(BIN)/*
	rm $(OBJ)*.o
