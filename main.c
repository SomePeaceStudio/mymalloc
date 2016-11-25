#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"

// -------------------- Globals and structures ------------------- //

typedef struct chunk
{
	size_t size;
	struct chunk* next;
} chunk_t;

// --------------------- Function prototypes --------------------- //

void printHelp();
chunk_t *createChunk(size_t size);
void addChunkEnd(chunk_t **start, chunk_t **end, chunk_t *newNode);
void printChunkList(chunk_t *start);
void printResult(char* method, double frag, double timeUsed);
void fileOpenError(char* filename);

void bestFit(chunk_t *chunkListHead, chunk_t *requestListHead);


int main(int argc, char *argv[]){
	
	char* chunksFilename;
	char* sizesFilename;
	chunk_t* chunkListHead = 0;
	chunk_t* chunkListEnd = 0;

	chunk_t* requestListHead = 0;
	chunk_t* requestListEnd = 0;

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
		addChunkEnd(&chunkListHead,&chunkListEnd,createChunk(sizeBuff));
    }
    // Read request list
	while(fscanf(sizesFile, "%d", &sizeBuff) > 0) {
		addChunkEnd(&requestListHead,&requestListEnd,createChunk(sizeBuff));
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

void bestFit(chunk_t *chunkListHead, chunk_t *requestListHead){
	chunk_t *bestFit = 0;
	// Current chunk
	chunk_t *currChunk;
	// Current request 
	chunk_t *currRequest;

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
				printf("%s: Failed to allocate %3d bytes of memory.\n", __func__, (int)currRequest -> size);
		}else{
			//printf("Best fit: %d For: %d\n", (int)bestFit->size, (int)currRequest->size);
			bestFit->size -= currRequest->size;
			bestFit = 0;
		}
    }
}


// --------------------------------------------------------------- //

chunk_t *createChunk(size_t size){
    chunk_t *newNode;
    newNode = (chunk_t*)malloc(sizeof(chunk_t));
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

void addChunkEnd(chunk_t **start, chunk_t **end, chunk_t *newNode){
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

void printChunkList(chunk_t *start){
    for(chunk_t *current = start; current != 0; current = current->next){
        printf("Size: %3d \n", (int)current->size);
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