
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author zeki
 */
public class BarrelGenerator {

  //  static String uniquesFolder = "/lustre/work3/manmatha/efcan/Dupp_Data/English/Clean_Large_Set_English_Unq/";
    public static void main ( String args[] ){
    /*    int u1 = 11223;
        int u2 = 19162;
        int c = 630; 	
        int lcs = 3226;
     //   calculateScore(u1,u2,c,lcs);
        double score = 0.22;
     //   calculateBound2( u1 , u2 , score);
      */
   //     String word = "aof";
     //   System.out.println(word.hashCode() + " -> " + littleEndian2(word.hashCode()));
     //   System.out.println(word.hashCode() + " -> " + littleEndian2(littleEndian2(word.hashCode())));
     //   System.out.println(Integer.toBinaryString(littleEndian2(littleEndian2(word.hashCode()))) + " " + Integer.toBinaryString(littleEndian2(word.hashCode())) + "\n"+ Integer.toBinaryString(word.hashCode()));
       // checkHashCode();
    //    int i = 2^10 + 2^5 + 2^20;

     //   System.out.println("yokeeee".hashCode()+ " - " + getHashCode("yokeeee"));


        
   //     byte[] r = littleEndian3(i);
   //     int a = littleEndian3b(r);
   //     a = 147808000;
   //     System.out.println(Integer.reverseBytes(a) + " - "+ littleEndian2(a));
   //     System.out.println(Integer.reverseBytes(Integer.reverseBytes(a)));

   //     System.out.println(i + " " + Integer.toBinaryString(i));
    //    System.out.println(Integer.toBinaryString(a));
    //    System.out.println(Byte.toString(r[0]) + " " + Byte.toString(r[1])  + " "+ Byte.toString(r[2])+ " "  +Byte.toString(r[3]) );

        //System.out.println(word.hashCode() + " -> " + littleEndian(word.hashCode()) & 0xFF);

       // generateQsubLines();
       // generateQsubLinesDetector();
        
      //  changeFormat("100k_list.txt", "CS_RESULTS.txt", "CS_RESULTS_PROCESSED.txt");
      //  changeFormat("100k_list.txt", "ITS_RESULTS.txt", "ITS_RESULTS_PROCESSED.txt");
      //    changeFormat("100k_list.txt", "ITS_2000_LIMIT.txt", "ITS_2000_LIMIT_PROCESSED2.txt");
      //  return;
        
        int from = Integer.parseInt(args[0]);
        int to = Integer.parseInt(args[1]);

      //  System.out.println(args[0] + " " + args[1] );
        binarizeFiles("/lustre/work3/manmatha/efcan/Dupp_Data/English/ListFiles/100k_files.txt", "/lustre/work3/manmatha/zeki/DuplicateDetectionC/barrel_" + args[0] + ".bar", from, to);
       // System.out.println("!!!! done");

    }

/*
    public static void changeFormat(String fileList, String processingResult, String outputFile){

        String [] lines = TextPreprocessor.readFile(fileList).split("\\s+");
        String [] output = TextPreprocessor.readFile(processingResult).split("[\n]");
        StringBuilder result = new StringBuilder();
        for ( int i = 0; i < output.length; i++ ) {
            String col [] = output[i].split("[\t]");

            int index = Integer.parseInt(col[0]);
            int index2 = Integer.parseInt(col[1]);

            String out = lines[index] + "\t" + lines[index2] + "\t";
            for ( int j = 2 ; j < 5; j++ ) {
                out += col[j] + "\t";
            }
            out += col[ col.length-1 ] + "\n";
            
            result.append(out);
        }
        try {
            new FileWriter(new File(outputFile)).append(result.toString()).close();
        } catch (IOException ex) {
            
        }
    }



    public static void changeFormat2(String fileList, String processingResult, String outputFile){

        String [] lines = TextPreprocessor.readFile(fileList).split("\\s+");
        String [] output = TextPreprocessor.readFile(processingResult).split("[\n]");
        StringBuilder result = new StringBuilder();
        for ( int i = 0; i < output.length; i++ ) {
            String col [] = output[i].split("[\t]");

            int index = Integer.parseInt(col[0]);
            int index2 = Integer.parseInt(col[1]);

            //String out = lines[index] + "\t" + lines[index2] + "\t";
            String out = extractFilename(lines[index]) + "\t" + extractFilename(lines[index2]) + "\t";

            for ( int j = 2 ; j < 5; j++ ) {
                out += col[j] + "\t";
            }
            out += col[ col.length-1 ] + "\n";

            result.append(out);
        }
        try {
            new FileWriter(new File(outputFile)).append(result.toString()).close();
        } catch (IOException ex) {

        }
    }
    */
    public static String extractFilename(String s){
        if ( s == null || s.equals("")){
            return "";
        }else{
            int start = s.lastIndexOf("\\") + 1;
            if ( start == 0 ){
                start = s.lastIndexOf('/') + 1;
            }

            int end = s.lastIndexOf('.');
            if ( end == -1){
                end = s.length();
            }
            return s.substring(start, end);
        }
    }

