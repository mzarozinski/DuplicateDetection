/*

Duplicate detector for Books using unique word representation.

Written by Ismet Zeki Yalniz
Center for Intelligent Information Retrieval
Dept. of Computer Science
U. of Massachusetts-Amherst
email:zeki@cs.umass.edu

 */

#define ITS

#include "headers.h"
#include "DuplicateDetector.h"
#include "DocPile.h"
#include "MyHashTable.h"

DuplicateDetector::DuplicateDetector(char *file1, char *file2, char *out, double t) {
    p1 = new DocPile(file1);
    p2 = new DocPile(file2);

    // p1 is the pile which has fewer number of books. In this way we initialize less number of hash tables.
    if (p1->numOfDocs > p2->numOfDocs) {
        DocPile * temp = p2;
        p2 = p1;
        p1 = temp;
    }

    outfilename = out;
    dupThreshold = t;

    // statically allocate space for the dynamic programming table
    X = (int *) calloc(2 * (MAX_NUMBER_OF_UNIQUE_WORDS + 1), sizeof (int));
    Y = &X[MAX_NUMBER_OF_UNIQUE_WORDS + 1];

    // open the output file
    outfile.open(outfilename, ios::out);
}

DuplicateDetector::DuplicateDetector(char *file1, char *out, double t) {
    p1 = new DocPile(file1);
    p2 = NULL;
    outfilename = out;
    dupThreshold = t;

    // statically allocate space for the dynamic programming table
    X = (int *) calloc(2 * (MAX_NUMBER_OF_UNIQUE_WORDS + 1), sizeof (int));
    Y = &X[MAX_NUMBER_OF_UNIQUE_WORDS + 1];

    // open the output file
    outfile.open(outfilename, ios::out);
}

