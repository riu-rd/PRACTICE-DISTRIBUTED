// Submitted by: Darius Vincent Ardales
// STDISCM - S13
// January 31, 2025

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <unordered_set>
#include <unordered_map>

using namespace std;

void parseGraphFile(const string &filename, unordered_set<string> &nodes, unordered_map<string, unordered_set<string>> &edges)
{
  ifstream file(filename);
  string line, src, dest;

  while (getline(file, line))
  {
    if (line.empty())
      continue;
    istringstream iss(line.substr(1)); // Skip '*' or '-'
    if (line[0] == '*')
    {
      iss >> src;
      nodes.insert(src);
    }
    else if (line[0] == '-')
    {
      iss >> src >> dest;
      edges[src].insert(dest);
    }
  }
  file.close();
  cout << "Graph File: " << filename << " has been loaded.\n";
}

void handleGraphQueries(const unordered_set<string> &nodes, const unordered_map<string, unordered_set<string>> &edges)
{
  string line, cmd, src, dest;

  while (true)
  {
    cout << "\nGraph Query: ";
    if (!getline(cin, line))
    {
      cout << "\nError Parsing Input.";
      break;
    }
    if (line.empty())
      continue; // Skip empty input

    istringstream iss(line);
    iss >> cmd;

    if (cmd == "exit")
    {
      cout << "\nExiting.";
      break;
    }
    else if (cmd == "nodes")
    {
      cout << "\n= = Response = =\nNodes: ";
      for (const auto &n : nodes)
        cout << n << " ";
      cout << endl;
    }
    else if (cmd == "edges")
    {
      cout << "\n= = Response = =\nEdge List:\n";
      for (const auto &[node, adjList] : edges)
        for (const auto &destNode : adjList)
          cout << "(" << node << ", " << destNode << ")\n";
    }
    else if (cmd == "node")
    {
      if (iss >> src)
      {
        cout << "Response: Node " << src << (nodes.count(src) ? " is" : " is not") << " in the graph.\n";
      }
    }
    else if (cmd == "edge")
    {
      if (iss >> src >> dest)
      {
        cout << "Response: Edge (" << src << ", " << dest << ") "
             << (edges.count(src) && edges.at(src).count(dest) ? "is" : "is not")
             << " in the graph.\n";
      }
    }
    else
      cout << "INVALID QUERY.\n";
  }
}

void txtToDot(const string &filename)
{
  ifstream inputFile(filename);

  if (!inputFile)
  {
    cerr << "Error: Cannot open file " << filename << endl;
    return;
  }

  string dotFileName = filename.substr(0, filename.find_last_of(".")) + ".dot";
  ofstream outputFile(dotFileName);
  if (!outputFile)
  {
    cerr << "Error: Cannot create output file " << dotFileName << endl;
    return;
  }

  outputFile << "digraph G {\n";

  string line;
  while (getline(inputFile, line))
  {
    istringstream iss(line);
    string type, node1, node2;
    iss >> type >> node1 >> node2;

    if (type == "*")
    {
      outputFile << "    " << node1 << ";\n";
    }
    else if (type == "-")
    {
      outputFile << "    " << node1 << " -> " << node2 << ";\n";
    }
  }

  outputFile << "}\n";
  inputFile.close();
  outputFile.close();
  cout << "DOT file created: " << dotFileName << endl;
}

void visualizeGraph(const string &filename)
{
  ifstream file(filename);
  if (!file)
  {
    cout << "Error: File '" << filename << "' not found.\n";
    return;
  }
  file.close();

  // Generate PNG from the DOT file
  string command = "dot -Tpng " + filename + " -o graph.png";
  int result = system(command.c_str()); // C-style string const char*
  if (result != 0)
  {
    cout << "Error: Failed to generate graph image.\n";
    return;
  }

  // Open the generated PNG file
  cout << "Graph visualization saved as 'graph.png'. Opening image...\n";
  system("start graph.png");
}

int main(int argc, char *argv[])
{
  string graphFileName = (argc > 1) ? argv[1] : "";
  ifstream file;
  unordered_set<string> nodes;
  unordered_map<string, unordered_set<string>> edges;

  while (true)
  {
    if (!graphFileName.empty() && (file.open(graphFileName), file))
    {
      cout << "Graph File: " << graphFileName << endl;
      break;
    }
    if (!graphFileName.empty()) // ignore empty inputs
      cout << "Error: File '" << graphFileName << "' does not exist.\n";
    cout << "Enter graph file name: ";
    getline(cin, graphFileName);
  }
  file.close();

  // if file name ends with .txt, create a .dot file
  if (graphFileName.substr(graphFileName.size() - 4) == ".txt")
  {
    txtToDot(graphFileName);
  }

  visualizeGraph(graphFileName.substr(0, graphFileName.find_last_of(".")) + ".dot");
  parseGraphFile(graphFileName, nodes, edges);
  handleGraphQueries(nodes, edges);

  return 0;
}
