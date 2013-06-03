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

// NOTE: none of the term ids should be zero. Make sure that they are positive. 
int main(int argc, char* argv[]){
	time_t seconds;
	seconds = time (NULL);
	DuplicateDetector * detector;
	double threshold = 0.10;


//	char filename1[] = "docPile1e.syn";
//	synthetic_pile_generator(1000, 10000, &filename1[0]);
//	char filename2[] = "docPile2e.syn";
//	synthetic_pile_generator(40000, 10000, &filename2[0]);

	// test 
/*	char file1[] = "barrel_52000.bar";
	char file2[] = "barrel_52000.bar";
	char fileout[] = "out.syn";
	detector = new DuplicateDetector( file1, fileout ,threshold);	

//	detector = new DuplicateDetector( file1, file2, fileout ,threshold);	
	detector->detectDuplicates();
	printf("Time elapsed %d sec\n", (time(NULL)-seconds));
	MyHashTable::freeMemory();
	return 1;
*/

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

/*public char ** tokenize(char * s, char * delim){

	// count the tokens
	char * token = strtok(s,delim);

}

void binarizeFiles(char* fileList, char* outFile, int from, int to){

        char * list =  readFile(fileList);
        int numberOfDocs = (to-from);
        FILE *f = fopen(filename, "wb");	
        int maxSize = numberOfDocs * 50000;
        int [] data = new int [maxSize];
        data[0] = numberOfDocs;
        int base = numberOfDocs*3+1;
        int counter = base;
		char * save1, * save2;

		char * filename = strtok(list, "\n", &save1);

		for ( int i = 0; filename != null && i < to; i++){
			if ( i < from){
				filename = strtok_r(list, "\n", &save1);
				continue;
			}

			char * wordList = readFile(filename); 

			char * word = strtok_r(wordList, "\n", &save2);
			int numOfWords = 0;
			while(word!= NULL){
				if ( strcmp(word,"") == 0){
					strok (wordList, "\n");
				}
			
			}




			filename = strtok_r(list, "\n", &save1);
		}

        for ( int i = from ; i < files.length && i < to; i++){
                
                
                char * words[] = readFile(filename).split("[\n]");

                int bookLen = words.length;
                if ( words[0].split("[\t]")[0].equals("")){ // the first token may be an empty string, ignore it
                   bookLen--;
                }
                data[i+1] = i;
                data[numberOfDocs + i + 1] = bookLen;
                data[numberOfDocs*2 + i + 1] = bookLen;

                for ( int j = 0 ; j < words.length; j++){
                     char * word[] = words[j].split("[\t]");
                     if ( !word[0].equals("") ) {
                        int hashCode = word[0].hashCode();
                        if ( hashCode < 0 ) {
                            hashCode *= -1;
                        }else if ( hashCode == 0 ) {
                            hashCode = 1; // something positive
                        }                        
                        data[counter] = hashCode;
                        counter++;
                     }
                 }
				
        }

		fwrite(pile, sizeof(int), (numOfDocuments*3 + numOfDocuments*avgDocLength + 1), f);
		fclose(f);
		free(pile);
    }
*/
