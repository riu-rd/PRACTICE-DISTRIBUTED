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
import java.util.concurrent.atomic.*;

public class ParallelShortestPrimePathSearch {

    private final HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph;

    public ParallelShortestPrimePathSearch(
            HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph) {
        this.graph = graph;
    }

    private boolean isPrime(BigInteger number) {
        return number.isProbablePrime(10);
    }

    private static class DFSState {
        String currentNode;
        List<String> path;
        BigInteger currentWeight;
        Set<String> visited;

        DFSState(String currentNode, List<String> path, BigInteger currentWeight, Set<String> visited) {
            this.currentNode = currentNode;
            this.path = path;
            this.currentWeight = currentWeight;
            this.visited = visited;
        }
    }

    public void searchPathShortestPrimeParallel(String start, String target) {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            System.out.println("One or both nodes not found in the graph.");
            return;
        }

     
        HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> reverseGraph = new HashMap<>();
        for (String node : graph.keySet()) {
            reverseGraph.put(node, new ArrayList<>());
        }
        for (String node : graph.keySet()) {
            for (AbstractMap.SimpleEntry<String, BigInteger> edge : graph.get(node)) {
                String neighbor = edge.getKey();
                BigInteger weight = edge.getValue();
                reverseGraph.computeIfAbsent(neighbor, k -> new ArrayList<>())
                        .add(new AbstractMap.SimpleEntry<>(node, weight));
            }
        }

      
        AtomicReference<BigInteger> bestWeight = new AtomicReference<>(new BigInteger("99999999999999999999999999999999999999999999999999"));
        AtomicReference<List<String>> bestPath = new AtomicReference<>(null);

        AtomicInteger tasksCounter = new AtomicInteger(0);

        ConcurrentHashMap<String, DFSState> forwardGlobal = new ConcurrentHashMap<>();
        ConcurrentHashMap<String, DFSState> backwardGlobal = new ConcurrentHashMap<>();

        int numThreads = Runtime.getRuntime().availableProcessors();
        ExecutorService executor = Executors.newFixedThreadPool(numThreads);

        long startTime = System.nanoTime();
        AtomicLong foundTime = new AtomicLong(0);

        Set<String> initialVisitedForward = new HashSet<>();
        initialVisitedForward.add(start);
        DFSState initialForward = new DFSState(start, new ArrayList<>(Arrays.asList(start)), BigInteger.ZERO, initialVisitedForward);
        tasksCounter.incrementAndGet();
        forwardGlobal.put(start, initialForward);
        executor.submit(() -> dfsForward(initialForward, target, bestWeight, bestPath, tasksCounter, executor,
                foundTime, forwardGlobal, backwardGlobal, reverseGraph));

        Set<String> initialVisitedBackward = new HashSet<>();
        initialVisitedBackward.add(target);
        DFSState initialBackward = new DFSState(target, new ArrayList<>(Arrays.asList(target)), BigInteger.ZERO, initialVisitedBackward);
        tasksCounter.incrementAndGet();
        backwardGlobal.put(target, initialBackward);
        executor.submit(() -> dfsBackward(initialBackward, start, bestWeight, bestPath, tasksCounter, executor,
                foundTime, forwardGlobal, backwardGlobal, reverseGraph));

        while (tasksCounter.get() > 0) {
            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }

