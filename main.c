//===========================================================================//
// Name: LSP_MD5
// Team: 
//      Kristaps
//      Elvijs
//      Raivis
//      Andis
//===========================================================================//

// ---------------------------- Includes ----------------------------------- //

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"

// -------------------- Globals and structures ----------------------------- //

// Defines how many times run every malloc algoritm function. 
// Used for calculating average run time.
#define TESTTIMES 10000

// Structure for linked list for storing chunks and requests.
typedef struct node{
    size_t  size;
    struct  node* next;
} node_t;

// Structure for test results, every malloc algorithm function returns this.
typedef struct testResults{
    double      fragmentation;
    size_t      totalDeniedSize;
    double      timeUsed;
} testResults_t;


// --------------------- Function prototypes ------------------------------- //

// Linked list related
node_t *createNode(size_t size);
void addNodeEnd(node_t **start, node_t **end, node_t *newNode);
node_t* dublicateList(node_t *head);
void deleteList(node_t *head);

// Print functions
void printResult(char* name, testResults_t results);
void fileOpenError(char* filename);
void printHelp();

// Malloc algorithm functions
testResults_t bestFit(node_t *chunkListHead, node_t *requestListHead);
testResults_t worstFit(node_t *chunkListHead, node_t *requestListHead);
testResults_t firstFit(node_t *chunkListHead, node_t *requestListHead);
testResults_t nextFit(node_t *chunkListHead, node_t *requestListHead);

// Result calculation
double getFragmentation(node_t *head);      // Calculates fragmentation
void initResults(testResults_t* results);   // Initializes test results struct
// multitest - run multiple tries of one algorithm
testResults_t multitest(testResults_t (*f)(node_t*,node_t*),node_t* ch,node_t* rh);

// Debug functions
void printList(node_t *start);



int main(int argc, char *argv[]){

    char* chunksFilename;
    char* sizesFilename;

    // Pointers to first and last elements in chunk and request lists
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
    // Read chunk list from chunksFile in linked list data structure
    while(fscanf(chunksFile, "%d", &sizeBuff) > 0) {
        addNodeEnd(&chunkListHead,&chunkListEnd,createNode(sizeBuff));
    }
    // Read request list from sizesFile in linked list data structure
    while(fscanf(sizesFile, "%d", &sizeBuff) > 0) {
        addNodeEnd(&requestListHead,&requestListEnd,createNode(sizeBuff));
    }

    // Run all mollock algorithm tests multiple times & print results
    printResult("BestFit", multitest(&bestFit,chunkListHead,requestListHead));
    printResult("WorstFit", multitest(&worstFit,chunkListHead,requestListHead));
    printResult("FirstFit", multitest(&firstFit,chunkListHead,requestListHead));
    printResult("NextFit", multitest(&nextFit,chunkListHead,requestListHead));

    return 0;
}

// ------------------------------- BEST FIT -------------------------------- //

// testResults_t bestFit(node_t *chunkListHead, node_t *requestListHead)
// chunkListHead - pointer to first element in chunks list 
// requestListHead - pointer to first element in requests list

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


// ------------------------------ WORST FIT -------------------------------- //

// testResults_t worstFit(node_t *chunkListHead, node_t *requestListHead)
// chunkListHead - pointer to first element in chunks list 
// requestListHead - pointer to first element in requests list

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

// -------------------------- FIRST FIT ------------------------------------ //

// testResults_t firstFit(node_t *chunkListHead, node_t *requestListHead)
// chunkListHead - pointer to first element in chunks list 
// requestListHead - pointer to first element in requests list

testResults_t firstFit(node_t *chunkListHead, node_t *requestListHead){
    // Results
    testResults_t results;
    initResults(&results);

    node_t *firstFit = 0;
    // Dublicate chunk list for ecapsulated test
    node_t *dChunkListHead = dublicateList(chunkListHead);
    // Current chunk / Request
    node_t *currChunk;
    node_t *currRequest;

    clock_t begin = clock();
    for(currRequest = requestListHead; currRequest != 0; currRequest = currRequest->next){
        // Find first fit
        for(currChunk = dChunkListHead; currChunk != 0; currChunk = currChunk->next){
            if(currChunk->size >= currRequest->size){
                if( firstFit == 0 ){
                    firstFit = currChunk;
                }else {
                    firstFit = currChunk;
                }
            }
        }
        if(firstFit == 0){
                //printf("%s: Failed to allocate %3zu bytes of memory.\n", __func__, currRequest -> size);
                results.totalDeniedSize += currRequest -> size;
        }else{
            //printf("First fit: %d For: %d\n", (int)firstFit->size, (int)currRequest->size);
            firstFit->size -= currRequest->size;
            firstFit = 0;
        }
    }
    clock_t end = clock();
    // Calculate Time used
    results.timeUsed = (double)(end - begin) / CLOCKS_PER_SEC;
    // Calculate Fragmentation
    results.fragmentation = getFragmentation(dChunkListHead);

        // Cleanup
    deleteList(dChunkListHead);
    return results;
}