DuplicateDetector::~DuplicateDetector(void) {
    if (p1 != NULL)
        delete p1;
    if (p2 != NULL)
        delete p2;

    // dont forget to flush the data.
    outfile.flush();
    outfile.close();
} 
// MCZ 7/2013 - merged detectDuplicatesBetweenPiles() and detectDuplicatesInThePile()
void DuplicateDetector::detectDuplicates() {

    int * data = p1->data;
    // if we only have one "pile" set data2 to point to it, else
    // point to the 2nd pile.
    int * data2 = (p2 == NULL ? p1->data : p2->data);

    const int base_index = p1->numOfDocs * 3 + 1;
    // if we're only using 1 pile, set "base_index2" to the same as "base_index",
    // otherwise use the actual offset for the 2nd pile.
    const int base_index2 = (p2 == NULL ? base_index : p2->numOfDocs * 3 + 1);
 
    int running_index = 0, next_running_index = 0, listLength, docLength, otherDocLength, otherListLength, *list, *otherList, *seq1, *seq2, common, T;
    double LCSlength = 0.0;
    int statPassed = 0;
    for (int i = 0; i < p1->numOfDocs; i++) {

        listLength = p1->listLengths[i];
        docLength = p1->docLengths[i];
        list = &data[ base_index + running_index ];
        running_index += listLength;

        if (listLength == 0) {
            printf("DocID = %d is empty, where i = %d\n", p1->docIDs[i], i);
            continue;
        }

        MyHashTable *table = new MyHashTable();
        table->putAll(list, listLength);

        // if we have 2 piles, reset the next running index to zero, we want
        // to go through the WHOLE 2nd pile and compare with the 1st
        next_running_index = (p2 == NULL ? running_index : 0);
        int start_index = (p2 == NULL ? (i + 1) : 0);
        DocPile *second_pile = (p2 == NULL ? p1 : p2);
        for (int j = start_index; j < second_pile->numOfDocs; j++) {

            otherListLength = second_pile->listLengths[j];
            if (otherListLength == 0) {
                continue;
            }

            otherDocLength = second_pile->docLengths[j];
            otherList = &data2[base_index2 + next_running_index ];
            next_running_index += otherListLength;

            table->intersect(otherList, otherListLength, &common, &seq1, &seq2);

            // CALCULATE THE MINIMUM NUMBER OF COMMON WORDS REQUIRED FOR A PAIR TO BE A DUPLICATE. USE IT FOR FILTERING NEGATIVE PAIRS BEFORE ACTUAL ALIGNMENT (LCS)	
#ifdef ITS					
            //1 - based on ITS score (note: the bound is not tightest. But It does not miss any true positive)
            int T_large = (int) exp(dupThreshold * log((double) (otherListLength + listLength))); // this statement overestimates T
            T = (int) exp(dupThreshold * log((double) (otherListLength + listLength - T_large))); // it underestimates T and its OK. Because T_large is larger than the actual LCS length necessary to classify these books as duplicates tuple. Therefore T is smaller than but close to the optimal threshold. 
#else
            //2 - based on CS score (use real length)
            T = (int) (dupThreshold * sqrt((double) otherListLength * (double) listLength));
#endif			

            // filtering
            if (common >= T) {
                statPassed++;

                // run LCS for the top K=2000 words for efficiency.	
                if (common <= MAX_NUMBER_OF_UNIQUE_WORDS) {
                    LCSlength = LCS(seq1, seq2, common, common);
                } else {
                    LCSlength = LCS(seq1, seq2, MAX_NUMBER_OF_UNIQUE_WORDS, MAX_NUMBER_OF_UNIQUE_WORDS);
                    double ratio = (double) common / MAX_NUMBER_OF_UNIQUE_WORDS;
                    LCSlength = (int) (ratio * LCSlength);
                }

                // CALCULATE THE MINIMUM NUMBER OF COMMON WORDS REQUIRED FOR A PAIR TO BE A DUPLICATE. USE IT FOR FILTERING NEGATIVE PAIRS BEFORE ACTUAL ALIGNMENT (LCS)				
#ifdef ITS
                // ITS score
                double score = log(LCSlength) / log((double) (otherListLength + listLength - LCSlength));
#else
                // CS score
                double score = LCSlength / sqrt((double) otherListLength * (double) listLength);
#endif
                if (score >= dupThreshold) {
                    // output the book pair as duplicates
                 //   sprintf(charbuffer, "%d\t%d\t%d\t%d\t%d\t%d\n", p1->docIDs[i], second_pile->docIDs[j], docLength, otherDocLength, common, (int) LCSlength);
                      sprintf(charbuffer, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p1->docIDs[i], second_pile->docIDs[j], docLength, otherDocLength, listLength, otherListLength, common, (int) LCSlength);
                    outfile << charbuffer;
                }
            }
        }
    }
    printf("Passed = %d\n", statPassed);
    outfile.flush();
}

int DuplicateDetector::LCS(int * s1, int * s2, int M, int N) {

    // clean the dynamic programming table
    memset(X, 0, 2 * (MAX_NUMBER_OF_UNIQUE_WORDS + 1) * sizeof (int));

    // compute length of LCS and all subproblems via dynamic programming
    for (int i = M - 1; i >= 0; i--) {
        for (int j = N - 1; j >= 0; j--) {
            if (s1[i] == s2[j])
                X[j] = 1 + Y[j + 1];
            else {
                if (Y[j] > X[j + 1]) {
                    X[j] = Y[j];
                } else {
                    X[j] = X[j + 1];
                }
            }
        }
        Y = X;
    }
    return X[0];
}

int DuplicateDetector::LCS_fast(int * s1, int * s2, int M, int N, int minimumLCS) {

    // clean the dynamic programming table
    memset(X, 0, 2 * (MAX_NUMBER_OF_UNIQUE_WORDS + 1) * sizeof (int));

    // compute length of LCS and all subproblems via dynamic programming
    for (int i = M - 1; i >= 0; i--) {
        for (int j = N - 1; j >= 0; j--) {
            if (s1[i] == s2[j])
                X[j] = 1 + Y[j + 1];
            else {
                if (Y[j] > X[j + 1]) {
                    X[j] = Y[j];
                } else {
                    X[j] = X[j + 1];
                }
            }
        }
        // if the LCS is ensured to be less than minimumLCS, then quit the dynamic programming and return whatever is observed so far.
        if ((minimumLCS - X[0]) > (i + 1)) {
            break;
        }
        Y = X;
    }
    return X[0];
}
