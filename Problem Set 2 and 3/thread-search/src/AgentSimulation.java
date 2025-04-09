import java.math.BigInteger;
import java.util.AbstractMap;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class AgentSimulation {


    private final HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph;

    private final HashMap<String, Agent> occupiedNodes = new HashMap<>();

    private final HashMap<String, Agent> agents = new HashMap<>();

    private final HashMap<String, AgentThread> agentThreads = new HashMap<>();

    private boolean deadlockResolutionEnabled = true;
    private volatile boolean simulationComplete = false;

    private int maxParkingSpacesAllowed = 2; 

    public void setDeadlockResolutionEnabled(boolean enabled) {
        this.deadlockResolutionEnabled = enabled;
    }

    public void setMaxParkingSpacesAllowed(int maxParkingSpacesAllowed) {
        this.maxParkingSpacesAllowed = maxParkingSpacesAllowed;
    }

    private String findAgentLocation(Agent agent) {
        for (String key : occupiedNodes.keySet()) {
            if (occupiedNodes.get(key) == agent) {
                return key;
            }
        }
        return "not active";
    }

    private void logAgentStatus() {
        for (Agent agent : agents.values()) {
            String location = findAgentLocation(agent);
            System.out.println("[" + agent.getName() + "]: current state is " + location);
        }
    }

    private class AgentThread extends Thread {
        private final Agent agent;
        private final String start;
        private final String target;

        private final BigInteger maxAllowed;

        private final long DEADLOCK_TIMEOUT = 3000;

        private boolean parked = false;
        private String parkedFrom = null;

        public AgentThread(Agent agent) {
            this.agent = agent;
            this.start = agent.getStartNode();
            this.target = agent.getGoal();
            this.maxAllowed = new BigInteger(agent.getRestriction());
        }

        public void setParked(boolean parked, String parkedFrom) {
            this.parked = parked;
            this.parkedFrom = parkedFrom;
        }


        public boolean isParked() {
            return parked;
        }

        public String getParkedFrom() {
            return parkedFrom;
        }

        @Override
        public void run() {

            synchronized (occupiedNodes) {
                occupiedNodes.put(start, agent);
                occupiedNodes.notifyAll();
            }

            System.out.println("[" + agent.getName() + "]: started at node " + start);

            dfs(start, new HashSet<>());

            while (parked) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    return;
                }
                synchronized (occupiedNodes) {
                    if (occupiedNodes.get(parkedFrom) == null) {

                        occupiedNodes.remove("Parking:" + agent.getName());
                        occupiedNodes.put(parkedFrom, agent);
                        occupiedNodes.notifyAll();
                        parked = false;
                        System.out
                                .println("[" + agent.getName() + "]: leaving parking, resuming at node " + parkedFrom);
                        dfs(parkedFrom, new HashSet<>());
                    }
                }
            }
        }

        private boolean dfs(String current, Set<String> visited) {
            if (parked) {
                return false;
            }
            if (current.equals(target)) {
                synchronized (occupiedNodes) {
                    occupiedNodes.put(current, null);
                    occupiedNodes.notifyAll();
                }
                System.out.println("[" + agent.getName() + "]: reached target " + target);
                return true;
            }
            visited.add(current);
            List<AbstractMap.SimpleEntry<String, BigInteger>> neighbors = graph.get(current);
            if (neighbors != null) {
                for (AbstractMap.SimpleEntry<String, BigInteger> edge : neighbors) {
                    String next = edge.getKey();
                    if (edge.getValue().compareTo(maxAllowed) <= 0 && !visited.contains(next)) {

                        parkedFrom = current;
                        long waitStart = System.currentTimeMillis();
                        boolean moved = false;
                        while (!moved) {
                            synchronized (occupiedNodes) {
                                if (parked) {
                                    return false;
                                }
                                if (occupiedNodes.get(next) == null) {
                                    occupiedNodes.put(next, agent);
                                    occupiedNodes.put(current, null);
                                    occupiedNodes.notifyAll();
                                    moved = true;
                                    System.out.println(
                                            "[" + agent.getName() + "]: moved from " + current + " to " + next);
                                    if (dfs(next, visited)) {
                                        return true;
                                    } else {
                                        if (occupiedNodes.get(current) == null) {
                                            occupiedNodes.put(current, agent);
                                            occupiedNodes.put(next, null);
                                            occupiedNodes.notifyAll();
                                            System.out.println("[" + agent.getName() + "]: backtracked from " + next
                                                    + " to " + current);
                                        } else {
                                            try {
                                                occupiedNodes.wait(500);
                                            } catch (InterruptedException e) {
                                                Thread.currentThread().interrupt();
                                                return false;
                                            }
                                        }
                                    }
                                } else {
                                    try {
                                        occupiedNodes.wait(500);
                                    } catch (InterruptedException e) {
                                        Thread.currentThread().interrupt();
                                        return false;
                                    }

                                    if (System.currentTimeMillis() - waitStart > DEADLOCK_TIMEOUT) {
                                        Agent blockingAgent = occupiedNodes.get(next);
                                        if (blockingAgent != null && deadlockResolutionEnabled) {
                                            AgentThread blockingThread = agentThreads.get(blockingAgent.getName());
                                            if (blockingThread != null && !blockingThread.isParked()) {
                                                System.out.println(
                                                        "[deadlock found]: " + agent.getName() + " waiting for node "
                                                                + next + " occupied by " + blockingAgent.getName());
                                                resolveDeadlock(current, next, agent, blockingAgent);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            visited.remove(current);
            return false;
        }

        private void resolveDeadlock(String waitingNode, String targetNode, Agent waitingAgent, Agent blockingAgent) {
            synchronized (occupiedNodes) {
                // Count current parking spaces.
                int currentParkingCount = 0;
                for (String key : occupiedNodes.keySet()) {
                    if (key.startsWith("Parking:")) {
                        currentParkingCount++;
                    }
                }
                if (currentParkingCount < maxParkingSpacesAllowed) {
                    AgentThread blockingThread = agentThreads.get(blockingAgent.getName());
                    if (blockingThread != null) {
                        String previous = (blockingThread.getParkedFrom() != null) ? blockingThread.getParkedFrom()
                                : targetNode;
                        blockingThread.setParked(true, previous);
                        occupiedNodes.put("Parking:" + blockingAgent.getName(), blockingAgent);
                        occupiedNodes.put(targetNode, null);
                        occupiedNodes.notifyAll();
                        System.out.println("[parking space made]: " + blockingAgent.getName()
                                + " parked from " + previous);
                    }
                } else {
                    System.out.println("[parking space denied]: Maximum parking spaces reached. Cannot park "
                            + blockingAgent.getName());
                }
            }
        }
    }


    public AgentSimulation(HashMap<String, List<AbstractMap.SimpleEntry<String, BigInteger>>> graph,
            HashMap<String, Agent> agents) {
        this.graph = graph;
        this.agents.putAll(agents);
        // Initialize all graph nodes as free.
        for (String node : graph.keySet()) {
            occupiedNodes.put(node, null);
        }
    }

    public void beginSimulation() {
        // Initial simulation log.
        System.out.println("[Initial Simulation Log Information:]");
        System.out.println("Nodes:");
        for (String node : graph.keySet()) {
            System.out.println(" - " + node);
        }
        System.out.println("Edges:");
        for (String node : graph.keySet()) {
            List<AbstractMap.SimpleEntry<String, BigInteger>> edges = graph.get(node);
            if (edges != null) {
                for (AbstractMap.SimpleEntry<String, BigInteger> edge : edges) {
                    System.out.println(" - " + node + " -> " + edge.getKey() + " : " + edge.getValue());
                }
            }
        }
        System.out.println("Agents:");
        for (Agent agent : agents.values()) {
            System.out.println(" - " + agent);
        }

        Thread statusLogger = new Thread(() -> {
            while (!simulationComplete) {
                synchronized (occupiedNodes) {
                    logAgentStatus();
                }
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        });
        statusLogger.start();

        for (Agent agent : agents.values()) {
            AgentThread thread = new AgentThread(agent);
            agentThreads.put(agent.getName(), thread);
            thread.start();
        }

        for (AgentThread thread : agentThreads.values()) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

        simulationComplete = true;
        try {
            statusLogger.join();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
