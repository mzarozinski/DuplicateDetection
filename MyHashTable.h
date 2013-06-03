#ifndef _MY_HASHTABLE_H_
#define _MY_HASHTABLE_H_
#include "headers.h"

class MyHashTable
{
	// static hash table storage
	static int ** hashTable;
	static int * buckets;
	static char * accessed; // a list of flags indicating whether an element in the hash table is accessed or not. 

	// lists of common elements sorted according to their original order.
	// they are singleton in order to make use of the same memory block over and over again
	static int *s1; 
	static int *s2; 

	// keep the original list of keys hashed.
	int numOfHashedKeys;
	int * hashedSequence;
	

	// params
//	int HASH_TABLE_SIZE;
//	int BUCKET_SIZE ;
	
public:

	MyHashTable(); // recommended hash table size is 10000, bucketsize 20.
	void cleanAccessedElements(); // reset flags and the table.
	void putAll(int* keys, int count); // same as put, but more efficient avoiding function calls
	int isElement(int key);  // test whether the element exists in the table or not.
	void intersect(int * keys, int keys_size, int *list_size, int ** seq_out1, int ** seq_out2);
	static void freeMemory();
	~MyHashTable(void); // deconstructor
};


#endif