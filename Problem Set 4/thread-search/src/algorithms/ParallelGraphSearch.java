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
package algorithms;
import java.math.BigInteger;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;

public class ParallelGraphSearch {

    private final HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph;

    public ParallelGraphSearch(HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph) {
        this.graph = graph;
    }

    private static class PathNode {
        String node;
        List<String> path;
        BigInteger weight;

        public PathNode(String node, List<String> path, BigInteger weight) {
            this.node = node;
            this.path = path;
            this.weight = weight;
        }
    }

    public void searchPathParallel(String start, String target) throws InterruptedException {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            System.out.println("One or both nodes not found in the graph.");
            return;
        }

        // Use a deque as a stack (LIFO)
        LinkedBlockingDeque<PathNode> stack = new LinkedBlockingDeque<>();
        Set<String> visited = ConcurrentHashMap.newKeySet();

        AtomicBoolean foundSolution = new AtomicBoolean(false);
        AtomicReference<PathNode> solution = new AtomicReference<>(null);

        List<String> startPath = new ArrayList<>();
        startPath.add(start);
        // Push onto the stack with weight BigInteger.ZERO
        stack.putFirst(new PathNode(start, startPath, BigInteger.ZERO));
        visited.add(start);

        int numThreads = Runtime.getRuntime().availableProcessors();
        ExecutorService executor = Executors.newFixedThreadPool(numThreads);
        List<Future<?>> futures = new ArrayList<>();

        AtomicLong foundTime = new AtomicLong(0);
        long startTime = System.nanoTime();
        Runnable worker = () -> {
            try {
                while (!foundSolution.get()) {
                    // Poll from the head of the deque (LIFO order)
                    PathNode current = stack.pollFirst(100, TimeUnit.MILLISECONDS);
                    if (current == null) {
                        if (stack.isEmpty()) {
                            break;
                        }
                        continue;
                    }

                    if (current.node.equals(target)) {
                        if (foundSolution.compareAndSet(false, true)) {
                            solution.set(current);
                        }
                        foundTime.set(System.nanoTime());
                        break;
                    }

                    List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = graph.get(current.node);
                    if (neighbors != null) {
                        for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                            String neighbor = edge.getKey();
                            BigInteger newWeight = current.weight.add(edge.getValue());

                            if (visited.add(neighbor)) {
                                List<String> newPath = new ArrayList<>(current.path);
                                newPath.add(neighbor);
                                // Push new node onto the stack to continue depth-first
                                stack.putFirst(new PathNode(neighbor, newPath, newWeight));
                            }
                        }
                    }
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        };

        for (int i = 0; i < numThreads; i++) {
            futures.add(executor.submit(worker));
        }

        executor.shutdown();
        try {
            executor.awaitTermination(1, TimeUnit.MINUTES);
        } catch (InterruptedException e) {
            executor.shutdownNow();
            Thread.currentThread().interrupt();
        }

        if (solution.get() != null) {
            PathNode sol = solution.get();
            System.out.println("Path: " + String.join(" -> ", sol.path) + " with weight: " + sol.weight);
            System.out.println("Time: " + (foundTime.get() - startTime) / 1000000 + "ms est.");
        } else {
            System.out.println("No path from " + start + " to " + target);
            System.out.println("Time: " + (System.nanoTime() - startTime) / 1000000 + "ms est.");
        }
    }
}
