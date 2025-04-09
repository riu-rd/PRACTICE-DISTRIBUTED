import sys
import random


def generate_graph(num_nodes, filename, connected):
    nodes = [f"n{i}" for i in range(num_nodes)]  # Generate node names (n0, n1, n2, ...)
    edges = set()

    if connected:
        # Ensure the graph is connected by creating a spanning tree first
        available_nodes = nodes[:]
        random.shuffle(available_nodes)

        for i in range(num_nodes - 1):
            edges.add((available_nodes[i], available_nodes[i + 1]))

        for _ in range(random.randint(num_nodes, num_nodes * 2)):
            src, dst = random.sample(nodes, 2)
            edges.add((src, dst))
    else:
        for _ in range(random.randint(num_nodes, num_nodes * 2)):  # Ensure complexity
            src, dst = random.sample(nodes, 2)
            edges.add((src, dst))  # Directed edge
    with open(filename, "w", encoding="utf-8") as file:  # Ensure UTF-8 encoding
        for node in nodes:
            file.write(f"* {node}\n")
        for edge in edges:
            file.write(f"- {edge[0]} {edge[1]} {random.randint(1, 10)}\n")

    print(f"Graph saved to {filename}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python script.py <num_nodes> <output_file> <connected>")
        sys.exit(1)

    num_nodes = int(sys.argv[1])
    filename = sys.argv[2]
    connected = sys.argv[3].lower() == "true"

    if num_nodes < 2:
        print("Number of nodes must be at least 2.")
        sys.exit(1)

    generate_graph(num_nodes, filename, connected)