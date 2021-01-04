import java.io.File;
import java.io.FileNotFoundException;
//import java.io.FileWriter;
//import java.io.IOException;
import java.util.BitSet;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {
        int m,k;    //m - size of bit table, k - number of hash functions
        int hashInteger;
        boolean flag;

        Scanner scan = new Scanner(System.in);
        System.out.println("Enter size of bit table: ");
        m = scan.nextInt();
        System.out.println("\nEnter number of hash functions: ");
        k = scan.nextInt();

        BitSet bitset = new BitSet(m);

        try{
            String toParse;

            //before running the program be sure path to list.txt and check.txt is updated in lines 28 and 29

            File inputList = new File("F:\\programming\\java projects\\mmn14_bloomFilter\\list.txt");
            File checkList = new File("F:\\programming\\java projects\\mmn14_bloomFilter\\check.txt");
            Scanner inputListScanner = new Scanner(inputList);
            Scanner checkListScanner = new Scanner(checkList);


            //filling bit table from list.txt
            //to get hash integer, we are using built-in method .hashCode,
            //it gave better results then murmur.
            //To receive k different hashes, we multiply index of function (0 to k-1) by 100
            //and concatenate key string with result, then getting hash of final string
            //index of hash in bit table = hash mod m
            while(inputListScanner.hasNextLine()){
                toParse = inputListScanner.nextLine();
                if(toParse.trim().length()>0) {
                    String[] parsed = toParse.split(",");
                    for (int i = 0; i < parsed.length; i++) {
                        parsed[i] = parsed[i].trim();
                        for(int hashFuncIndex = 0; hashFuncIndex < k; hashFuncIndex++){
                            hashInteger = Math.abs((parsed[i]+hashFuncIndex*100).hashCode() % m);
                            bitset.set(hashInteger, true);
                        }
                    }
                }
            }

            while(checkListScanner.hasNextLine()){      //checking if the word from check.txt exists in bit table
                toParse = checkListScanner.nextLine();
                if(toParse.trim().length()>0){
                    String[] parsed = toParse.split(",");
                    for(int i = 0; i < parsed.length; i++){
                        parsed[i] = parsed[i].trim();
                        flag = false;
                        for(int hashFuncIndex = 0; hashFuncIndex < k && !flag; hashFuncIndex++){
                            hashInteger = Math.abs((parsed[i]+hashFuncIndex*100).hashCode() % m);
                            if(!bitset.get(hashInteger)){
                                flag = true;

                            }
                        }
                        if(!flag) System.out.println("Key " + parsed[i] + " is in the set");
                        else System.out.println("Key " + parsed[i] + " is not in the set");
                    }
                }

            }
        }catch(FileNotFoundException e){
            System.out.println("\nFile reading error occurred");
            e.printStackTrace();
        }

        //Writing dictionary file list.txt
        //We are getting set of words with length 6, constructed from all combinations of letters {a,b,c,d,e,f}
        //Its 46656 words total
        /*try{
            String str;
            int count = 0;
            File file = new File("F:\\programming\\java projects\\mmn14_bloomFilter\\list.txt");
            FileWriter writer = new FileWriter("F:\\programming\\java projects\\mmn14_bloomFilter\\list.txt");
            for(int a = 0; a < 6; a++){
                for(int b = 0; b < 6; b++){
                    for(int c = 0; c < 6; c++){
                        for(int d = 0; d < 6; d++){
                            for(int e = 0; e < 6; e++){
                                for(int f = 0; f < 6; f++){
                                    str = Character.toString((char)('a' + a)) + (char)('a'+b) + (char)('a'+c) +
                                            (char)('a'+d) + (char)('a'+e) + (char)('a'+f) + ", ";
                                    writer.write(str);
                                    count++;
                                    if(count%10 == 0) writer.write("\n");
                                }
                            }
                        }
                    }
                }
            }
            writer.close();
        } catch(IOException e){
            System.out.println("Error occurred");
            e.printStackTrace();
        }*/

    }
}

