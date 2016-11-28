

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

// Linked list
node_t *createNode(size_t size);
void addNodeEnd(node_t **start, node_t **end, node_t *newNode);
node_t* dublicateList(node_t *head);
void deleteList(node_t *head);

// Print functions
void printList(node_t *start);
void printResult(char* method, double frag, double timeUsed, size_t unfitmem);
void fileOpenError(char* filename);
void printHelp();

// Malloc algorithm functions
size_t bestFit(node_t *chunkListHead, node_t *requestListHead);

double getFragmentation(node_t *head);



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
	clock_t begin = clock(); // Mesures CPU time spent
	size_t unallocatedMem = bestFit(chunkListHead,requestListHead);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printResult("BestFit",time_spent,0,unallocatedMem);

    // TODO: Test all allocation methods
    // TODO: How to calculate fragmentation?

	return 0;
}

// --------------------------------------------------------------- //

size_t bestFit(node_t *chunkListHead, node_t *requestListHead){
	// Best fitted chunk
	node_t *bestFit = 0;
	// Dublicate chunk list for ecapsulated test
	node_t *dChunkListHead = dublicateList(chunkListHead);
	// Current chunk
	node_t *currChunk;
	// Current request 
	node_t *currRequest;
	// Memory that could not fit
	size_t memThatCouldNotFit = 0;

	for(currRequest = requestListHead; currRequest != 0; currRequest = currRequest->next){
		// Find best fit
		for(currChunk = dChunkListHead; currChunk != 0; currChunk = currChunk->next){
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
				memThatCouldNotFit += currRequest -> size;
		}else{
			//printf("Best fit: %d For: %d\n", (int)bestFit->size, (int)currRequest->size);
			bestFit->size -= currRequest->size;
			bestFit = 0;
		}
    }
    printf("Fragmentation: %3e\n", getFragmentation(dChunkListHead));
    deleteList(dChunkListHead);
    return memThatCouldNotFit;
}


// --------------------------------------------------------------- //

//
// Formula from:
// https://en.wikipedia.org/wiki/Fragmentation_(computing)
// External Memory Fragmentation = 1 - 	largest block of free memory
//										----------------------------
//										total free memory

double getFragmentation(node_t *head){
	if(head == 0){
		return 0;
	}
	size_t largestFreeBlock = head->size;
	size_t totalFree = 0;

	for(node_t *current = head; current != 0; current = current->next){
		if(current->size > largestFreeBlock){
			largestFreeBlock = current->size;
		}
		totalFree+=current->size;
    }
    return 1 - ((double)largestFreeBlock/totalFree);
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

void addNodeEnd(node_t **head, node_t **end, node_t *newNode){
    // if List is empty
    if(*head == 0){
        *head = *end = newNode;
    }
    // if List not empty
    else{
        (*end)->next = newNode;
        *end = (**end).next;
    }
}


// --------------------------------------------------------------- //

node_t* dublicateList(node_t *head){
	if( head == 0 ){
		return 0;
	}
	node_t *newHead = 0;
	node_t *newEnd = 0;
	for(node_t *current = head; current != 0; current = current->next){
        addNodeEnd(&newHead,&newEnd,createNode(current->size));
    }
   	return newHead;
}

// --------------------------------------------------------------- //

void deleteList(node_t *head){
	if(head == 0){
		return;
	}
	node_t *temp = head;
	node_t *current = head;
	current = current->next;
	free(temp);
	while(current != 0){
		temp = current;
		current = current->next;
		free(temp);
	}
}

// --------------------------------------------------------------- //

void printList(node_t *head){
    for(node_t *current = head; current != 0; current = current->next){
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

void printResult(char* name, double timeUsed, double frag, size_t unfitmem){
	printf("%s: Time used: %3fs Fragmentation: %3f MemoryNotAllocated: %3zu\n",\
			name, timeUsed, frag, unfitmem );
};

// --------------------------------------------------------------- //