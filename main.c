#include "stdio.h"
#include "string.h"

void printHelp();
void fileOpenError(char* filename);

int main(int argc, char *argv[]){

	char* chunksFilename;
	char* sizesFilename;

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
	printf("%s\n", chunksFilename);
	printf("%s\n", sizesFilename);
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
	// Read in all chunks
	printf("%s\n", "CHUNKS:");
	int chunkBuff; // Buffer for reading chunks form chunksFile
	while(fscanf(chunksFile, "%d", &chunkBuff) > 0) {
        printf("%d\n", chunkBuff);
    }
	printf("%s\n", "SIZES:");
    int sizeBuff; // Buffer for reading sizes form sizesFile
	while(fscanf(sizesFile, "%d", &sizeBuff) > 0) {
        printf("%d\n", sizeBuff);
    }
    // TODO: Test allocation methods

	return 0;
}


//TODO: Write better printHelp();
void printHelp(){
	printf("Wrong input.\nExpected:\n./file -c chunks.txt -s sizes.txt\n");
};

void fileOpenError(char* filename){
	printf("Could not open file: %s\n",filename);
};