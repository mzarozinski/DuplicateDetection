
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.regex.Pattern;

/**
 *
 * @author zeki 
 * Modified: 6/2013 Michael Z
 */
public class BarrelGenerator {

    static Pattern ignoreCharsPattern = null; // pre-compiled regex for matching
    static String inFile = null; // file with list of books to process
    static String outFile = null; // name of the file to write the output to
    static Integer startFileIdx = null;    // file to start with (from inFile)
    static Integer endFileIdx = null;    // file to end with (from inFile)

    public static void main(String args[]) throws IOException {

        readCommandLine(args);
        binarizeFiles();

    }

    // write out all the unique words in a book in binary format. Multiple books
    // are written to a single "barrel"
    public static void binarizeFiles() { // String fileList, String outFile, int from, int to) {
        String list = readFile(inFile);
        String[] files = list.split("[\n]");

        // make sure we have a start/end index
        if (startFileIdx == null) {
            startFileIdx = 0;
        }
        if (endFileIdx == null) {
            endFileIdx = files.length; // process whole list
        }
        // make sure, if there is an end index specified, it's not over the
        // actual file count
        endFileIdx = Math.min(endFileIdx, files.length);

        // if no out file was specified, use the input file name (modified a bit)
        if (outFile == null) {
            // "standard" for output files is filename_<startFileIdx>.bar
            outFile = inFile.substring(0, inFile.length() - 4) + "_" + startFileIdx.toString() + ".bar";
        }

        int numberOfDocs = (endFileIdx - startFileIdx);

        try {
            int maxSize = numberOfDocs * 50000;  // This may get resized below if needed
            int[] data = new int[maxSize];
            data[0] = numberOfDocs;
            int base = numberOfDocs * 3 + 1;
            int counter = base;
            int bookLen = 0;
            for (int bookIdx = startFileIdx; bookIdx < files.length && bookIdx < endFileIdx; bookIdx++) {
                String words[] = readFile(files[bookIdx].trim()).split(System.getProperty("line.separator"));

                // create LinkedHashMap (with initial capacity specified) for this book to store the tokens and their frequencies.
                // Using LinkedHashMap as opposed to a HashMap because the order in which the words
                // are encountered is important. HashMap does not guarantee order. 
                // In the future, we might want to be clever when specifying the initial capacity by estimating the 
                // number of uniq words based on the number of lines in the book.
                LinkedHashMap<String, Integer> bookMap = new LinkedHashMap<String, Integer>(2048);

                bookLen = 0; // starting a new book
                int bookUniqWordCount = 0;
                // loop through each line of the book
                for (int j = 0; j < words.length; j++) {

                    // remove any "ignore characters" and extra spaces then split by white space
                    String word[] = ignoreCharsPattern.matcher(words[j]).replaceAll("").trim().split("[\\s+]");

                    // count frequencies of each work in a book
                    for (String token : word) {
                        if (!token.equals("")) {
                            Integer freq = bookMap.get(token);
                            if (freq == null) {
                                bookMap.put(token, 1);
                                bookUniqWordCount++;
                            } else {
                                bookMap.put(token, freq + 1);
                                if (freq == 1) {
                                    bookUniqWordCount--;
                                }
                            }
                            bookLen++;
                        }
                    }

                }// end loop through a book

                // IF writing out they uniq words will blow out the array, 
                // increase the data size
                if (counter + bookUniqWordCount > maxSize - 1) {
                    // for now, just double the memory
                    maxSize = maxSize << 1;
                    int[] tmp = new int[maxSize];
                    System.arraycopy(data, 0, tmp, 0, data.length);
                    data = tmp;
                }
                // get only the tokens with a frequency of 1
                for (Map.Entry<String, Integer> entry : bookMap.entrySet()) {
                    if (entry.getValue().equals(1)) {
                        int hashCode = entry.getKey().hashCode();
                        // TODO would be faster to do:
                        // hashCode &= 0x7FFFFFFF 
                        // but that only ensures positive numbers, doesn't take absolute value, not sure if that's OK
                        if (hashCode < 0) {
                            hashCode *= -1;
                        } else if (hashCode == 0) {
                            hashCode = 1; // something positive
                        }
                        data[counter] = hashCode;
                        counter++;
                    } // end word is unique
                } // end loop through word frequencies

                int index = (bookIdx - startFileIdx);
                data[index + 1] = bookIdx; // doc id
                data[numberOfDocs + index + 1] = bookLen;
                data[numberOfDocs * 2 + index + 1] = bookUniqWordCount; // # uniq words in the doc

            } // end loop through list of files

            // write the file
            DataOutputStream os = new DataOutputStream(new FileOutputStream(outFile));

            // create a byte array
            byte[] b = new byte[4 * counter];
            for (int i = 0; i < data.length && i < counter; i++) {
                int index = i * 4;
                b[index] = (byte) (data[i] & 0x000000FF);
                b[index + 1] = (byte) ((data[i] & 0x0000FF00) >>> 8);
                b[index + 2] = (byte) ((data[i] & 0x00FF0000) >>> 16);
                b[index + 3] = (byte) ((data[i] & 0xFF000000) >>> 24);
            }
            os.write(b);
            os.close();

        } catch (Exception ex) {
            System.err.println(ex.toString());
        }

    } // end binarizeFiles()

