import sys
import random

def generate_square_graph(side_length, filename):
    nodes = []
    edges = set()
    
    # Create nodes for an n x n grid with sequential names
    total_nodes = side_length * side_length
    for i in range(total_nodes):
        nodes.append(f"n{i}")
    
    # Create edges to connect adjacent nodes (ensuring the graph is connected)
    for r in range(side_length):
        for c in range(side_length):
            current_index = r * side_length + c
            # Connect to the right neighbor, if it exists
            if c < side_length - 1:
                right_neighbor_index = current_index + 1
                edges.add((nodes[current_index], nodes[right_neighbor_index]))
            # Connect to the bottom neighbor, if it exists
            if r < side_length - 1:
                bottom_neighbor_index = current_index + side_length
                edges.add((nodes[current_index], nodes[bottom_neighbor_index]))
    
    # Write the nodes and edges to the file
    with open(filename, "w", encoding="utf-8") as file:
        for node in nodes:
            file.write(f"* {node}\n")
        for edge in edges:
            file.write(f"- {edge[0]} {edge[1]} {random.randint(1, 10)}\n")
    
    print(f"Graph saved to {filename}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python square_graph.py <side_length> <output_file>")
        sys.exit(1)
    
    try:
        side_length = int(sys.argv[1])
        if side_length < 1:
            raise ValueError
    except ValueError:
        print("Side length must be an integer greater than 0.")
        sys.exit(1)
    
    filename = sys.argv[2]
    generate_square_graph(side_length, filename)
