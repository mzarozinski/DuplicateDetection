/*

An efficient hashtable implementation for the duplicate detection problem.

Written by Ismet Zeki Yalniz
Center for Intelligent Information Retrieval
Dept. of Computer Science
U. of Massachusetts-Amherst
email:zeki@cs.umass.edu

 */

#include "MyHashTable.h"
#include "headers.h"


// initialize the static members 
int * MyHashTable::s1 = (int *) calloc(MAX_NUMBER_OF_UNIQUE_WORDS, sizeof (int));
int * MyHashTable::s2 = (int *) calloc(MAX_NUMBER_OF_UNIQUE_WORDS, sizeof (int));
int ** MyHashTable::hashTable = (int **) calloc(HASH_TABLE_SIZE, sizeof (int*));
extern int BLOCK_SIZE = (BUCKET_SIZE + 1)*2;
int * MyHashTable::buckets = (int *) calloc(HASH_TABLE_SIZE*BLOCK_SIZE, sizeof (int));
char * MyHashTable::accessed = (char *) calloc(MAX_DOC_SIZE, sizeof (char));

// system calls are avoided. 

MyHashTable::~MyHashTable(void) {

    // first free all extended buckets
    /*	for ( int i = 0 ; i < HASH_TABLE_SIZE; i++){
                    int * bucket = hashTable[i];
                    if ( bucket[0] > BUCKET_SIZE ){
                            free(bucket); 
                    }	
            }
     */

    // free the hashtable
    //	free(hashTable);
    // free the array buckets.
    //	free(buckets);
    // free the accessed flags.
    //	free(accessed);
    // free lists of common words
    //	free(s1); 
}

// for duplicate detection, recommended hash table size is between 5000 to 15000 and bucketsize 10.

MyHashTable::MyHashTable() {

    // system calls are avoided by making these variables static
    //	hashTable = (int **) calloc(HASH_TABLE_SIZE, sizeof(int*));
    //	int BLOCK_SIZE = (BUCKET_SIZE+1)*2;
    //	buckets = (int *)calloc(HASH_TABLE_SIZE*BLOCK_SIZE, sizeof(int)); 

    // The first element of a bucket tells the size of it, the second one tells its occupancy
    // then the [key, value] pairs follow this data in the list

    // reset the table for the next one. 
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        // delete the bucket if it was elongated in the previous run.
        int * bucket = hashTable[i];
        if (bucket != NULL && bucket[0] > BUCKET_SIZE) { // if the bucket capacity was improved, release its memory
            free(bucket);
        }
        // reset the buckets
        buckets[i * BLOCK_SIZE] = BUCKET_SIZE; // capacity 
        buckets[i * BLOCK_SIZE + 1] = 0; // size - empty
        hashTable[i] = &buckets[i * BLOCK_SIZE];
    }

    // singleton 
    //	if (s1 == NULL){
    // one time allocation of these lists help speed up the whole intersection process
    //		s1 = (int *) calloc(2* MAX_NUMBER_OF_UNIQUE_WORDS, sizeof(int));
    //		s2 = &s1[MAX_NUMBER_OF_UNIQUE_WORDS];
    //	}
}


// same as put, but more efficient avoiding function calls

void MyHashTable::putAll(int* keys, int count) {

    int loc, *bucket, curBucketSize, curBucketOccupancy, newCapacity, *newBucket;
    hashedSequence = keys;
    numOfHashedKeys = count;
    //accessed = (char *) calloc(count, sizeof(char));

    for (int i = 0; i < count; i++) {
        int key = keys[i];

        //locate the bucket
        loc = key % HASH_TABLE_SIZE;
        bucket = hashTable[loc];
        curBucketSize = bucket[0];
        curBucketOccupancy = bucket[1];

        // if the bucket is full, then
        if (curBucketOccupancy == curBucketSize) {

            // increase the size of the bucket by a factor of two
            newCapacity = curBucketSize * 2;
            newBucket = (int *) calloc(newCapacity + 2, sizeof (int)); // there is a problem here. new Bucket returns NULL sometimes on both Linux and windows. dont know why.
            if (newBucket == NULL) {
                printf("Cannot allocate memory in MyHashTable.cpp\n");
                exit(1);
            }

            memcpy(newBucket, bucket, curBucketSize + 2);

            // clean the old buckets from the memory
            if (curBucketSize > BUCKET_SIZE) {
                free(bucket);
            }

            newBucket[0] = newCapacity;
            hashTable[loc] = newBucket;
            bucket = newBucket;
        }

        //add the key and value into the list
        bucket[2 + curBucketOccupancy * 2] = key;
        bucket[3 + curBucketOccupancy * 2] = i; // keep the index of this element in the original sequence
        bucket[1]++;
    }
}

//int MyHashTable::isElement(int key){  // test whether the element exists in the table or not.
//	return 1;
//}

void MyHashTable::intersect(int * keys, int keys_length, int *list_size, int ** seq_out1, int ** seq_out2) {

    // we dont even need to reset this static array. Its values will be overwritten
    //memset(s1,0,sizeof(int)*MAX_NUMBER_OF_UNIQUE_WORDS*2);

    int common = 0;
    int key, loc, *bucket, curBucketSize, curBucketOccupancy;

    for (int i = 0; i < keys_length; i++) {
        // early termination case - may be implemented but not improving the performance in our case.
        //if ( ){}

        // test whether the key is in the hashTable or not		
        key = keys[i];
        loc = key % HASH_TABLE_SIZE;
        bucket = hashTable[loc];
        curBucketSize = bucket[0];
        curBucketOccupancy = bucket[1];

        for (int j = 0; j < curBucketOccupancy; j++) {
            // if the key is there, then turn the accessed flag on
            if (bucket[j * 2 + 2] == key) {
                if (common < MAX_NUMBER_OF_UNIQUE_WORDS) {
                    accessed[bucket[j * 2 + 3]] = 1;
                    s1[common] = key;
                }
                common++;
                break;
            }
        }
    }

    // iterate over accessed elements to recover the second sequence
    int curCount = 0;
    for (int i = 0; i < numOfHashedKeys; i++) {
        if (accessed[i] == 1) {
            accessed[i] = 0; // reset to the original value for the next intersection task
            s2[curCount] = hashedSequence[i]; // output the integer
            curCount++;
        }
    }

    // output the lists
    *seq_out1 = s1;
    *seq_out2 = s2;
    list_size[0] = common; // output the length of the list
}

void MyHashTable::freeMemory() {
    //fflush(outfile);
    free(buckets);
    free(hashTable);
    free(accessed);
    free(s1);
}
