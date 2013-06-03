/*

Represents a pile of books written in binary format.

Document pile specification:
1 - The first integer n is the total number of documents in the pile. 
2 - Next n integers specify the document id (docID) of each file.
3 - Next n integers specify the document length (docLength) of each file
4 - Next n integers specify the number of word IDs included for each file (listLength)

Written by Ismet Zeki Yalniz
Center for Intelligent Information Retrieval
Dept. of Computer Science
U. of Massachusetts-Amherst
email:zeki@cs.umass.edu

*/

#include "DocPile.h"
#include "headers.h"

DocPile::DocPile(char * fname){
	readPile(fname);
}

DocPile::~DocPile(void){
	free(data);
}

void DocPile::readPile(char * fname){
	FILE *file;
	unsigned long fileLen;

	file = fopen(fname, "rb");
	if (!file){
		fprintf(stderr, "Cant open the file %s", fname);
		exit(1);
		return;
	}
	
	// file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);

	//allocate memory
	data=(int *)malloc(fileLen+1);
	if (!data){
		fprintf(stderr, "Cant allocate memory for the file!");
        fclose(file);
		return;
	}

	//read file into data
	fread((void*)data, fileLen, 1, file);
	fclose(file);	

	// locate data in the file
	numOfDocs = data[0];
	
	printf("numofdocts = %d docIDs[0] = %d docLength = %d listLengths[0] = %d\n", numOfDocs, data[1], data[1+numOfDocs], data[1+2*numOfDocs]);
	//printf("numofdocts = %d \n", numOfDocs);
	fflush(stdout);

	docIDs = &data[1];
	docLengths = &data[1+numOfDocs];
	listLengths = &data[1+2*numOfDocs];
}