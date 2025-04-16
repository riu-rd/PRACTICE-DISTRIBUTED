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
import java.math.BigInteger;
import java.util.*;
import graph.*; 

public class RemoteGraphServer {
    private static final int PORT = 9876;
    private static Graph graph;
    private static final String BASE_DIR = "thread-search/graphs";

    public static void main(String[] args) {
        // Initialize the graph.
        String graphFile = "big.txt"; // Use your actual graph file name
        graph = new Graph(BASE_DIR, graphFile);

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Server is listening on port " + PORT);
            while (true) {
                // Accept an incoming client connection.
                Socket clientSocket = serverSocket.accept();
                // Create a new thread to handle each connection.
                new Thread(() -> handleClient(clientSocket)).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    private static void handleClient(Socket clientSocket) {
        try (
            BufferedReader in = new BufferedReader(
                                    new InputStreamReader(clientSocket.getInputStream()));
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true)
        ) {
            // Read the query message.
            String query = in.readLine();
            System.out.println("Received query: " + query);

            // Process the query.
            String response = processQuery(query);

            // Send the response back to the client.
            out.println(response);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                clientSocket.close();
            } catch(IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Processes incoming commands similarly to TerminalUI.
     */
    private static String processQuery(String query) {
        if (query == null || query.trim().isEmpty()) {
            return "Empty query received.";
        }
        String trimmed = query.trim();
        // For commands that should not be case sensitive, we use lower-case.
        String[] parts = trimmed.split("\\s+");
        String cmd = parts[0].toLowerCase();
        // Get the current graph data.
        HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graphMap = graph.getGraph();
        String result = "";

        switch (cmd) {
            case "nodes":
                result = "Nodes: " + graphMap.keySet().toString();
                break;
            case "node":
                if (parts.length < 2) {
                    result = "Usage: node <node>";
                } else {
                    String node = parts[1];
                    result = graphMap.containsKey(node)
                        ? "Node " + node + " exists"
                        : "Node " + node + " not found";
                }
                break;
            case "edges":
                StringBuilder sbEdges = new StringBuilder("Edges:\n");
                for (String node : graphMap.keySet()) {
                    sbEdges.append(node)
                           .append(" -> ")
                           .append(graphMap.get(node))
                           .append("\n");
                }
                result = sbEdges.toString();
                break;
            case "edge":
                if (parts.length < 3) {
                    result = "Usage: edge <node1> <node2>";
                } else {
                    String node1 = parts[1];
                    String node2 = parts[2];
                    if (graphMap.containsKey(node1)) {
                        boolean found = false;
                        for (AbstractMap.SimpleEntry<String, BigInteger> edge : graphMap.get(node1)) {
                            if (edge.getKey().equals(node2)) {
                                result = "Edge from " + node1 + " -> " + node2 +
                                         " with weight: " + edge.getValue() + " exists";
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            result = "Edge from " + node1 + " -> " + node2 + " not found";
                        }
                    } else {
                        result = "Node " + node1 + " not found";
                    }
                }
                break;
            case "path":
                if (parts.length < 3) {
                    result = "Usage: path <node1> <node2>";
                } else {
                    String node1 = parts[1];
                    String node2 = parts[2];
                    if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                        if (graph.getIsParallel()) {
                            result = captureOutput(() -> graph.searchPathParallel(node1, node2));
                        } else {
                            result = captureOutput(() -> graph.searchPath(node1, node2));
                        }
                    } else {
                        result = "Invalid nodes";
                    }
                }
                break;
            case "prime-path":
                if (parts.length < 3) {
                    result = "Usage: prime-path <node1> <node2>";
                } else {
                    String node1 = parts[1];
                    String node2 = parts[2];
                    if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                        if (graph.getIsParallel()) {
                            result = captureOutput(() -> graph.searchPathPrimeParallel(node1, node2));
                        } else {
                            result = captureOutput(() -> graph.searchPathPrime(node1, node2));
                        }
                    } else {
                        result = "Invalid nodes";
                    }
                }
                break;
            case "shortest-path":
                if (parts.length < 3) {
                    result = "Usage: shortest-path <node1> <node2>";
                } else {
                    String node1 = parts[1];
                    String node2 = parts[2];
                    if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                        if (graph.getIsParallel()) {
                            result = captureOutput(() -> graph.searchPathShortestParallel(node1, node2));
                        } else {
                            result = captureOutput(() -> graph.searchPathShortest(node1, node2));
                        }
                    } else {
                        result = "Invalid nodes";
                    }
                }
                break;
            case "shortest-prime-path":
                if (parts.length < 3) {
                    result = "Usage: shortest-prime-path <node1> <node2>";
                } else {
                    String node1 = parts[1];
                    String node2 = parts[2];
                    if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                        if (graph.getIsParallel()) {
                            result = captureOutput(() -> graph.searchPathShortestPrimeParallel(node1, node2));
                        } else {
                            result = captureOutput(() -> graph.searchPathShortestPrime(node1, node2));
                        }
                    } else {
                        result = "Invalid nodes";
                    }
                }
                break;
            case "parallel":
                boolean current = graph.getIsParallel();
                graph.setIsParallel(!current);
                result = "Parallel mode: " + graph.getIsParallel();
                break;
            case "help":
                result = getInstructions();
                break;
            case "load":
                if (parts.length < 2) {
                    result = "Usage: load <filename>";
                } else {
                    String filename = parts[1];
                    graph.parseGraphFile(filename);
                    result = "Loaded file: " + filename;
                }
                break;
            case "clear":
                // The clear command returns a clear-screen ANSI sequence.
                result = "\033[H\033[2J";
                break;
            default:
                result = "Invalid query";
        }
        return result;
    }

    /**
     * Returns a string with all the commands/instructions.
     */
    private static String getInstructions() {
        StringBuilder instructions = new StringBuilder();
        instructions.append("[System Queries]:\n")
                    .append(" help\t\tlist all valid queries\n")
                    .append(" nodes\t\tlist all nodes (labels) in the graph\n")
                    .append(" edges\t\tlist all edges in the graph\n")
                    .append(" parallel\ttoggle parallel mode on/off\n")
                    .append(" clear\t\tclears the terminal\n")
                    .append(" exit\t\tterminate the program\n\n")
                    .append("[Path Queries]:\n")
                    .append(" node x\n")
                    .append(" edge x y\n")
                    .append(" path x y\n")
                    .append(" shortest-path x y\n")
                    .append(" prime-path x y\n")
                    .append(" shortest-prime-path x y\n");
        return instructions.toString();
    }

    /**
     * Captures output printed to System.out during the execution of the provided runnable.
     */
    private static String captureOutput(Runnable runnable) {
        PrintStream originalOut = System.out;
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        PrintStream ps = new PrintStream(baos);
        System.setOut(ps);
        try {
            runnable.run();
        } finally {
            System.out.flush();
            System.setOut(originalOut);
        }
        return baos.toString().trim();
    }
}