        executor.shutdownNow();
        long endTime = System.nanoTime();
        if (bestPath.get() != null) {
            System.out.println("Shortest prime path: " + String.join(" -> ", bestPath.get())
                    + " with weight: " + bestWeight.get());
            System.out.println("Time: " + (foundTime.get() - startTime) / 1_000_000 + "ms est.");
        } else {
            System.out.println("No prime path from " + start + " to " + target);
            System.out.println("Time: " + (endTime - startTime) / 1_000_000 + "ms est.");
        }
    }

    private void dfsForward(DFSState state, String target,
            AtomicReference<BigInteger> bestWeight, AtomicReference<List<String>> bestPath,
            AtomicInteger tasksCounter, ExecutorService executor, AtomicLong foundTime,
            ConcurrentHashMap<String, DFSState> forwardGlobal,
            ConcurrentHashMap<String, DFSState> backwardGlobal,
            HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> reverseGraph) {
        try {
            if (state.currentNode.equals(target)) {
                if (isPrime(state.currentWeight) && state.currentWeight.compareTo(bestWeight.get()) < 0) {
                    bestWeight.set(state.currentWeight);
                    bestPath.set(new ArrayList<>(state.path));
                    foundTime.set(System.nanoTime());
                }
            }

            if (backwardGlobal.containsKey(state.currentNode)) {
                DFSState backwardState = backwardGlobal.get(state.currentNode);
                BigInteger combinedWeight = state.currentWeight.add(backwardState.currentWeight);
                if (combinedWeight.compareTo(bestWeight.get()) < 0 && isPrime(combinedWeight)) {
                    List<String> combinedPath = new ArrayList<>(state.path);
                    List<String> backwardPathReversed = new ArrayList<>(backwardState.path);
                    Collections.reverse(backwardPathReversed);
                    if (!backwardPathReversed.isEmpty()) {
                        backwardPathReversed.remove(0);
                    }
                    combinedPath.addAll(backwardPathReversed);
                    if (combinedWeight.compareTo(bestWeight.get()) < 0) {
                        bestWeight.set(combinedWeight);
                        bestPath.set(combinedPath);
                        foundTime.set(System.nanoTime());
                    }
                }
            }

            List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = graph.get(state.currentNode);
            if (neighbors != null) {
                for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                    String neighbor = edge.getKey();
                    if (state.visited.contains(neighbor))
                        continue;
                    BigInteger newWeight = state.currentWeight.add(edge.getValue());
                    if (newWeight.compareTo(bestWeight.get()) >= 0)
                        continue;
                    List<String> newPath = new ArrayList<>(state.path);
                    newPath.add(neighbor);
                    Set<String> newVisited = new HashSet<>(state.visited);
                    newVisited.add(neighbor);
                    DFSState newState = new DFSState(neighbor, newPath, newWeight, newVisited);

                    boolean update = false;
                    DFSState existing = forwardGlobal.get(neighbor);
                    if (existing == null || newWeight.compareTo(existing.currentWeight) < 0) {
                        forwardGlobal.put(neighbor, newState);
                        update = true;
                    }
                    if (update) {
                        tasksCounter.incrementAndGet();
                        executor.submit(() -> dfsForward(newState, target, bestWeight, bestPath, tasksCounter, executor,
                                foundTime, forwardGlobal, backwardGlobal, reverseGraph));
                    }
                }
            }
        } finally {
            tasksCounter.decrementAndGet();
        }
    }

    private void dfsBackward(DFSState state, String start,
            AtomicReference<BigInteger> bestWeight, AtomicReference<List<String>> bestPath,
            AtomicInteger tasksCounter, ExecutorService executor, AtomicLong foundTime,
            ConcurrentHashMap<String, DFSState> forwardGlobal,
            ConcurrentHashMap<String, DFSState> backwardGlobal,
            HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> reverseGraph) {
        try {
            if (state.currentNode.equals(start)) {
                if (isPrime(state.currentWeight) && state.currentWeight.compareTo(bestWeight.get()) < 0) {
                    bestWeight.set(state.currentWeight);
                    List<String> reversedPath = new ArrayList<>(state.path);
                    Collections.reverse(reversedPath);
                    bestPath.set(reversedPath);
                    foundTime.set(System.nanoTime());
                }
            }

            if (forwardGlobal.containsKey(state.currentNode)) {
                DFSState forwardState = forwardGlobal.get(state.currentNode);
                BigInteger combinedWeight = state.currentWeight.add(forwardState.currentWeight);
                if (combinedWeight.compareTo(bestWeight.get()) < 0 && isPrime(combinedWeight)) {
                    List<String> combinedPath = new ArrayList<>(forwardState.path);
                    List<String> reversedPath = new ArrayList<>(state.path);
                    Collections.reverse(reversedPath);
                    if (!reversedPath.isEmpty()) {
                        reversedPath.remove(0);
                    }
                    combinedPath.addAll(reversedPath);
                    if (combinedWeight.compareTo(bestWeight.get()) < 0) {
                        bestWeight.set(combinedWeight);
                        bestPath.set(combinedPath);
                        foundTime.set(System.nanoTime());
                    }
                }
            }

            List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = reverseGraph.get(state.currentNode);
            if (neighbors != null) {
                for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                    String neighbor = edge.getKey();
                    if (state.visited.contains(neighbor))
                        continue;
                    BigInteger newWeight = state.currentWeight.add(edge.getValue());
                    if (newWeight.compareTo(bestWeight.get()) >= 0)
                        continue;
                    List<String> newPath = new ArrayList<>(state.path);
                    newPath.add(neighbor);
                    Set<String> newVisited = new HashSet<>(state.visited);
                    newVisited.add(neighbor);
                    DFSState newState = new DFSState(neighbor, newPath, newWeight, newVisited);

                    boolean update = false;
                    DFSState existing = backwardGlobal.get(neighbor);
                    if (existing == null || newWeight.compareTo(existing.currentWeight) < 0) {
                        backwardGlobal.put(neighbor, newState);
                        update = true;
                    }
                    if (update) {
                        tasksCounter.incrementAndGet();
                        executor.submit(() -> dfsBackward(newState, start, bestWeight, bestPath, tasksCounter, executor,
                                foundTime, forwardGlobal, backwardGlobal, reverseGraph));
                    }
                }
            }
        } finally {
            tasksCounter.decrementAndGet();
        }
    }
}
