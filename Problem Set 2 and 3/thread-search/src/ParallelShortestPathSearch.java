import java.math.BigInteger;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

public class ParallelShortestPathSearch {

    private final HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph;

    public ParallelShortestPathSearch(HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph) {
        this.graph = graph;
    }

    private static class NodeEntry implements Comparable<NodeEntry> {
        String node;
        BigInteger distance;
        
        public NodeEntry(String node, BigInteger distance) {
            this.node = node;
            this.distance = distance;
        }
        
        @Override
        public int compareTo(NodeEntry other) {
            return this.distance.compareTo(other.distance);
        }
    }

    public void searchPathShortestParallel(String start, String target) {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            System.out.println("One or both nodes not found in the graph.");
            return;
        }

    
        BigInteger INF = new BigInteger("1000000000000000000000000000");

        ConcurrentHashMap<String, BigInteger> distances = new ConcurrentHashMap<>();
        ConcurrentHashMap<String, String> predecessors = new ConcurrentHashMap<>();

        // Initialize distances for all nodes.
        for (String node : graph.keySet()) {
            distances.put(node, INF);
        }
        distances.put(start, BigInteger.ZERO);

        PriorityBlockingQueue<NodeEntry> queue = new PriorityBlockingQueue<>();
        queue.add(new NodeEntry(start, BigInteger.ZERO));

        AtomicBoolean finished = new AtomicBoolean(false);
        int numThreads = Runtime.getRuntime().availableProcessors();
        ExecutorService executor = Executors.newFixedThreadPool(numThreads);
        long startTime = System.nanoTime();
        AtomicLong foundTime = new AtomicLong(0);

        Runnable worker = () -> {
            try {
                while (!finished.get()) {
                    NodeEntry currentEntry = queue.poll(100, TimeUnit.MILLISECONDS);
                    if (currentEntry == null) {
                        if (queue.isEmpty()) break;
                        else continue;
                    }

                    String currentNode = currentEntry.node;
                    BigInteger currentDist = currentEntry.distance;

                    // Skip outdated entries.
                    if (!currentDist.equals(distances.get(currentNode))) {
                        continue;
                    }

                    if (currentNode.equals(target)) {
                        finished.set(true);
                        foundTime.set(System.nanoTime());
                        break;
                    }

                    List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = graph.get(currentNode);
                    if (neighbors != null) {
                        for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                            String neighbor = edge.getKey();
                            BigInteger weight = edge.getValue();
                            BigInteger newDistance = currentDist.add(weight);

                            while (true) {
                                BigInteger currentNeighborDist = distances.get(neighbor);
                                if (newDistance.compareTo(currentNeighborDist) < 0) {
                                    if (distances.replace(neighbor, currentNeighborDist, newDistance)) {
                                        predecessors.put(neighbor, currentNode);
                                        queue.put(new NodeEntry(neighbor, newDistance));
                                        break;
                                    }
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        };

        for (int i = 0; i < numThreads; i++) {
            executor.submit(worker);
        }

        executor.shutdown();
        try {
            executor.awaitTermination(1, TimeUnit.MINUTES);
        } catch (InterruptedException e) {
            executor.shutdownNow();
            Thread.currentThread().interrupt();
        }

        if (!distances.get(target).equals(INF)) {
            List<String> path = new ArrayList<>();
            String current = target;
            while (current != null) {
                path.add(current);
                current = predecessors.get(current);
            }
            Collections.reverse(path);
            System.out.println("Shortest path: " + String.join(" -> ", path) 
                    + " with weight: " + distances.get(target));
            System.out.println("Time: " + (foundTime.get() - startTime) / 1000000 + "ms est.");
        } else {
            System.out.println("No path from " + start + " to " + target);
            System.out.println("Time: " + (System.nanoTime() - startTime) / 1000000 + "ms est.");
        }
    }
}
