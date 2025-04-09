// Submitted by: Darius Vincent Ardales
// STDISCM - S13
// January 31, 2025

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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

  parseGraphFile(graphFileName, nodes, edges);
  handleGraphQueries(nodes, edges);

  return 0;
}
