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
import java.io.*;
import java.net.*;
import java.util.Scanner;

public class RemoteGraphClient {

    private static final String SERVER_ADDRESS = "localhost";
    private static final int SERVER_PORT = 9876;

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        printInstructions();

        while (true) {
            System.out.print("[>] ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("exit")) {
                System.out.println("Exiting Interactive Graph Client.");
                break;
            }
            
            // Send the command to the server and print its response.
            String response = sendQuery(input);
            System.out.println("Response: " + response);
        }
        scanner.close();
    }

    /**
     * Prints all the valid commands/instructions.
     */
    private static void printInstructions() {
        System.out.println("Interactive Graph Client");
        System.out.println("Enter one of the following commands:");
        System.out.println(" [System Queries]:");
        System.out.println("  help               -> list all valid queries");
        System.out.println("  nodes              -> list all nodes (labels) in the graph");
        System.out.println("  edges              -> list all edges in the graph");
        System.out.println("  parallel           -> toggle parallel mode on/off");
        System.out.println("  clear              -> clears the terminal");
        System.out.println("  exit               -> terminate the program");
        System.out.println("\n [Path Queries]:");
        System.out.println("  node x             ");
        System.out.println("  edge x y           ");
        System.out.println("  path x y           ");
        System.out.println("  shortest-path x y  ");
        System.out.println("  prime-path x y     ");
        System.out.println("  shortest-prime-path x y");
        System.out.println("------------------------------------------------");
    }

    /**
     * Connects to the server, sends a single query, and returns the response.
     * This method waits until a connection can be established.
     */
    public static String sendQuery(String query) {
        String response = "";
        while (true) {
            try (Socket socket = new Socket(SERVER_ADDRESS, SERVER_PORT);
                 PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                 BufferedReader in = new BufferedReader(
                         new InputStreamReader(socket.getInputStream()))) {

                // Connection was successful; send the query.
                out.println(query);
                response = in.readLine();
                break;
            } catch (IOException e) {
                System.out.println("Server not available, waiting...");
                try {
                    Thread.sleep(1000); // Wait 1 second before retrying.
                } catch (InterruptedException ie) {
                    Thread.currentThread().interrupt();
                    return "Interrupted while waiting for server.";
                }
            }
        }
        return response;
    }
}