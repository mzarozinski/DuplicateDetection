#ifndef _DOC_PILE_H_
#define _DOC_PILE_H_

class DocPile
{
public:
	int numOfDocs;
	int * uniqueTerms; 
	int * docIDs;
	int * docLengths; 
	int * listLengths;
	int * data;

	DocPile(char * f);
	void readPile(char *file);
	~DocPile(void);
};

#endif
