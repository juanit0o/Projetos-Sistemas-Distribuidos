#Nomes dos diretorios
OBJ_DIR = obj
INCLUDE_DIR = include
BIN_DIR = bin
SRC_DIR = src

#Ficheiros obj gerados por cada teste
OBJ_TEST_DATA = test_data.o data.o
OBJ_TEST_ENTRY = test_entry.o entry.o data.o
OBJ_TEST_TREE = test_tree.o tree.o entry.o data.o
OBJ_TEST_SERIALIZATION = test_serialization.o serialization.o tree.o entry.o data.o

#Header files p cada ficheiro objeto
data.o = data.h
entry.o = entry.h
tree.o = tree.h tree-private.h entry.h data.h
serialization.o = serialization.h tree.h tree-private.h entry.h data.h

#Header files para cada ficheiro teste objeto
test_data.o = data.h
test_entry.o = entry.h data.h
test_tree.o = tree.h tree-private.h entry.h data.h
test_serialization.o = serialization.h tree.h tree-private.h entry.h data.h

#Compilador + opcao para poder correr o gdb
CC = gcc
CFLAGS = -g -Wall

#Correr todos os testes com o comando make
all: test_data test_entry test_tree test_serialization %.o
.PHONY: all


#criacao dos executaveis
test_data: $(OBJ_TEST_DATA)
	    $(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJ_TEST_DATA)) -o $(BIN_DIR)/test_data
	    ./$(BIN_DIR)/test_data

test_entry: $(OBJ_TEST_ENTRY)
	    $(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJ_TEST_ENTRY)) -o $(BIN_DIR)/test_entry
	    ./$(BIN_DIR)/test_entry

test_tree:  $(OBJ_TEST_TREE)
	    $(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJ_TEST_TREE)) -o $(BIN_DIR)/test_tree
	    ./$(BIN_DIR)/test_tree

test_serialization:  $(OBJ_TEST_SERIALIZATION)
			$(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/,$(OBJ_TEST_SERIALIZATION)) -o $(BIN_DIR)/test_serialization
			./$(BIN_DIR)/test_serialization

#criacao dos objetos de tudo o que esta na pasta src
%.o: $(SRC_DIR)/%.c $($@)
	    $(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<

clean:
	    rm -rf $(BIN_DIR)/* /$(OBJ_DIR)/*