// -------------------------------- NEXT FIT ------------------------------- //

// testResults_t nextFit(node_t *chunkListHead, node_t *requestListHead)
// chunkListHead - pointer to first element in chunks list 
// requestListHead - pointer to first element in requests list

testResults_t nextFit(node_t *chunkListHead,node_t *requestListHead){
    // Results
    testResults_t results;
    initResults(&results);


    // Dublicate chunk list for ecapsulated test
    node_t *dChunkListHead = dublicateList(chunkListHead);
    // Current chunk / Request
    node_t *currChunk;
    node_t *currRequest;
    // specify helper to get a failure allocations
    int helper = 0;

    currChunk = dChunkListHead;
    node_t *nextFit = currChunk;

    clock_t begin = clock();
    for(currRequest = requestListHead; currRequest != 0; currRequest = currRequest->next){
        currChunk = nextFit;
        if(currChunk->size >= currRequest->size){
            currChunk->size -= currRequest->size;
            helper = 1;
            //printf("Next fit: %d For: %d\n", (int)nextFit->size, (int)currRequest->size);
        }else {
            currChunk = currChunk->next;
            if (currChunk->size >= currRequest->size){
                currChunk->size -= currRequest->size;
                helper = 1;
                //printf("Next fit: %d For: %d\n", (int)nextFit->size, (int)currRequest->size);
            }
        }
        nextFit = currChunk;
        if (currChunk->size<=currRequest->size){
            //printf("%s: Failed to allocate %3zu bytes of memory.\n", __func__, currRequest -> size);
            results.totalDeniedSize += currRequest->size;
        }
        if (nextFit->next == 0){
            nextFit = dChunkListHead;
            helper = 0;
        }
        if (helper == 0){
            //printf("%s: Failed to allocate %3zu bytes of memory.\n", __func__, currRequest -> size);
            results.totalDeniedSize += currRequest->size;
        }
    }
    clock_t end = clock();
    // Calculate Time used
    results.timeUsed = (double)(end - begin) / CLOCKS_PER_SEC;
    // Calculate Fragmentation
    results.fragmentation = getFragmentation(dChunkListHead);

    // Cleanup
    deleteList(dChunkListHead);
    return results;
}

// ------------------------------------------------------------------------- //

// double getFragmentation(node_t *head) where head is pointer to first chunks
// list element.
// 
// Formula used:
// https://en.wikipedia.org/wiki/Fragmentation_(computing)
// External Memory Fragmentation = 1 -  largest block of free memory
//                                      ----------------------------
//                                      total free memory

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

// ------------------------------------------------------------------------- //

// Initializes new testResults_t struct elements
void initResults(testResults_t* results){
    results->fragmentation = 0;
    results->totalDeniedSize = 0;
    results->timeUsed = 0;
}

// ------------------------------------------------------------------------- //

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

// ------------------------------------------------------------------------- //

// Creates new node with specified size
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

// ------------------------------------------------------------------------- //

// Adds new node element at the end of linked list 
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


// ------------------------------------------------------------------------- //

// Dublicated liked list starting from first element.
// Used for encapsulated tests in each malloc algorithm function.
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

// ------------------------------------------------------------------------- //

// Delete linded list.
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

// ------------------------------------------------------------------------- //

// Prints linked list size values starting from first element
void printList(node_t *head){
    for(node_t *current = head; current != 0; current = current->next){
        printf("Size: %3zu \n", current->size);
    }
}

// ------------------------------------------------------------------------- //

// Prints help if wrong flags pased to main()
void printHelp(){
    printf("Wrong input.\nExpected:\n./file -c chunks.txt -s sizes.txt\n");
};

// ------------------------------------------------------------------------- //

// Printed if could not open file
void fileOpenError(char* filename){
    printf("Could not open file: %s\n",filename);
};

// ------------------------------------------------------------------------- //

// Prints name specified and results returned from malloc algorithm function
void printResult(char* name, testResults_t results){
    printf("%s: Time used: %3fs Fragmentation: %3f %% MemoryNotAllocated: %3zu\n",\
            name, results.timeUsed, results.fragmentation*100, results.totalDeniedSize);
};

// ------------------------------------------------------------------------- //
