/*

Duplicate detector for Books using unique word representation.

Written by Ismet Zeki Yalniz
Center for Intelligent Information Retrieval
Dept. of Computer Science
U. of Massachusetts-Amherst
email:zeki@cs.umass.edu

 */
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

void DuplicateDetector::detectDuplicates() {
    if (p2 == NULL) {
        detectDuplicatesInThePile();
    } else {
        detectDuplicatesBetweenPiles();
    }
}

void DuplicateDetector::detectDuplicatesInThePile() {

    MyHashTable * table;
    int * data = p1->data;

    int base_index = p1->numOfDocs * 3 + 1;
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

        table = new MyHashTable();
        table->putAll(list, listLength);

        next_running_index = running_index;
        for (int j = (i + 1); j < p1->numOfDocs; j++) {

            otherListLength = p1->listLengths[j];
            if (otherListLength == 0) {
                continue;
            }
            otherDocLength = p1->docLengths[j];
            otherList = &data[ base_index + next_running_index ];
            next_running_index += otherListLength;

            table->intersect(otherList, otherListLength, &common, &seq1, &seq2);


            // CALCULATE THE MINIMUM NUMBER OF COMMON WORDS REQUIRED FOR A PAIR TO BE A DUPLICATE. USE IT FOR FILTERING NEGATIVE PAIRS BEFORE ACTUAL ALIGNMENT (LCS)	
#ifdef ITS					
            //1 - based on ITS score (note: the bound is not tightest. But It does not miss any true positive)
            int T_large = (int) exp(dupThreshold * log((double) (otherDocLength + docLength))); // this statement overestimates T
            T = (int) exp(dupThreshold * log((double) (otherDocLength + docLength - T_large))); // it underestimates T and its OK. Because T_large is larger than the actual LCS length necessary to classify these books as duplicates tuple. Therefore T is smaller than but close to the optimal threshold. 
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
                double score = log(LCSlength) / log((double) (otherDocLength + docLength - LCSlength));
#else
                // CS score
                double score = LCSlength / sqrt((double) otherListLength * (double) listLength);
#endif
                if (score >= dupThreshold) {
                    // output the book pair as duplicates
                    sprintf(charbuffer, "%d\t%d\t%d\t%d\t%d\t%d\n", p1->docIDs[i], p1->docIDs[j], docLength, otherDocLength, common, (int) LCSlength);
                    outfile << charbuffer;
                }
            }
        }
    }
    printf("Passed = %d\n", statPassed);
    outfile.flush();
}

void DuplicateDetector::detectDuplicatesBetweenPiles() {

    MyHashTable * table;
    int * data = p1->data;
    int * data2 = p2->data;

    int base_index = p1->numOfDocs * 3 + 1;
    int base_index2 = p2->numOfDocs * 3 + 1;

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

        table = new MyHashTable();
        table->putAll(list, listLength);

        next_running_index = 0;
        for (int j = 0; j < p2->numOfDocs; j++) {

            otherListLength = p2->listLengths[j];
            if (otherListLength == 0) {
                continue;
            }

            otherDocLength = p2->docLengths[j];
            otherList = &data2[ base_index2 + next_running_index ];
            next_running_index += otherListLength;

            table->intersect(otherList, otherListLength, &common, &seq1, &seq2);

            // CALCULATE THE MINIMUM NUMBER OF COMMON WORDS REQUIRED FOR A PAIR TO BE A DUPLICATE. USE IT FOR FILTERING NEGATIVE PAIRS BEFORE ACTUAL ALIGNMENT (LCS)	
#ifdef ITS					
            //1 - based on ITS score (note: the bound is not tightest. But it does not miss any true positive)
            int T_large = (int) exp(dupThreshold * log((double) (otherDocLength + docLength))); // this statement overestimates T
            T = (int) exp(dupThreshold * log((double) (otherDocLength + docLength - T_large))); // it underestimates T and its OK. Because T_large is larger than the actual LCS length necessary to classify these books as duplicates tuple. Therefore T is smaller than but close to the optimal threshold. 
#else
            //2 - based on CS score (use real length)
            // MCZ - This is using total doc length but input is the binary file which just contains 
            // uniq words - think this is an artifact of the barrel gen not using uniq
           // T = (int) (dupThreshold * sqrt((double) otherDocLength * (double) docLength));
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

#ifdef ITS
                // ITS score
                double score = log(LCSlength) / log((double) (otherDocLength + docLength - LCSlength));
#else
                // CS score
                // MCZ - trying list len
               // double score = LCSlength / sqrt((double) otherDocLength * (double) docLength);
               double score = LCSlength / sqrt((double) otherListLength * (double) listLength);
#endif
                if (score >= dupThreshold) {
                    // output the book pair as duplicates
                    sprintf(charbuffer, "%d\t%d\t%d\t%d\t%d\t%d\n", p1->docIDs[i], p2->docIDs[j], docLength, otherDocLength, common, (int) LCSlength);
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
