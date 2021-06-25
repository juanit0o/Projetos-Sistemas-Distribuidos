#GRUPO 19
#Diogo Pinto - 52763
#Francisco Ramalho - 53472
#João Funenga - 53504

OBJ_DIR = obj
INCLUDE_DIR = include
BIN_DIR = bin
SRC_DIR = src
LIB_DIR = lib

#Ficheiros obj gerados por cada teste
OBJ_TREE_CLIENT = tree_client.o client_lib.o
OBJ_TREE_SERVER = tree_server.o server_lib.o
OBJ_CLIENT_LIB = client_stub.o network_client.o data.o entry.o tree.o
OBJ_SERVER_LIB = network_server.o tree_skel.o data.o entry.o tree.o

OBJS = tree_client.o entry.o data.o client_stub.o network_client.o tree_server.o network_server.o tree_skel.o tree.o

#Header files p cada ficheiro objeto
data.o = data.h
entry.o = entry.h
tree.o = tree.h tree-private.h entry.h data.h
client_stub.o = client_stub.h client_stub_private.h messsage_private.h data.h entry.h tree.h network_client.h sdmesssage.pb-c.h
network_client.o = network_client.h client_stub.h client_stub_private.h sdmesssage.pb-c.h messsage_private.h
network_server.o = network_server.h sdmesssage.pb-c.h messsage_private.h
sdmesssage.pb-c.o = sdmesssage.pb-c.h
tree_skel.o = tree_skel.h tree.h data.h entry.h messsage_private.h
tree_server.o = network_server.h tree_skel.h
tree_client.o = data.h entry.h client_stub.h

#Compilador + opcao para poder correr o gdb + comandos protobuf
CC = gcc
PROTOC_DIR =/usr/local
CFLAGS = -g -Wall
PROTO_FLAGS = -g
PROTO = -O2 -I${PROTOC_DIR}/include/ -L/usr/local/lib -lprotobuf-c
PROTO_COMPILE = protoc --c_out=.
LIB_CLIENT = $(LIB_DIR)/client_lib.o

#Correr todos os testes com o comando make
.PHONY: proto   #Obrigar o proto a ser feito antes de qualquer target
all: proto client_lib.o server_lib.o $(OBJS) tree_client tree_server

#Gerar ficheiro header,c e gerar o objeto e mover para as pastas respetivas
proto:
		$(PROTO_COMPILE) sdmessage.proto
		$(CC) $(PROTO_FLAGS) -c sdmessage.pb-c.c
		mv sdmessage.pb-c.c $(SRC_DIR)/
		mv sdmessage.pb-c.h $(INCLUDE_DIR)/
		mv sdmessage.pb-c.o $(OBJ_DIR)/
-include proto    #independentemente do target escolhido na consola, corre sempre o proto primeiro

client_lib.o: $(OBJ_CLIENT_LIB)
		ld -r $(addprefix $(OBJ_DIR)/,$(OBJ_CLIENT_LIB)) -o $(LIB_DIR)/client_lib.o

server_lib.o: $(OBJ_SERVER_LIB)
		ld -r $(addprefix $(OBJ_DIR)/,$(OBJ_SERVER_LIB)) -o $(LIB_DIR)/server_lib.o

%.o: src/%.c $($@)
		$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<

tree_client: $(OBJ_TREE_CLIENT)
		$(CC) $(CFLAGS) $(OBJ_DIR)/tree_client.o $(LIB_DIR)/client_lib.o -o $(BIN_DIR)/tree_client $(OBJ_DIR)/sdmessage.pb-c.o $(PROTO)

tree_server: $(OBJ_TREE_SERVER)
		$(CC) $(CFLAGS) $(OBJ_DIR)/tree_server.o $(LIB_DIR)/server_lib.o -o $(BIN_DIR)/tree_server $(OBJ_DIR)/sdmessage.pb-c.o $(PROTO)


clean:
		rm -rf $(BIN_DIR)/*
		rm -rf $(OBJ_DIR)/*
		rm -rf $(LIB_DIR)/client_lib.o
		rm -rf $(LIB_DIR)/server_lib.o
		rm -rf $(SRC_DIR)/sdmessage.pb-c.c
		rm -rf $(INCLUDE_DIR)/sdmessage.pb-c.h
