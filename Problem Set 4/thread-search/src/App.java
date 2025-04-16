/*
    STDISCM - S13
    Problem Set 4: Graph Query - 'Prime' and Shortest Paths Between Nodes (RPC Version)

    This is a group project submission for Problem Set 4.
    All members contributed to the implementation and testing of this solution.

    Submitted on: April 11, 2025
    Group Members:
        1. Shuan Noel Co
        2. John Marc Gregorio
        3. Sebastien Dela Cruz
        4. Darius Ardales
*/
import java.util.Scanner;
import java.io.File;

import graph.*;

public class App {
    
    static final String BASE_DIR = "thread-search/graphs";

    public static void main(String[] args) throws Exception {
        
        // Main Query Loop
        try( Scanner scanner = new Scanner(System.in) ) {

            File file;
            String fileName = "";
            do {
                System.out.print("\nEnter graph file name: ");
                fileName = scanner.nextLine().trim();
                file = new File(BASE_DIR + '/' + fileName );

                if( !file.exists() || !file.isFile() ) {
                    System.out.println("File '" + fileName + "' does not exist.");
                }
            } while( !file.exists() || !file.isFile() );

            Graph graph = new Graph( BASE_DIR, fileName );
            TerminalUI ui = new TerminalUI(graph);
            ui.start();

        } catch( Exception e ) {
            System.err.println("Error reading file: " + e.getMessage());
        }
    }
}