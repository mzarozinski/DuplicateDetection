Please compile duplicate detector using the following command:

g++ -O3 -o DuplicateDetector Main.cpp {DocPile,MyHashTable,DuplicateDetector}.cpp










You can access the code on Swarm under the folder: 
/lustre/work3/manmatha/zeki/DuplicateDetectionC/

There are two different modules. The first one which generates the binary files called 'barrels' containing the unique word representations for the books in the collection.The other one gets those binary files and run the duplicate detector on them efficiently. 

JAVA MODULE:
==============
The BarrelGenerator.java code simply gets a list of files (fullpath list which is typed inside the main function of the BarrelGenerator) and a range of line numbers to determine which books to be put into the barrel. You can easily parallelize the barrel generation phase using several instances of the BarrelGenerator program on Swarm. For example,

qsub -b y -cwd java BarrelGenerator 0 2000
qsub -b y -cwd java BarrelGenerator 2000 4000
qsub -b y -cwd java BarrelGenerator 4000 6000
qsub -b y -cwd java BarrelGenerator 6000 8000
....

It will generate binary files named as follows: barrel_0.bar, barrel_2000.bar, barrel_4000.bar etc. 

I recommend you limit the size of your barrel with 2000 books. The BarrelGenerator code (a single java file) is under: 

/lustre/work3/manmatha/zeki/DuplicateDetectionC/BarrelGenerator.java


C MODULE
============
This module runs the duplicate detector for the barrels containing the books. To compile the code, use the following line since the code optimization flag make the code run several times faster than usual compilation rules:

g++ -O3 -o DuplicateDetector Main.cpp {DocPile,MyHashTable,DuplicateDetector}.cpp

There are two functions you may need to modify based on which similarity function you use (DUPNIQ-its or DUPNIQ-cs). You can refer to the actual paper for their exact meaning of those functions (we also provide the thresholds for DUPNIQ-its and DUPNIQ-cs). There is also a filtering statement which is to avoid aligning books which does not contain sufficient number of common unique terms for being partial duplicates. The filtering threshold is calculated based on the similarity score threshold (see the code for details). You should turn the filtering on for performance reasons.  

DuplicateDetector::detectDuplicatesInThePile()
DuplicateDetector::detectDuplicatesBetweenPiles()

You may also limit or reformat the output files. You do not want to output the similarity scores for each pair. This would require a large storage. Therefore I recommend you to output only those which are considered to be duplicates. The duplicate detector code can run in parallel using the grid engine using: 

qsub -b y -cwd barrel_0.bar 0.txt 0.1 

where "0.txt" is the output file and the last parameter is the similarity threshold. The duplicate detector must be run for pairs of barrels as well as follows:  

qsub -b y -cwd barrel_0.bar barrel_2000.bar 0_2000.txt 0.1

All the necessary code to generate qsub scripts and other manipulations are in BarrelGenerator.java file. 

If you have any questions, please let me know. 

I. Zeki Yalniz
CIIR, UMASS, Amherst
zeki@cs.umass.edu