    private static String readFile(String filename) {
        BufferedReader in = null;
        StringBuffer buf = new StringBuffer();
        try {
            in = new BufferedReader(new FileReader(filename));
            String line = null;
            while ((line = in.readLine()) != null) {
                if (line.trim().length() == 0) {
                    continue;
                }
                buf.append(line);
                buf.append(System.getProperty("line.separator"));
            }
        } catch (Exception ex) {
            System.err.println("Error reading file: " + filename);
            System.exit(1);
        }
        return buf.toString();
    }

    // function to get any configuration parameters for the program
    private static void readConfigFile(String fileName) throws IOException {

        if (fileName == null) {
            return;
        }

        boolean blnIgnorePunctuation = false;
        boolean blnIgnoreNumbers = false;

        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(new File(fileName)));

            String line = null;

            line = reader.readLine().trim().toLowerCase();

            while (line != null) {
                String[] tokens = line.split("=");
                int numOfArgs = tokens.length;
                if (numOfArgs > 0) {

                    if (tokens[0].equalsIgnoreCase("ignorePunctuation")) {
                        if (line.substring(line.indexOf('=') + 1).equalsIgnoreCase("true")) {
                            blnIgnorePunctuation = true;
                        }
                    } else if (tokens[0].equalsIgnoreCase("ignoreNumbers")) {
                        if (line.substring(line.indexOf('=') + 1).equalsIgnoreCase("true")) {
                            blnIgnoreNumbers = true;
                        }
                    } else {
                        System.err.println("UNKNOWN OPTION: \"" + tokens[0] + "\" ->IGNORING...");
                    }
                } else {
                    System.err.println("Error in the configuration file. Skipping the parameter at line:" + line);
                }

                line = reader.readLine();

            }
        } catch (Exception ex) {
            System.err.println("Error: Can not read configuration file -> " + fileName);
            System.exit(0);
        } finally {
            if (reader != null) {
                reader.close();
            }
        }

        // pre-compile the regex so we only incure the cost once
        String regex = null;
        if (blnIgnorePunctuation && blnIgnoreNumbers) {
            regex = "[^a-zA-Z\\s]";
        } else if (blnIgnorePunctuation) {
            regex = "[^a-zA-Z0-9\\s]";
        } else if (blnIgnoreNumbers) {
            regex = "[0-9]"; // just remove numbers
        } else {
            regex = "";
        }

        try {
            ignoreCharsPattern = Pattern.compile(regex);
        } catch (Exception ex) {
            System.err.println("Error processing characters to ignore: " + ex.toString());
            System.exit(1);
        }

    } // end readConfigFile

    private static void readCommandLine(String args[]) {

        // split the switch and the value
        for (String arg : args) {
            String flag = null;
            String value = null;
            try {
                flag = arg.substring(0, 2);
                value = arg.substring(2);
            } catch (Exception ex) {
                System.err.println("Invalid command line argument: " + arg);
                usage();
                System.exit(1);
            }

			if (flag.toLowerCase().equals("-i")) 
                    inFile = value;
			else if (flag.toLowerCase().equals("-o")) 
                    outFile = value;
			else if (flag.toLowerCase().equals("-s")) 
                    startFileIdx = new Integer(value);
			else if (flag.toLowerCase().equals("-e")) 
                    endFileIdx = new Integer(value);
 			else if (flag.toLowerCase().equals("-c"))  {
                    try {
                        // using same config file scheme as the recursive alignment tool
                        readConfigFile(value.toString());
                    } catch (Exception ex) {
                        System.err.println("Error reading config file: " + ex.toString());
                    }
			} else {
                    System.err.println("Invalid command line switch: " + flag);
                    usage();
                    System.exit(1);
            } 
			
        } // end loop through args

        // make sure we have an input file specified
        if (inFile == null) {
            System.err.println("No input file specified");
            usage();
            System.exit(1);
        }
    } // end readCommandLine

    private static void usage() {
        String USAGE = "\nBarrel Generator Copyright (C) 2013 by the University of Massachusetts at Amherst\n"
                + "\nThis program comes with ABSOLUTELY NO WARRANTY. "
                + "This is free software, and you are welcome to redistribute it under certain conditions;"
                + " see the attached GNU licence for details.\n"
                + "\nUSAGE: java BarrelGenerator -i<inputFilename> -s<startFileIndex> -e<endFileIndex> -c<configFile> -o<outputFilename>\n\n"
                + "<inputFilename> is the text file that contains a list of files to process (including path)\n"
                + "<startFileIndex> the row to start at in the input file\n"
                + "<endFileIndex> the row to end before in the input file\n"
                + "<outputFilename> (optional) is the filename for the output\n"
                + "<configFile> (optional) file can contain the following arguments on each line:\n"
                + "\tignorePunctuation=<TRUE|FALSE> (indicates if punctuations should be ignored. Default is TRUE)\n"
                + "\tignoreNumbers=<TRUE|FALSE> (indicates if numeric characters should be ignored. Default is TRUE)\n";

        System.err.println(USAGE);

    } // end usage()
}
