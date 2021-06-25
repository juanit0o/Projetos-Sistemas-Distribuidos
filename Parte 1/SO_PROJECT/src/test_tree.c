#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../include/data.h"
#include "../include/entry.h"
#include "../include/tree.h"
#include "../include/tree-private.h"

/**************************************************************/
int testTreeVazia() {
	struct tree_t *tree = tree_create();

	int result = tree != NULL && tree_size(tree) == 0;

	tree_destroy(tree);

	printf("tree - testTreeVazia: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testPutInexistente() {
	int result, i;
	struct tree_t *tree = tree_create();
	char *key[1024];
	struct data_t *data[1024], *d;

	for(i=0; i<1024; i++) {
		key[i] = (char*)malloc(16*sizeof(char));
		sprintf(key[i],"a/key/b-%d",i);
		data[i] = data_create2(strlen(key[i])+1,strdup(key[i]));
		tree_put(tree,key[i],data[i]);
		//printf("%s %d\n",key[i], tree_size(tree) );

	}

	assert(tree_size(tree) == 1024);
	result = (tree_size(tree) == 1024);

	for(i=0; i<1024; i++) {
		d = tree_get(tree,key[i]);
		assert(d->datasize == data[i]->datasize);
		assert(memcmp(d->data,data[i]->data,d->datasize) == 0);
		assert(d->data != data[i]->data);
		//printf("3\n");
		result = result && (d->datasize == data[i]->datasize &&
                           memcmp(d->data,data[i]->data,d->datasize) == 0 &&
                           d->data != data[i]->data);

		data_destroy(d);
	}

	for(i=0; i<1024; i++) {
		free(key[i]);
		data_destroy(data[i]);
	}
	tree_destroy(tree);
	printf("tree - testPutInexistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testPutExistente() {
	int result, i;
	struct tree_t *tree = tree_create();
	char *key[1024];
	struct data_t *data[1024], *d;

	for(i=0; i<1024; i++) {
		key[i] = (char*)malloc(16*sizeof(char));
		sprintf(key[i],"a/key/b-%d",i);
		data[i] = data_create2(strlen(key[i])+1,strdup(key[i]));
		tree_put(tree,key[i],data[i]);
	}


	assert(tree_size(tree) == 1024);
	result = (tree_size(tree) == 1024);

	d = data_create2(strlen("256")+1,strdup("256"));

	tree_put(tree,key[256],d);

	data_destroy(d);

	assert(tree_size(tree) == 1024);
	result = result && (tree_size(tree) == 1024);

	for(i=0; i<1024; i++) {
		//printf("%d\n", i);
		d = tree_get(tree,key[i]);
		//printf("%s\n", key[i]);
		if(i==256) {

			//printf("%d\n", d->datasize);
			result = result && (d->datasize == strlen("256")+1 &&
        	                   memcmp(d->data,"256",d->datasize) == 0);
		} else {
			result = result && (d->datasize == data[i]->datasize &&
        	                   memcmp(d->data,data[i]->data,d->datasize) == 0 &&
        	                   d->data != data[i]->data);
		}
		//printf("%s\n", key[i]);
		data_destroy(d);
		//printf("%s\n", key[i]);
	}

	for(i=0; i<1024; i++) {
		free(key[i]);
		data_destroy(data[i]);
	}
	//printf("%d\n", tree_size(tree) );
	tree_destroy(tree);
	//printf("%d\n", tree_size(tree) );
	printf("tree - testPutExistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

//!!!!!!! ATENÇAO QUE MUDEI PARA TREE2 E DATA2

int testDelInexistente() {
	int result, i;
	struct tree_t *tree = tree_create();
	char *key;
	struct data_t *data;

	for(i=0; i<1024; i++) {
		key = (char*)malloc(16*sizeof(char));
		sprintf(key,"a/key/b-%d",i);
		data = data_create2(strlen(key)+1,key);
		tree_put(tree,key,data);

		data_destroy(data);
	}

	assert(tree_size(tree) == 1024);
	result = (tree_size(tree) == 1024);
	//printf("---------- %d %d\n", result, tree_size(tree));
	result = result && (tree_get(tree,"a/key/b-1024") == NULL) &&
			   (tree_get(tree,"abc") == NULL);
	result = result && (tree_del(tree,"a/key/b-1024") != 0) &&
			   (tree_del(tree,"abc") != 0);
	result = result && (tree_get(tree,"a/key/b-1024") == NULL) &&
			   (tree_get(tree,"abc") == NULL);
  //printf("---------- %d\n", result);
	assert(tree_size(tree) == 1024);
	result = result && (tree_size(tree) == 1024);
  //printf("---------- %d\n", result);
	tree_destroy(tree);

	printf("tree - testDelInexistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testDelExistente() {
	int result, i;
	struct tree_t *tree = tree_create();
	char *key;
	struct data_t *data, *data2 = NULL;

	for(i=0; i<1024; i++) {
		key = (char*)malloc(16*sizeof(char));
		sprintf(key,"a/key/b-%d",i);
		data = data_create2(strlen(key)+1,key);
		//printf("%s\n", key );
		tree_put(tree,key,data);
		//printf("%d\n", data->datasize );
		data_destroy(data);
	}

	//printf("----------------------------- %d\n", tree_size(tree));
	assert(tree_size(tree) == 1024);
	result = (tree_size(tree) == 1024);
	//printf("----------------------------- %d\n", result);
	result = result && ((data = tree_get(tree,"a/key/b-1023")) != NULL) &&
			   ((data2 = tree_get(tree,"a/key/b-45")) != NULL);
	//printf("----------------------------- %d\n", result);

	data_destroy(data);

	data_destroy(data2);

	result = result && (tree_del(tree,"a/key/b-1023") == 0) &&
			   (tree_del(tree,"a/key/b-45") == 0);
	//printf("----------------------------- %d\n", result);
	//printf("----------------------------- %d\n", tree_size(tree));
	result = result && (tree_get(tree,"a/key/b-1023") == NULL) &&
			   (tree_get(tree,"a/key/b-45") == NULL);
  //printf("----------------------------- %d\n", result);
	//printf("----------------------------- %d\n", tree_size(tree));
	assert(tree_size(tree) == 1022);
	//printf("----------------------------- %d\n", tree_size(tree));
	result = result && (tree_size(tree) == 1022);
  //printf("----------------------------- %d\n", result);
	tree_destroy(tree);

	printf("tree - testDelExistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testGetKeys() {
	int result = 1,i,j,achou;
	struct tree_t *tree = tree_create();
	char **keys;
	char *k[4] = {"abc","bcd","cde","def"};
	struct data_t *d = data_create(5);

	tree_put(tree,k[3],d);
	tree_put(tree,k[2],d);
	tree_put(tree,k[1],d);
	tree_put(tree,k[0],d);
	data_destroy(d);

	keys = tree_get_keys(tree);

	for(i=0; keys[i] != NULL; i++) {
		achou = 0;
		for(j=0; j<4; j++) {
			achou = (achou || (strcmp(keys[i],k[j]) == 0));
		}
		result = (result && achou);
	}
	result = result && (tree_size(tree) == i);
	tree_free_keys(keys);
	tree_destroy(tree);
	printf("tree - testGetKeys: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int main() {
	int score = 0;

	printf("iniciando teste tree bin\n");

	score += testTreeVazia();

	score += testPutInexistente();

	score += testPutExistente();

	score += testDelInexistente();

	score += testDelExistente();

	score += testGetKeys();

	printf("teste tree bin: %d/6\n",score);

    if (score == 6)
        return 0;
    else
        return -1;
}
