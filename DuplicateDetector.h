//#pragma once
#include "headers.h"

#ifndef _DUP_DETECTOR_H_
#define _DUP_DETECTOR_H_

class DocPile;

class DuplicateDetector
{
	char * filename1;
	char * filename2;
	char * outfilename;
	double dupThreshold;
	DocPile *p1;
	DocPile *p2;
	void detectDuplicatesInThePile();
	void detectDuplicatesBetweenPiles();
 


	int * X;
	int * Y;

	// output purposes
	ofstream outfile;
	char charbuffer[1000];

public:
	DuplicateDetector(char *file1, char *file2, char *outfilename, double threshold);	
	DuplicateDetector(char *file1, char *outfilename, double threshold);
	int LCS(int * s1, int * s2, int M, int N);
	int LCS_fast(int * s1, int * s2, int M, int N, int minLCS);
	void detectDuplicates();	
	~DuplicateDetector(void);
};


#endif
