import java.util.*;
import java.io.*;
import java.math.BigInteger;
import java.nio.file.Paths;

public class Graph {

    private boolean isParallel = false;

    // Graph: mapping from node name to a list of outgoing edges (destination and
    // weight)
    private HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph = new HashMap<>();

    // Agents: mapping from the node name to the Agent located at that node.
    private HashMap<String, Agent> agents = new HashMap<>();


    private void parseGraphFile(String filePath) {
        // Clear existing graph and agents
        this.graph.clear();
        this.agents.clear();

        // Hardcoding file path to a graph in the "graphs" folder
        filePath = Paths.get("graphs", filePath).toAbsolutePath().toString();
        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            String line;
            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty())
                    continue;

                // Check if line is a vertex (and optionally agent) definition.
                if (line.startsWith("*")) {
                    // Remove "*" and trim
                    line = line.substring(1).trim();
                    String[] parts = line.split("\\s+");
                    if (parts.length < 1)
                        continue;
                    // First token is the node name.
                    String node = parts[0];
                    graph.putIfAbsent(node, new ArrayList<>());

                    // If there's a second token, it represents an agent at this node.
                    if (parts.length > 1) {
                        String agentLabel = parts[1];
                        Agent agent = new Agent(agentLabel, node);
                        agents.put(node, agent);
                    }
                }
                // Process edge definitions.
                else if (line.startsWith("-")) {
                    // Remove "-" and trim
                    line = line.substring(1).trim();
                    String[] parts = line.split("\\s+");
                    if (parts.length != 3) {
                        System.err.println("Invalid edge format: " + line);
                        continue;
                    }
                    String fromNode = parts[0];
                    String toNode = parts[1];
                    BigInteger weight = new BigInteger(parts[2]);

                    // Ensure nodes exist in the graph (they might not be explicitly declared in the
                    // vertex section)
                    graph.putIfAbsent(fromNode, new ArrayList<>());
                    graph.putIfAbsent(toNode, new ArrayList<>());
                    graph.get(fromNode).add(new AbstractMap.SimpleEntry<>(toNode, weight));
                }
            }
        } catch (IOException e) {
            System.err.println("Error reading file: " + e.getMessage());
        }
    }

    public Graph(String filePath) {
        this.graph = new HashMap<>();
        this.parseGraphFile(filePath);
    }

    // Helper method to print a path given the predecessor map and distances.
    private void printPath(HashMap<String, String> predecessors, String start, String target,
            HashMap<String, BigInteger> distances) {
        List<String> path = new ArrayList<>();
        String step = target;
        while (step != null) {
            path.add(step);
            step = predecessors.get(step);
        }
        Collections.reverse(path);
        BigInteger totalWeight = distances.get(target);
        System.out.println("Path: " + String.join(" -> ", path) + " with weight: " + totalWeight);
    }

    // DFS SEARCH PATH REGULAR //

    private long searchPath(String start, String target) {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            long time = System.nanoTime();
            System.out.println("One or both nodes not found in the graph.");
            return time;
        }
        Stack<String> stack = new Stack<>();
        HashMap<String, String> predecessors = new HashMap<>();
        HashMap<String, BigInteger> distances = new HashMap<>();
        HashSet<String> visited = new HashSet<>();
        stack.push(start);
        predecessors.put(start, null);
        distances.put(start, BigInteger.ZERO);

        while (!stack.isEmpty()) {
            String current = stack.pop();
            if (current.equals(target)) {
                long time = System.nanoTime();
                printPath(predecessors, start, target, distances);
                return time;
            }
            if (!visited.contains(current)) {
                visited.add(current);
                for (AbstractMap.SimpleEntry<String, BigInteger> edge : graph.get(current)) {
                    String neighbor = edge.getKey();
                    BigInteger weight = edge.getValue();
                    BigInteger newDistance = distances.get(current).add(weight);
                    if (!visited.contains(neighbor)) {
                        stack.push(neighbor);
                        predecessors.put(neighbor, current);
                        distances.put(neighbor, newDistance);
                    }
                }
            }
        }
        System.out.println("No path found from " + start + " to " + target);
        return System.nanoTime();
    }

    private void searchPathParallel(String start, String target) throws InterruptedException {
        ParallelGraphSearch parallelSearch = new ParallelGraphSearch(this.graph);
        parallelSearch.searchPathParallel(start, target);
    }

    // DFS SEARCH PATH REGULAR //

    // Helper to check if a number is prime.
    private boolean isPrime(int number) {
        if (number <= 1)
            return false;
        for (int i = 2; i <= Math.sqrt(number); i++) {
            if (number % i == 0)
                return false;
        }
        return true;
    }

    // BFS SEARCH PATH PRIME //

    private long searchPathPrime(String start, String target) {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            long time = System.nanoTime();
            System.out.println("One or both nodes not found in the graph.");
            return time;
        }

        // Build reverseGraph using BigInteger for weights
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

        // State now stores BigInteger for totalWeight.
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

        Queue<State> forwardQueue = new LinkedList<>();
        Queue<State> backwardQueue = new LinkedList<>();

        Map<String, State> forwardVisited = new HashMap<>();
        Map<String, State> backwardVisited = new HashMap<>();

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

        while (!forwardQueue.isEmpty() && !backwardQueue.isEmpty()) {
            if (!forwardQueue.isEmpty()) {
                State currentForward = forwardQueue.poll();

                if (backwardVisited.containsKey(currentForward.node)) {
                    State backwardState = backwardVisited.get(currentForward.node);
                    BigInteger totalWeight = currentForward.totalWeight.add(backwardState.totalWeight);
                    if (totalWeight.isProbablePrime(10)) {
                        List<String> fullPath = new ArrayList<>(currentForward.path);
                        List<String> backwardPath = new ArrayList<>(backwardState.path);
                        Collections.reverse(backwardPath);
                        backwardPath.remove(0);
                        fullPath.addAll(backwardPath);

                        long time = System.nanoTime();
                        System.out.println("Prime Path: " + String.join(" -> ", fullPath)
                                + " with weight: " + totalWeight);
                        return time;
                    }
                }

                for (AbstractMap.SimpleEntry<String, BigInteger> edge : graph.get(currentForward.node)) {
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

            if (!backwardQueue.isEmpty()) {
                State currentBackward = backwardQueue.poll();

                if (forwardVisited.containsKey(currentBackward.node)) {
                    State forwardState = forwardVisited.get(currentBackward.node);
                    BigInteger totalWeight = currentBackward.totalWeight.add(forwardState.totalWeight);
                    if (totalWeight.isProbablePrime(10)) {
                        List<String> fullPath = new ArrayList<>(forwardState.path);
                        List<String> backwardPath = new ArrayList<>(currentBackward.path);
                        Collections.reverse(backwardPath);
                        backwardPath.remove(0);
                        fullPath.addAll(backwardPath);

                        long time = System.nanoTime();
                        System.out.println("Prime Path: " + String.join(" -> ", fullPath)
                                + " with weight: " + totalWeight);
                        return time;
                    }
                }

                for (AbstractMap.SimpleEntry<String, BigInteger> edge : reverseGraph.get(currentBackward.node)) {
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
        long time = System.nanoTime();
        System.out.println("No prime path found from " + start + " to " + target);
        return time;
    }

    private void searchPathPrimeParallel(String start, String target) {
        ParallelPrimeGraphSearch parallelPrimeSearch = new ParallelPrimeGraphSearch(this.graph);
        parallelPrimeSearch.searchPathPrimeParallel(start, target);
    }

    // BFS SEARCH PATH PRIME //

    // Shortest Djkstra Path Search //

    private long searchPathShortest(String start, String target) {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            long time = System.nanoTime();
            System.out.println("One or both nodes not found in the graph.");
            return time;
        }

        // PriorityQueue using BigInteger for weight comparisons
        PriorityQueue<AbstractMap.SimpleEntry<String, BigInteger>> queue = new PriorityQueue<>(
                Comparator.comparing(AbstractMap.SimpleEntry::getValue));

        // Map to store distances with BigInteger
        HashMap<String, BigInteger> distances = new HashMap<>();
        HashMap<String, String> predecessors = new HashMap<>();
        Set<String> visited = new HashSet<>();

        // Define an arbitrary large value as "infinity"
        BigInteger INF = new BigInteger("9999999999999999999999999999");

        for (String node : graph.keySet()) {
            distances.put(node, INF);
        }
        distances.put(start, BigInteger.ZERO);
        queue.add(new AbstractMap.SimpleEntry<>(start, BigInteger.ZERO));

        while (!queue.isEmpty()) {
            String current = queue.poll().getKey();
            if (visited.contains(current))
                continue;
            visited.add(current);
            if (current.equals(target)) {
                long time = System.nanoTime();
                printPath(predecessors, start, target, distances);
                return time;
            }
            for (AbstractMap.SimpleEntry<String, BigInteger> edge : graph.get(current)) {
                String neighbor = edge.getKey();
                BigInteger weight = edge.getValue();
                BigInteger newDistance = distances.get(current).add(weight);
                if (!visited.contains(neighbor) && newDistance.compareTo(distances.get(neighbor)) < 0) {
                    distances.put(neighbor, newDistance);
                    predecessors.put(neighbor, current);
                    queue.add(new AbstractMap.SimpleEntry<>(neighbor, newDistance));
                }
            }
        }

        System.out.println("No path found from " + start + " to " + target);
        return System.nanoTime();
    }

    private void searchPathShortestParallel(String start, String target) {
        ParallelShortestPathSearch parallelSearch = new ParallelShortestPathSearch(this.graph);
        parallelSearch.searchPathShortestParallel(start, target);
    }

    // Shortest Djkstra Path Search //

    // Shortest Prime Path BFS Search //

    public long searchPathShortestPrime(String start, String target) {
        if (!graph.containsKey(start) || !graph.containsKey(target)) {
            long time = System.nanoTime();
            System.out.println("One or both nodes not found in the graph.");
            return time;
        }

        List<String> bestPath = null;
        // Initialize bestWeight to a very large value ("infinity")
        BigInteger bestWeight = new BigInteger("99999999999999999999999999999999999999999999999999");

        // Build sortedGraph using BigInteger for weights.
        Map<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> sortedGraph = new HashMap<>();
        for (String node : graph.keySet()) {
            List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = new ArrayList<>(graph.get(node));
            neighbors.sort(Comparator.comparing(AbstractMap.SimpleEntry::getValue));
            sortedGraph.put(node, neighbors);
        }

        // Build sortedReverseGraph similarly.
        Map<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> sortedReverseGraph = new HashMap<>();
        for (String node : graph.keySet()) {
            sortedReverseGraph.put(node, new ArrayList<>());
        }
        for (String node : graph.keySet()) {
            for (AbstractMap.SimpleEntry<String, BigInteger> edge : graph.get(node)) {
                String neighbor = edge.getKey();
                BigInteger weight = edge.getValue();
                sortedReverseGraph.computeIfAbsent(neighbor, k -> new ArrayList<>())
                        .add(new AbstractMap.SimpleEntry<>(node, weight));
            }
        }
        for (String node : sortedReverseGraph.keySet()) {
            List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = sortedReverseGraph.get(node);
            neighbors.sort(Comparator.comparing(AbstractMap.SimpleEntry::getValue));
        }

        // BFSState now holds BigInteger for weight.
        class BFSState {
            String node;
            BigInteger weight;
            List<String> path;
            Set<String> visited;

            BFSState(String node, BigInteger weight, List<String> path, Set<String> visited) {
                this.node = node;
                this.weight = weight;
                this.path = path;
                this.visited = visited;
            }
        }

        Queue<BFSState> forwardQueue = new LinkedList<>();
        Queue<BFSState> backwardQueue = new LinkedList<>();

        Map<String, BigInteger> bestCostForward = new HashMap<>();
        Map<String, BigInteger> bestCostBackward = new HashMap<>();

        Map<String, BFSState> forwardStateMap = new HashMap<>();
        Map<String, BFSState> backwardStateMap = new HashMap<>();

        Set<String> visitedForward = new HashSet<>();
        visitedForward.add(start);
        BFSState startState = new BFSState(start, BigInteger.ZERO, new ArrayList<>(Collections.singletonList(start)),
                visitedForward);
        forwardQueue.add(startState);
        bestCostForward.put(start, BigInteger.ZERO);
        forwardStateMap.put(start, startState);

        Set<String> visitedBackward = new HashSet<>();
        visitedBackward.add(target);
        BFSState targetState = new BFSState(target, BigInteger.ZERO, new ArrayList<>(Collections.singletonList(target)),
                visitedBackward);
        backwardQueue.add(targetState);
        bestCostBackward.put(target, BigInteger.ZERO);
        backwardStateMap.put(target, targetState);

        while (!forwardQueue.isEmpty() || !backwardQueue.isEmpty()) {
            if (!forwardQueue.isEmpty()) {
                int levelSize = forwardQueue.size();
                for (int i = 0; i < levelSize; i++) {
                    BFSState current = forwardQueue.poll();
                    if (current.weight.compareTo(bestWeight) >= 0)
                        continue;

                    if (backwardStateMap.containsKey(current.node)) {
                        BFSState backwardState = backwardStateMap.get(current.node);
                        BigInteger candidateWeight = current.weight.add(backwardState.weight);
                        // Check if candidate weight is less than bestWeight and is prime.
                        if (candidateWeight.compareTo(bestWeight) < 0 && candidateWeight.isProbablePrime(10)) {
                            bestWeight = candidateWeight;
                            List<String> fullPath = new ArrayList<>(current.path);
                            List<String> backwardPath = new ArrayList<>(backwardState.path);
                            Collections.reverse(backwardPath);
                            if (!backwardPath.isEmpty()) {
                                backwardPath.remove(0); // Avoid duplicating the meeting node.
                            }
                            fullPath.addAll(backwardPath);
                            bestPath = fullPath;
                        }
                    }

                    List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = sortedGraph.get(current.node);
                    if (neighbors != null) {
                        for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                            String neighbor = edge.getKey();
                            if (current.visited.contains(neighbor))
                                continue;
                            BigInteger newWeight = current.weight.add(edge.getValue());
                            if (newWeight.compareTo(bestWeight) >= 0)
                                continue;
                            if (bestCostForward.containsKey(neighbor)
                                    && newWeight.compareTo(bestCostForward.get(neighbor)) >= 0)
                                continue;
                            bestCostForward.put(neighbor, newWeight);
                            List<String> newPath = new ArrayList<>(current.path);
                            newPath.add(neighbor);
                            Set<String> newVisited = new HashSet<>(current.visited);
                            newVisited.add(neighbor);
                            BFSState newState = new BFSState(neighbor, newWeight, newPath, newVisited);
                            forwardQueue.add(newState);
                            forwardStateMap.put(neighbor, newState);
                        }
                    }
                }
            }

            if (!backwardQueue.isEmpty()) {
                int levelSize = backwardQueue.size();
                for (int i = 0; i < levelSize; i++) {
                    BFSState current = backwardQueue.poll();
                    if (current.weight.compareTo(bestWeight) >= 0)
                        continue;

                    if (forwardStateMap.containsKey(current.node)) {
                        BFSState forwardState = forwardStateMap.get(current.node);
                        BigInteger candidateWeight = current.weight.add(forwardState.weight);
                        if (candidateWeight.compareTo(bestWeight) < 0 && candidateWeight.isProbablePrime(10)) {
                            bestWeight = candidateWeight;
                            List<String> fullPath = new ArrayList<>(forwardState.path);
                            List<String> backwardPath = new ArrayList<>(current.path);
                            Collections.reverse(backwardPath);
                            if (!backwardPath.isEmpty()) {
                                backwardPath.remove(0);
                            }
                            fullPath.addAll(backwardPath);
                            bestPath = fullPath;
                        }
                    }

                    List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = sortedReverseGraph.get(current.node);
                    if (neighbors != null) {
                        for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                            String neighbor = edge.getKey();
                            if (current.visited.contains(neighbor))
                                continue;
                            BigInteger newWeight = current.weight.add(edge.getValue());
                            if (newWeight.compareTo(bestWeight) >= 0)
                                continue;
                            if (bestCostBackward.containsKey(neighbor)
                                    && newWeight.compareTo(bestCostBackward.get(neighbor)) >= 0)
                                continue;
                            bestCostBackward.put(neighbor, newWeight);
                            List<String> newPath = new ArrayList<>(current.path);
                            newPath.add(neighbor);
                            Set<String> newVisited = new HashSet<>(current.visited);
                            newVisited.add(neighbor);
                            BFSState newState = new BFSState(neighbor, newWeight, newPath, newVisited);
                            backwardQueue.add(newState);
                            backwardStateMap.put(neighbor, newState);
                        }
                    }
                }
            }
        }

        long time = System.nanoTime();
        if (bestPath == null) {
            System.out.println("No prime path found from " + start + " to " + target);
        } else {
            System.out.println("Shortest Prime Path: " +
                    String.join(" -> ", bestPath) + " with weight: " + bestWeight);
        }
        return time;
    }

    private void searchPathShortestPrimeParallel(String start, String target) {
        ParallelShortestPrimePathSearch parallelShortestPrimeSearch = new ParallelShortestPrimePathSearch(this.graph);
        parallelShortestPrimeSearch.searchPathShortestPrimeParallel(start, target);
    }

    // Shortest Prime Path DFS Search //

    public void queryGraph(String query) throws InterruptedException {
        query = query.trim().toLowerCase();
        String[] parts = query.split(" ");
        long startTime = System.nanoTime();
        long endTime = 0;

        if (parts[0].equals("nodes")) {
            System.out.println("Nodes: " + graph.keySet());
        } else if (parts[0].equals("node") && parts.length == 2) {
            String node = parts[1];
            if (graph.containsKey(node)) {
                System.out.println("Node " + node + " Exists");
            } else {
                System.out.println("Node " + node + " not found");
            }
        } else if (parts[0].equals("edges")) {
            for (String node : graph.keySet()) {
                System.out.println(node + " -> " + graph.get(node));
            }
        } else if (parts[0].equals("edge") && parts.length == 3) {
            String node1 = parts[1];
            String node2 = parts[2];
            if (graph.containsKey(node1)) {
                boolean found = false;
                // Adapted to BigInteger: edges now hold BigInteger weights.
                for (AbstractMap.SimpleEntry<String, BigInteger> edge : graph.get(node1)) {
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
        } else if (parts[0].equals("agents") && parts.length == 1) {
            System.out.println("Agents:");
            for (Map.Entry<String, Agent> entry : agents.entrySet()) {
                System.out.println(entry.getValue());
            }
        } else if (parts[0].equals("path") && parts.length == 3) {
            String node1 = parts[1];
            String node2 = parts[2];
            if (graph.containsKey(node1) && graph.containsKey(node2)) {
                if (isParallel) {
                    searchPathParallel(node1, node2);
                } else {
                    endTime = searchPath(node1, node2);
                }
            } else {
                System.out.println("Invalid nodes");
            }
        } else if (parts[0].equals("prime-path") && parts.length == 3) {
            String node1 = parts[1];
            String node2 = parts[2];
            if (graph.containsKey(node1) && graph.containsKey(node2)) {
                if (isParallel) {
                    searchPathPrimeParallel(node1, node2);
                } else {
                    endTime = searchPathPrime(node1, node2);
                }
            } else {
                System.out.println("Invalid nodes");
            }
        } else if (parts[0].equals("shortest-path") && parts.length == 3) {
            String node1 = parts[1];
            String node2 = parts[2];
            if (graph.containsKey(node1) && graph.containsKey(node2)) {
                if (isParallel) {
                    searchPathShortestParallel(node1, node2);
                } else {
                    endTime = searchPathShortest(node1, node2);
                }
            } else {
                System.out.println("Invalid nodes");
            }
        } else if (parts[0].equals("shortest-prime-path") && parts.length == 3) {
            String node1 = parts[1];
            String node2 = parts[2];
            if (graph.containsKey(node1) && graph.containsKey(node2)) {
                if (isParallel) {
                    searchPathShortestPrimeParallel(node1, node2);
                } else {
                    endTime = searchPathShortestPrime(node1, node2);
                }
            } else {
                System.out.println("Invalid nodes");
            }
        } else if (parts[0].equals("clear")) {
            System.out.print("\033[H\033[2J");
            System.out.flush();
            endTime = System.nanoTime();
            if (isParallel) {
                System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
            }
        } else if (parts[0].equals("parallel")) {
            isParallel = !isParallel;
            System.out.println("Parallel mode: " + isParallel);
            endTime = System.nanoTime();
            if (isParallel) {
                System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
            }
        } else if (parts[0].equals("load") && parts.length == 2) {
            parseGraphFile(parts[1]);
            endTime = System.nanoTime();
            if (isParallel) {
                System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
            }
        } else if (parts[0].equals("exit")) {
            System.exit(0);
        } else if (parts[0].equals("simulate-agents") && parts.length == 1) {
            AgentSimulation simulation = new AgentSimulation(this.graph, this.agents);
            simulation.beginSimulation();
        } else {
            System.out.println("Invalid query");
            endTime = System.nanoTime();
            if (isParallel) {
                System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
            }
        } 

        if (!isParallel) {
            if (endTime == 0) {
                endTime = System.nanoTime();
            }
            System.out.println("Time: " + (endTime - startTime) / 1000000 + "ms est.");
        }
    }
}
