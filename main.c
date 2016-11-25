#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"

// -------------------- Globals and structures ------------------- //

typedef struct node{
	size_t size;
	struct node* next;
} node_t;

// --------------------- Function prototypes --------------------- //

void printHelp();
node_t *createNode(size_t size);
void addNodeEnd(node_t **start, node_t **end, node_t *newNode);
void printList(node_t *start);
void printResult(char* method, double frag, double timeUsed);
void fileOpenError(char* filename);

void bestFit(node_t *chunkListHead, node_t *requestListHead);


int main(int argc, char *argv[]){
	
	char* chunksFilename;
	char* sizesFilename;
	node_t* chunkListHead = 0;
	node_t* chunkListEnd = 0;

	node_t* requestListHead = 0;
	node_t* requestListEnd = 0;

	int sizeBuff; // Buffer for reading sizes and chunks form sizesFile

	if(argc != 5){
		printHelp();
		return 1;
	}
	// Read flags
	for(int i = 1; i < argc; i++){
		if(!strcmp(argv[i],"-c")){
			chunksFilename = argv[i+1];
			i++;
			continue;
		}
		if(!strcmp(argv[i],"-s")){
			sizesFilename = argv[i+1];
			i++;
			continue;
		}
		printHelp();
		return 1;
		
	}
	// Open files
	FILE* chunksFile;
	FILE* sizesFile;
	if((chunksFile = fopen(chunksFilename, "r")) == NULL){
		fileOpenError(chunksFilename);
		return 1;
	}
	if((sizesFile = fopen(sizesFilename, "r")) == NULL){
		fileOpenError(sizesFilename);
		return 1;
	}
	// Read chunk list
	while(fscanf(chunksFile, "%d", &sizeBuff) > 0) {
		addNodeEnd(&chunkListHead,&chunkListEnd,createNode(sizeBuff));
    }
    // Read request list
	while(fscanf(sizesFile, "%d", &sizeBuff) > 0) {
		addNodeEnd(&requestListHead,&requestListEnd,createNode(sizeBuff));
    }


    // Best Fit Test
	clock_t begin = clock();
	bestFit(chunkListHead,requestListHead);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printResult("BestFit",time_spent,0);

    // TODO: Test all allocation methods
    

	return 0;
}

// --------------------------------------------------------------- //

void bestFit(node_t *chunkListHead, node_t *requestListHead){
	node_t *bestFit = 0;
	// Current chunk
	node_t *currChunk;
	// Current request 
	node_t *currRequest;

	for(currRequest = requestListHead; currRequest != 0; currRequest = currRequest->next){
		// Find best fit
		for(currChunk = chunkListHead; currChunk != 0; currChunk = currChunk->next){
			if(currChunk->size >= currRequest->size){
				if( bestFit == 0 ){
					bestFit = currChunk;
				}else if( currChunk->size - currRequest->size < bestFit->size - currRequest->size ){
					bestFit = currChunk;
				}
			}
    	}
    	if(bestFit == 0){
				printf("%s: Failed to allocate %3zu bytes of memory.\n", __func__, currRequest -> size);
		}else{
			//printf("Best fit: %d For: %d\n", (int)bestFit->size, (int)currRequest->size);
			bestFit->size -= currRequest->size;
			bestFit = 0;
		}
    }
}


// --------------------------------------------------------------- //

node_t *createNode(size_t size){
    node_t *newNode;
    newNode = (node_t*)malloc(sizeof(node_t));
    // If did not allocate memory
    if( newNode == NULL ){
        printf("%s\n", "Error: Could not allocate memory");
        exit(1);
    }
    newNode->size = size;
    newNode->next = 0;
    return newNode;
};

// --------------------------------------------------------------- //

void addNodeEnd(node_t **start, node_t **end, node_t *newNode){
    // if List is empty
    if(*start == 0){
        *start = *end = newNode;
    }
    // if List not empty
    else{
        (*end)->next = newNode;
        *end = (**end).next;
    }
}


// --------------------------------------------------------------- //

void printList(node_t *start){
    for(node_t *current = start; current != 0; current = current->next){
        printf("Size: %3zu \n", current->size);
    }
}

// --------------------------------------------------------------- //

//TODO: Write better printHelp();
void printHelp(){
	printf("Wrong input.\nExpected:\n./file -c chunks.txt -s sizes.txt\n");
};

// --------------------------------------------------------------- //

void fileOpenError(char* filename){
	printf("Could not open file: %s\n",filename);
};

// --------------------------------------------------------------- //

void printResult(char* name, double timeUsed, double frag){
	printf("%s: Time used: %3fs Fragmentation: %3f\n",name,timeUsed,frag);
};

// --------------------------------------------------------------- //