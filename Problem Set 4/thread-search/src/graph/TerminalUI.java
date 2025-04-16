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
package graph;
import java.util.*;

import algorithms.*;

import java.io.*;
import java.math.BigInteger;
import java.nio.file.Paths;

public class TerminalUI {

    // Main Attributes
    private Graph graph;

    // ANSI Escape Codes
    private static final String RESET = "\033[0m";
    private static final String BOLD = "\033[1m";
    private static final String WHITE = "\033[37m";
    private static final String MAGENTA = "\033[35m";
    private static final String GREEN = "\033[32m";
    private static final String BLUE = "\033[34m";
    private static final String GRAY = "\033[90m";
    private static final String RED = "\033[31m";

    public TerminalUI( Graph graph ) {
        this.graph = graph;
    }

    // Start the interactive terminal.
    public void start() throws InterruptedException {
        Scanner scanner = new Scanner(System.in);
        printInstructions();

        while (true) {
            System.out.print(GRAY + "\n[>] " + RESET);
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("exit")) {
                break;
            }
            processQuery(input);
        }

        // Shutdown the executor service.
        scanner.close();
    }

    public void printInstructions() {
        System.out.print(
            BOLD + MAGENTA + "[System Queries]:" + RESET + "\n" +
            MAGENTA + "help" + RESET + " " + GRAY + "\t\t\t\tlist all valid queries" + "\n" +
            MAGENTA + "nodes" + RESET + " " + GRAY + "\t\t\t\tlist all nodes (labels) in the graph" + "\n" +
            MAGENTA + "edges" + RESET + " " + GRAY + "\t\t\t\tlist all edges in the graph" + "\n" +
            MAGENTA + "parallel" + RESET + " " + GRAY + "\t\t\tturns parallel mode on/off" + "\n" +
            MAGENTA + "clear" + RESET + " " + GRAY + "\t\t\t\tclears the terminal" + "\n" +
            MAGENTA + "exit" + RESET + " " + GRAY + "\t\t\t\tterminate the program" + RESET + "\n\n" +

            BOLD + GREEN + "[Path Queries]:" + RESET + "\n" +
            GREEN + "node x" + RESET + " " + GRAY + "\n" +
            GREEN + "edge x y" + RESET + " " + GRAY + "\n" +
            GREEN + "path x y" + RESET + " " + GRAY + "\n" +
            GREEN + "shortest-path x y" + RESET + " " + GRAY + "\n" +
            GREEN + "prime-path x y" + RESET + " " + GRAY + "\n" +
            GREEN + "shortest-prime-path x y" + RESET + " " + GRAY + 
        "\n");
    }

    public void processQuery( String query ) throws InterruptedException {
        query = query.trim().toLowerCase();
        String[] parts = query.split(" ");

        HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graphMap = graph.getGraph();

        long startTime = System.nanoTime(); 
        long endTime = 0;

        switch( parts[0].toLowerCase() ) {
            case "nodes": {
                System.out.println("Nodes: " + graphMap.keySet());
            } break;

            case "node": {
                String node = parts[1];
                if (graphMap.containsKey(node)) {
                    System.out.println("Node " + node + " Exists");
                } else {
                    System.out.println("Node " + node + " not found");
                }
            } break;

            case "edges": {
                System.out.println("Edges:");
                for( String node : graphMap.keySet() ) {
                    System.out.println(node + " -> " + graphMap.get(node));
                }
            } break;

            case "edge": {
                String node1 = parts[1];
                String node2 = parts[2];
                if (graphMap.containsKey(node1)) {
                    boolean found = false;
                    for (AbstractMap.SimpleEntry<String, BigInteger> edge : graphMap.get(node1)) {
                        if (edge.getKey().equals(node2)) {
                            System.out.println("Edge from " + node1 + " -> " + node2
                                            + " with weight: " + edge.getValue() + " exists");
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        System.out.println("Edge from " + node1 + " -> " + node2 + " not found");
                    }
                } else {
                    System.out.println("Node " + node1 + " not found");
                }
            } break;

            case "path": {
                String node1 = parts[1];
                String node2 = parts[2];
                
                if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                    if ( graph.getIsParallel() ) {
                        graph.searchPathParallel(node1, node2);
                    } else {
                        endTime = graph.searchPath(node1, node2);
                    }
                } else {
                    System.out.println("Invalid nodes");
                }
            } break;

            case "prime-path": {
                String node1 = parts[1];
                String node2 = parts[2];
                if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                    if ( graph.getIsParallel() ) {
                        graph.searchPathPrimeParallel(node1, node2);
                    } else {
                        endTime = graph.searchPathPrime(node1, node2);
                    }
                } else {
                    System.out.println("Invalid nodes");
                }
            } break;

            case "shortest-path": {
                String node1 = parts[1];
                String node2 = parts[2];
                if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                    if ( graph.getIsParallel() ) {
                        graph.searchPathShortestParallel(node1, node2);
                    } else {
                        endTime = graph.searchPathShortest(node1, node2);
                    }
                } else {
                    System.out.println("Invalid nodes");
                }
            } break;

            case "shortest-prime-path": {
                String node1 = parts[1];
                String node2 = parts[2];
                if (graphMap.containsKey(node1) && graphMap.containsKey(node2)) {
                    if ( graph.getIsParallel() ) {
                        graph.searchPathShortestPrimeParallel(node1, node2);
                    } else {
                        endTime = graph.searchPathShortestPrime(node1, node2);
                    }
                } else {
                    System.out.println("Invalid nodes");
                }

            } break;

            case "parallel": {
                Boolean isParallelTemp = graph.getIsParallel();
                graph.setIsParallel( !isParallelTemp );

                System.out.println("Parallel mode: " + graph.getIsParallel());
                endTime = System.nanoTime();
                if( graph.getIsParallel() ) {
                    System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
                }


            } break;

            case "help": {
                printInstructions();
            } break;

            case "load": {
                graph.parseGraphFile(parts[1]);
                endTime = System.nanoTime();
                if (graph.getIsParallel()) {
                    System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
                }

            } break;

            case "clear": {
                System.out.print("\033[H\033[2J");
                System.out.flush();
                endTime = System.nanoTime();
                if( graph.getIsParallel() ) {
                    System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
                }
            } break;

            default: {
                System.out.println("Invalid query");
                endTime = System.nanoTime();
                if( graph.getIsParallel() ) {
                    System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
                }
            }
        }

        if( !graph.getIsParallel() ) {
            if (endTime == 0) {
                endTime = System.nanoTime();
            }
            System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
        }
    }
}