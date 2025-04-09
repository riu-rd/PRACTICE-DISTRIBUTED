import java.math.BigInteger;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

public class ParallelPrimeGraphSearch {

    private final HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph;

    public ParallelPrimeGraphSearch(HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph) {
        this.graph = graph;
    }


    public void searchPathPrimeParallel(String start, String target) {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            System.out.println("One or both nodes not found in the graph.");
            return;
        }

        // Build the reverse graph with BigInteger weights.
        Map<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> reverseGraph = new HashMap<>();
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

        class State {
            String node;
            BigInteger totalWeight;
            List<String> path;

            State(String node, BigInteger totalWeight, List<String> path) {
                this.node = node;
                this.totalWeight = totalWeight;
                this.path = path;
            }
        }

        ConcurrentLinkedQueue<State> forwardQueue = new ConcurrentLinkedQueue<>();
        ConcurrentLinkedQueue<State> backwardQueue = new ConcurrentLinkedQueue<>();

        ConcurrentHashMap<String, State> forwardVisited = new ConcurrentHashMap<>();
        ConcurrentHashMap<String, State> backwardVisited = new ConcurrentHashMap<>();

        List<String> startPath = new ArrayList<>();
        startPath.add(start);
        State startState = new State(start, BigInteger.ZERO, startPath);
        forwardQueue.add(startState);
        forwardVisited.put(start, startState);

        List<String> targetPath = new ArrayList<>();
        targetPath.add(target);
        State targetState = new State(target, BigInteger.ZERO, targetPath);
        backwardQueue.add(targetState);
        backwardVisited.put(target, targetState);

        AtomicBoolean foundSolution = new AtomicBoolean(false);
        AtomicReference<State> solution = new AtomicReference<>(null);
        long startTime = System.nanoTime();

        Runnable worker = () -> {
            try {
                while (!foundSolution.get()) {
                    boolean didWork = false;

                    State currentForward = forwardQueue.poll();
                    if (currentForward != null) {
                        didWork = true;

                        if (backwardVisited.containsKey(currentForward.node)) {
                            State backwardState = backwardVisited.get(currentForward.node);
                            BigInteger totalWeight = currentForward.totalWeight.add(backwardState.totalWeight);
                            
                            if (totalWeight.isProbablePrime(10)) {
                                List<String> fullPath = new ArrayList<>(currentForward.path);
                                List<String> backwardPath = new ArrayList<>(backwardState.path);
                                Collections.reverse(backwardPath);
                                if (!backwardPath.isEmpty()) {
                                    backwardPath.remove(0); // avoid duplicate meeting node
                                }
                                fullPath.addAll(backwardPath);
                                State sol = new State(currentForward.node, totalWeight, fullPath);
                                if (foundSolution.compareAndSet(false, true)) {
                                    solution.set(sol);
                                }
                                break;
                            }
                        }

                        List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = graph.get(currentForward.node);
                        if (neighbors != null) {
                            for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                                String neighbor = edge.getKey();
                                if (currentForward.path.contains(neighbor))
                                    continue;
                                BigInteger newWeight = currentForward.totalWeight.add(edge.getValue());
                                List<String> newPath = new ArrayList<>(currentForward.path);
                                newPath.add(neighbor);
                                if (!forwardVisited.containsKey(neighbor)) {
                                    State newState = new State(neighbor, newWeight, newPath);
                                    forwardQueue.add(newState);
                                    forwardVisited.put(neighbor, newState);
                                }
                            }
                        }
                    }

                    State currentBackward = backwardQueue.poll();
                    if (currentBackward != null) {
                        didWork = true;

                        if (forwardVisited.containsKey(currentBackward.node)) {
                            State forwardState = forwardVisited.get(currentBackward.node);
                            BigInteger totalWeight = currentBackward.totalWeight.add(forwardState.totalWeight);
                            if (totalWeight.isProbablePrime(10)) {
                                List<String> fullPath = new ArrayList<>(forwardState.path);
                                List<String> backwardPath = new ArrayList<>(currentBackward.path);
                                Collections.reverse(backwardPath);
                                if (!backwardPath.isEmpty()) {
                                    backwardPath.remove(0);
                                }
                                fullPath.addAll(backwardPath);
                                State sol = new State(currentBackward.node, totalWeight, fullPath);
                                if (foundSolution.compareAndSet(false, true)) {
                                    solution.set(sol);
                                }
                                break;
                            }
                        }

                        List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = reverseGraph.get(currentBackward.node);
                        if (neighbors != null) {
                            for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                                String neighbor = edge.getKey();
                                if (currentBackward.path.contains(neighbor))
                                    continue;
                                BigInteger newWeight = currentBackward.totalWeight.add(edge.getValue());
                                List<String> newPath = new ArrayList<>(currentBackward.path);
                                newPath.add(neighbor);
                                if (!backwardVisited.containsKey(neighbor)) {
                                    State newState = new State(neighbor, newWeight, newPath);
                                    backwardQueue.add(newState);
                                    backwardVisited.put(neighbor, newState);
                                }
                            }
                        }
                    }

                    if (!didWork && forwardQueue.isEmpty() && backwardQueue.isEmpty()) {
                        break;
                    }
                }
            } catch (Exception e) {
                Thread.currentThread().interrupt();
            }
        };

        int numThreads = Runtime.getRuntime().availableProcessors();
        ExecutorService executor = Executors.newFixedThreadPool(numThreads);
        List<Future<?>> futures = new ArrayList<>();
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
        long endTime = System.nanoTime();

        if (solution.get() != null) {
            State sol = solution.get();
            System.out.println("Prime Path: " + String.join(" -> ", sol.path)
                    + " with weight: " + sol.totalWeight);
            System.out.println("Time: " + (endTime - startTime) / 1_000_000 + "ms est.");
        } else {
            System.out.println("No prime path found from " + start + " to " + target);
            System.out.println("Time: " + (endTime - startTime) / 1_000_000 + "ms est.");
        }
    }
}