    public static void generateQsubLines(){

        int jobsPerComp = 2000;
        int totalJobs = 104000;

        StringBuilder builder = new StringBuilder();
        for ( int i = 0 ; i < totalJobs; i= i + jobsPerComp ){
            builder.append("qsub -b y -cwd java BarrelGenerator " + i + " " + (i + jobsPerComp) + "\n");
        }
        try {
//            new FileWriter(new File("/lustre/work3/manmatha/zeki/DuplicateDetectionC/qsub_commands_100K_dataset.sh")).append(builder.toString()).close();
             new FileWriter(new File("qsub_barrels_100K_dataset.sh")).append(builder.toString()).close();
        } catch (IOException ex) {
        }
    }

    public static void generateQsubLinesDetector(){

        int jobsPerComp = 2000;
        int totalJobs = 104000;

        StringBuilder builder = new StringBuilder();
        for ( int i = 0 ; i < totalJobs; i= i + jobsPerComp ){
            builder.append("qsub -b y -cwd ./DuplicateDetector /lustre/work3/manmatha/zeki/DuplicateDetectionC/barrel_" + i + ".bar " + i + ".txt 0.1\n");
        }

        for ( int i = 0 ; i < totalJobs; i= i + jobsPerComp ){
            for ( int j = i ; j < totalJobs; j= j + jobsPerComp ){
                if ( i == j ){continue;}
                builder.append("qsub -b y -cwd ./DuplicateDetector /lustre/work3/manmatha/zeki/DuplicateDetectionC/barrel_" + i + ".bar /lustre/work3/manmatha/zeki/DuplicateDetectionC/barrel_" + j + ".bar " + i + "_" + j + ".txt 0.1\n");
            }
       }

        try {
            new FileWriter(new File("qsub_detector_commands_100K_dataset.sh")).append(builder.toString()).close();

        //    new FileWriter(new File("/lustre/work3/manmatha/zeki/DuplicateDetectionC/qsub_commands_100K_dataset.sh")).append(builder.toString()).close();
        } catch (IOException ex) {
        }
    }

    public static void binarizeFiles(String fileList, String outFile, int from, int to){
        String list =  readFile(fileList);
        String [] files = list.split("[\n]");
        int numberOfDocs = (to-from);
        DataOutputStream os;
        try {

            int maxSize = numberOfDocs * 50000;
            int [] data = new int [maxSize];
            data[0] = numberOfDocs;
            int base = numberOfDocs*3+1;
            int counter = base;

         //   int numOfBooks = 0;
            for ( int i = from ; i < files.length && i < to; i++ ){
                String filename = files[i].trim();
                System.out.println(files[i]);
                String words[] = readFile(filename).split("[\n]");

                int bookLen = 0;
              //  if ( words[0].split("[\t]")[0].equals("")){ // the first token may be an empty string, ignore it
               //    bookLen--;
              //  }
                for ( int j = 0 ; j < words.length; j++){
                     String word[] = words[j].split("[\t]");
                  //   if ( !word[0].equals("") ) {
                        int hashCode = getHashCode(word[0]);
                        if ( hashCode < 0 ) {
                            hashCode *= -1;
                        }else if ( hashCode == 0 ) {
                            hashCode = 1; // something positive
                        }                        
                        data[counter] = hashCode;
                        counter++;
                        bookLen++;
                  //   }
                 }

                int index = (i-from);
                data[index+1] = i;
                data[numberOfDocs + index + 1] = bookLen;
                data[numberOfDocs*2 + index + 1] = bookLen;

            }
       //     data[0] = (i-from);


            // write the file
            os = new DataOutputStream(new FileOutputStream(outFile));
           // BufferedOutputStream buf = new BufferedOutputStream(os);
        //    for (int i = 0; i < data.length && i < counter; i++ ) {
               // os.writeInt ( littleEndian2(data[i]));
                // TODO: change the hash function
              //  System.out.println(data[i]);
        //        os.writeInt ( Integer.reverseBytes(data[i]));
                //  buf.write ( Integer.reverseBytes(data[i]) );
         //   }

            // create a byte array
            byte [] b = new byte[4*counter];
            for (int i = 0; i < data.length && i < counter; i++ ) {
                int index = i*4;
                b[index] =   (byte) (data[i] & 0x000000FF);
                b[index+1] = (byte) ((data[i] & 0x0000FF00) >>>  8 );
                b[index+2] = (byte) ((data[i] & 0x00FF0000) >>> 16 );
                b[index+3] = (byte) ((data[i] & 0xFF000000) >>> 24 );
            }
            os.write(b);
          //  buf.flush();
          //  buf.close();
            os.close();
           // System.out.println(counter + "\t" + data[0]);

        } catch (Exception ex) {
        }
    }

