public class Agent {
    private String name; // e.g., "1-2"
    private String goal; // derived from name; for instance, "1" (the target)
    private String startNode; // starting node
    private String restriction; // a default or provided restriction value

    public Agent(String name, String startNode) {
        this.name = name;
        this.startNode = startNode;
        // Extract the goal from the agent label. Assuming the format "x-y"
        if (name.contains("-")) {
            this.goal = name.split("-")[0];
        } else {
            this.goal = "";
        }
        // Set a default restriction, for example "1000"
        this.restriction = goal;
    }

    public String getName() {
        return name;
    }

    public String getGoal() {
        return goal;
    }

    public String getStartNode() {
        return startNode;
    }

    public String getRestriction() {
        return restriction;
    }

    @Override
    public String toString() {
        return "Agent{name='" + name + "', goal='" + goal + "', startNode='" + startNode + "', restriction='"
                + restriction + "'}";
    }
}
