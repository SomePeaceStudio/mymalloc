#include "stdio.h"
#include "string.h"

void printHelp();
void fileOpenError(char* filename);

int main(int argc, char const *argv[]){

	char* chunksFilename;
	char* sizesFilename;

	if(argc != 5){
		printHelp();
		return 1;
	}
	// Read flags
	for(int i = 1; i < argc; i++){
		if(!strcmp(argv,"-c")){
			chunksFilename = argv[i+1];
			i++;
			break;
		}else if(!strcmp(argv,"-s")){
			sizesFilename = argv[i+1];
			i++;
			break;
		}else{
			printHelp();
			return 1;
		}
	}
	// Open files
	FILE* chunksFile;
	FILE* sizesFile;
	if((chunksFile = fopen(chunksFilename, "r")) == NULL){
		fileOpenError(chunksFilename);
		return 1;
	}
	if((chunksFile = fopen(sizesFilename, "r")) == NULL){
		fileOpenError(chunksFilename);
		return 1;
	}
	// Read in all chunks
	printf("%s\n", "CHUNKS:");
	int chunkBuff; // Buffer for reading chunks form chunksFile
	while(fscanf(file, "%d", &chunkBuff) > 0) {
        printf("%d\n", chunkBuff);
    }
	printf("%s\n", "SIZES:");
    int sizeBuff; // Buffer for reading sizes form sizesFile
	while(fscanf(file, "%d", &sizeBuff) > 0) {
        printf("%d\n", sizeBuff);
    }
    // TODO: Test allocation methods

	return 0;
}


//TODO: Write better printHelp();
void printHelp(){
	printf("Wrong input.\nExpected:\n./file -c chunks.txt -s sizes.txt");
};

void fileOpenError(char* filename){
	printf("Could not open file: %s\n",filename);
};