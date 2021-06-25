CC       = gcc
BIN_DIR  = binary
SRC_DIR  = src
TSTS_DIR = tests
INC_DIR  = include
OBJ_DIR  = objs
LIB_DIR  = libs

OBJECTS: data.o entry.o serialization.o tree.o

test_data: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(TSTS_DIR)/test_data.c $(OBJ_DIR)/data.o -o $(BIN_DIR)/test_data

test_entry: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(TSTS_DIR)/test_entry.c $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o -o \
	$(BIN_DIR)/test_entry

test_serialization: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(TSTS_DIR)/test_serialization.c $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/serialization.o -o \
	$(BIN_DIR)/test_serialization

test_tree: $(OBJ_DIR)/*.o
	$(CC) -g -Wall -I $(INC_DIR) $(TSTS_DIR)/test_tree.c $(OBJ_DIR)/data.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/tree.o -o \
	$(BIN_DIR)/test_tree


data.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/data.c -I $(INC_DIR) -o $(OBJ_DIR)/data.o

entry.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/entry.c -I $(INC_DIR) -o $(OBJ_DIR)/entry.o

serialization.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/serialization.c -I $(INC_DIR) -o $(OBJ_DIR)/serialization.o
	
tree.o: $(SRC_DIR) $(INC_DIR)
	$(CC) -g -c $(SRC_DIR)/tree.c -I $(INC_DIR) -o $(OBJ_DIR)/tree.o

clean:
	rm -f ./$(OBJ_DIR)/*.o
	rm -f ./$(BIN_DIR)/*
	rm -f ./$(LIB_DIR)/*
