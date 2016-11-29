

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"

// -------------------- Globals and structures ------------------- //

#define TESTTIMES 100

typedef struct node{
	size_t 	size;
	struct 	node* next;
} node_t;

typedef struct testResults{
	double 		fragmentation;
	size_t 		totalDeniedSize;
	double 		timeUsed;
} testResults_t;




// --------------------- Function prototypes --------------------- //

// Linked list
node_t *createNode(size_t size);
void addNodeEnd(node_t **start, node_t **end, node_t *newNode);
node_t* dublicateList(node_t *head);
void deleteList(node_t *head);

// Print functions
void printList(node_t *start);
void printResultOld(char* method, double frag, double timeUsed, size_t unfitmem); //Deprecated soon be removed
void printResult(char* name, testResults_t results);
void fileOpenError(char* filename);
void printHelp();

// Malloc algorithm functions
testResults_t bestFit(node_t *chunkListHead, node_t *requestListHead);
testResults_t worstFit(node_t *chunkListHead, node_t *requestListHead);
// Result calculation
double getFragmentation(node_t *head);
void initResults(testResults_t* results);
// For testing multiple times
testResults_t multitest(testResults_t (*f)(node_t*,node_t*),node_t* ch,node_t* rh);




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

	// bestFit(chunkListHead,requestListHead);
	printResult("BestFit", multitest(&bestFit,chunkListHead,requestListHead));
	printResult("WorstFit", multitest(&worstFit,chunkListHead,requestListHead));

    // TODO: Test all allocation methods

	return 0;
}

// --------------------------------------------------------------- //

testResults_t bestFit(node_t *chunkListHead, node_t *requestListHead){
	// Results
	testResults_t results;
	initResults(&results);

	node_t *bestFit = 0;
	// Dublicate chunk list for ecapsulated test
	node_t *dChunkListHead = dublicateList(chunkListHead);;
	// Current chunk / Request
	node_t *currChunk;
	node_t *currRequest;

	// For time testing
	clock_t begin;
    clock_t end;
    clock_t total = 0;

	begin = clock();
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
				//printf("%s: Failed to allocate %3zu bytes of memory.\n", __func__, currRequest -> size);
				results.totalDeniedSize += currRequest -> size;
		}else{
			//printf("Best fit: %d For: %d\n", (int)bestFit->size, (int)currRequest->size);
			bestFit->size -= currRequest->size;
			bestFit = 0;
		}
    }
	end = clock();
	// Calculate Fragmentation
    results.fragmentation = getFragmentation(dChunkListHead);
	// Calculate Time used
	results.timeUsed = (double)(end - begin) / CLOCKS_PER_SEC;
	// Cleanup
    deleteList(dChunkListHead);

	return results;
}

testResults_t worstFit(node_t *chunkListHead, node_t *requestListHead){
	// Results
	testResults_t results;
	initResults(&results);

	node_t *worstFit = 0;
	// Dublicate chunk list for ecapsulated test
	node_t *dChunkListHead = dublicateList(chunkListHead);;
	// Current chunk / Request
	node_t *currChunk;
	node_t *currRequest;

	// For time testing
	clock_t begin;
    clock_t end;
    clock_t total = 0;

	begin = clock();
	for(currRequest = requestListHead; currRequest != 0; currRequest = currRequest->next){
		// Find worst fit
		for(currChunk = dChunkListHead; currChunk != 0; currChunk = currChunk->next){
			if(currChunk->size >= currRequest->size){
				if( worstFit == 0 ){
					worstFit = currChunk;
				}else if( currChunk->size - currRequest->size > worstFit->size - currRequest->size ){
					worstFit = currChunk;
				}
			}
    	}
    	if(worstFit == 0){
				//printf("%s: Failed to allocate %3zu bytes of memory.\n", __func__, currRequest -> size);
				results.totalDeniedSize += currRequest -> size;
		}else{
			//printf("Worst fit: %d For: %d\n", (int)worstFit->size, (int)currRequest->size);
			worstFit->size -= currRequest->size;
			worstFit = 0;
		}
    }
	end = clock();
	// Calculate Fragmentation
    results.fragmentation = getFragmentation(dChunkListHead);
	// Calculate Time used
	results.timeUsed = (double)(end - begin) / CLOCKS_PER_SEC;
	// Cleanup
    deleteList(dChunkListHead);

	return results;
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

void initResults(testResults_t* results){
	results->fragmentation = 0;
	results->totalDeniedSize = 0;
	results->timeUsed = 0;
}

// --------------------------------------------------------------- //
// Tests malloc algorithm n times, where n = TESTTIMES
// testResults_t multitest(testMethod,chunksHead,requestsHead)
testResults_t multitest(testResults_t (*f)(node_t*,node_t*),node_t* ch,node_t* rh){
	testResults_t results;
	initResults(&results);

	double totalTime = 0;
	for (int i = 0; i < TESTTIMES; ++i){
		results = f(ch,rh);
		//printResult("BestFit", multitest(&bestFit,chunkListHead,requestListHead));
		totalTime += results.timeUsed;
	}

	results.timeUsed = totalTime/TESTTIMES;
	return results;
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
//Deprecated soon be removed
void printResultOld(char* name, double timeUsed, double frag, size_t unfitmem){
	printf("%s: Time used: %3fs Fragmentation: %3f MemoryNotAllocated: %3zu\n",\
			name, timeUsed, frag, unfitmem );
};

// --------------------------------------------------------------- //

void printResult(char* name, testResults_t results){
	printf("%s: Time used: %3fs Fragmentation: %3f %% MemoryNotAllocated: %3zu\n",\
			name, results.timeUsed, results.fragmentation*100, results.totalDeniedSize);
};

// --------------------------------------------------------------- //