    public static void checkHashCode(){
        String filename = "stonesquarries00howerich_djvu.xml.data";
        String words[] = readFile(filename).split("[\n]");

                int bookLen = words.length;
                if ( words[0].split("[\t]")[0].equals("")){ // the first token may be an empty string, ignore it
                   bookLen--;
                }
             //   int index = (i-from);
             //   data[index+1] = i;
             //   data[numberOfDocs + index + 1] = bookLen;
             //   data[numberOfDocs*2 + index + 1] = bookLen;
                int [] data = new int[bookLen];
                for ( int j = 0 ; j < words.length; j++){
                     String word[] = words[j].split("[\t]");
                     if ( !word[0].equals("") ) {
                        int hashCode = word[0].hashCode();
                        if ( hashCode < 0 ) {
                            hashCode *= -1;
                        }else if ( hashCode == 0 ) {
                            hashCode = 1; // something positive
                        }
                        //data[j] = Integer.reverseBytes(hashCode);
                        data[j] = hashCode;
                        System.out.println(j + " -> " + data[j]);
                     }
                 }
               // Integer.toBinaryString(1206629809);
               // Integer.toBinaryString(108004200);


    }

    public static int getHashCode(String s) {
	int h = 0;
	int off = 0;

        int len = s.length();
        char val[] = new char[len];
        s.getChars(0, len, val, 0);

        for (int i = 0; i < len; i++) {
           h = 31*h + val[off++];
        }
        return h;
    }


   public static int littleEndian2(int a){

        int r1 = ((int)a & 0x000000FF) << 24;
        int r2 = ((int)a & 0x0000FF00) << 8;
        int r3 = ((int)a & 0x00FF0000) >>> 8;
        int r4 = ((int)a & 0xFF000000) >>> 24;

        return (int)(r1 | r2 | r3 | r4);
    }


    public static void calculateScore(int u1 , int u2 , int c, int lcs){
        double score = Math.log(lcs)/Math.log(u1+u2-lcs);
        System.out.println(score);
    }
    public static void calculateBound(int u1 , int u2 , double score){

        double t = Math.exp(score * Math.log(u1+u2) );
        double t2 =  Math.exp(score * Math.log(u1+u2-t) );
        System.out.println(t + " " + t2);
    }

    public static void calculateBound2(int u1 , int u2 , double score){

        double t = score * Math.sqrt((double)u1*(double)u2) ;
        System.out.println(t);
    }



    public static String readFile(String filename) {

        File file = new File(filename);
        return readFile(file);
    }

    public static String readFile(File file) {

        if ( file == null || !file.exists()) { return null;}

        StringBuffer bufc = null;
        FileReader fr = null;

        try {
            fr = new FileReader(file);

            int theChar;
            bufc = new StringBuffer();

            while (((theChar = fr.read()) != -1)) {
                bufc.append((char) theChar);
            }
        } catch (IOException ioe) {
            System.out.println("TextPreprocessor: Can not read the file "  + file.getAbsolutePath());
        } finally {
            if (fr != null) {
                try {
                    fr.close();
                } catch (IOException ioe) {
                }
            }
        }
        fr = null;
        return bufc.toString();
    }
}
