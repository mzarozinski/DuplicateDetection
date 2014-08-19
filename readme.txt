UMASS Duplicate Detection Tools (V1.0) 
Copyright (C) 2013 by the University of Massachusetts at Amherst 
released under the GNU  GPL v3.0 (see GNU_license.txt)

Written by I. Zeki Yalniz
Maintained by Michael Zarozinski (MichaelZ@cs.umass.edu)
 
There are two different modules. The first one which generates the binary files called 'barrels' containing the unique word representations for the books in the collection. The other one gets those binary files and run the duplicate detector on them efficiently. 

JAVA MODULE:
==============
The BarrelGenerator.java code simply gets a list of files (full path list which is typed inside the main function of the BarrelGenerator) and a range of line numbers to determine which books to be put into the barrel. 

Compile using: "javac BarrelGenerator.java"

USAGE: java BarrelGenerator -i<inputFilename> -s<startFileIndex> -e<endFileIndex> -c<configFile> -o<outputFilename>

* <inputFilename> is the text file that contains a list of files to process (including path)
* <startFileIndex> (optional) the row to start at in the input file. If not specified, the whole file is processed. 
* <endFileIndex> (optional) the row to end before in the input file. If not specified, the whole file is processed. 
* <outputFilename> (optional) is the file name for the output
* <configFile> (optional) file can contain the following arguments on each line:
        * ignorePunctuation=<TRUE|FALSE> (indicates if punctuations should be ignored. Default is TRUE)
        * ignoreNumbers=<TRUE|FALSE> (indicates if numeric characters should be ignored. Default is TRUE)

For example: 

	java BarrelGenerator -i/tmp/fileList.txt -s2000 -e4000  

Will create a barrel named barrel_2000.bar which contains the books pointed to on lines 2,000 through 4,000 found in fileList.txt.
 
It is recommended you limit the size of your barrel with 2000 books.  


C MODULE
============
This module runs the duplicate detector for the barrels containing the books. To compile the code, use the following line since the code optimization flag make the code run several times faster than usual compilation rules:

g++ -O3 -o DuplicateDetector Main.cpp {DocPile,MyHashTable,DuplicateDetector}.cpp

There are two similarity that can be used:  DUPNIQ-its or DUPNIQ-cs. The default is ITS, to CS, comment out the "#define ITS" line in DuplicateDetector.cpp.

Refer to the actual paper (http://ciir-publications.cs.umass.edu/pub/web/getpdf.php?id=970) for their exact meaning of those functions (we also provide the thresholds for DUPNIQ-its and DUPNIQ-cs). There is also a filtering statement which is to avoid aligning books which does not contain sufficient number of common unique terms for being partial duplicates. The filtering threshold is calculated based on the similarity score threshold (see the code for details). You should turn the filtering on for performance reasons.  

USAGE:

	DuplicateDetector <docpile1> <docpile2> <outputfile> <threshold>
	OR
	DuplicateDetector <docpile1> <outputfile> <threshold>

The first will look for duplicates between "docpile1" and "docpile2". The second will search for duplicates within "docpile1".

The recommended thresholds are 0.12 and 0.72 for DUPNIQ-cs and DUPNIQ-its respectively
