/*

Main function for the duplicate detection framework.

Written by Ismet Zeki Yalniz
Center for Intelligent Information Retrieval
Dept. of Computer Science
U. of Massachusetts-Amherst
email:zeki@cs.umass.edu

*/

#include "headers.h"
#include "DuplicateDetector.h"
#include "MyHashTable.h"

char usage[] = "USAGE:\nDuplicateDetector <docpile1> <docpile2> <outputfile> <threshold>\nOR\nDuplicateDetector <docpile1> <outputfile> <threshold>\n";
void synthetic_pile_generator(int numOfDocuments, int avgDocLength, char * filename);

// NOTE:  none of the term ids should be zero. Make sure that they are positive. 
int main(int argc, char* argv[]){
	time_t seconds;
	seconds = time (NULL);
	DuplicateDetector * detector;
	double threshold = 0.10;

	if ( argc == 4){
		threshold = atof(argv[3]);
		if (threshold == 0.0f){
			printf("Illegal input for the threshold: %s\nQuitting...", argv[3]);
			return 0;
		}
		detector = new DuplicateDetector( argv[1], argv[2], threshold);		
	}else if (argc == 5){
		threshold = atof(argv[4]); 
		if (threshold == 0.0f){
			printf("Illegal input for the threshold: %s\nQuitting...", argv[4]);
			return 0;
		}
		detector = new DuplicateDetector( argv[1], argv[2], argv[3], threshold);		
	}else{
		printf(usage);
		exit(0);
	}
	detector->detectDuplicates();

	printf("Time elapsed %d sec\n", (time(NULL)-seconds));
	MyHashTable::freeMemory();
	return 0;
}

void synthetic_pile_generator(int numOfDocuments, int avgDocLength, char * filename) {
		
	int * pile = (int * ) malloc( sizeof(int) * (numOfDocuments*3 + numOfDocuments*avgDocLength + 1) );
	pile[0] = numOfDocuments;
	int count = 1;

	for (int i = 0; i < numOfDocuments; i++){
		pile[i+1] = i; // fileid
		pile[numOfDocuments+i+1] = avgDocLength; // docLength
		pile[2*numOfDocuments+i+1] = avgDocLength; // listLength				
	}
	for ( int j = 3*numOfDocuments+1 ; j < (numOfDocuments*3 + numOfDocuments*avgDocLength + 1) ; j++){
			pile[j] = count;
			count++;
	}

	FILE *f = fopen(filename, "wb");	
	fwrite(pile, sizeof(int), (numOfDocuments*3 + numOfDocuments*avgDocLength + 1), f);
	fclose(f);

	free(pile);

}
