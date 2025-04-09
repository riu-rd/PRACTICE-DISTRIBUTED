// Submitted by: Darius Vincent Ardales
// STDISCM - S13
// February 15, 2024
// Problem Set 1 - Graph Query: Paths Between Nodes [Sequential Implementation]

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <chrono>
#include <windows.h>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <thread>

using namespace std;
using namespace chrono;

using Graph = map<string, vector<string>>;

// Get the process times (kernel and user) using Windows API.
void getProcessTime(FILETIME &kernelTime, FILETIME &userTime)
{
  FILETIME createTime, exitTime;
  GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime);
}

// Convert FILETIME to seconds.
double fileTimeToSeconds(const FILETIME &ft)
{
  ULARGE_INTEGER ui;
  ui.LowPart = ft.dwLowDateTime;
  ui.HighPart = ft.dwHighDateTime;
  return static_cast<double>(ui.QuadPart) / 10000000; // Convert to seconds
}

// Parse the graph file and build the Graph.
void parseGraphFile(const string &filename, Graph &graph)
{
  ifstream file(filename);
  string line, src, dest;

  while (getline(file, line))
  {
    if (line.empty())
      continue;

    if (line[0] == '*')
    {
      istringstream iss(line.substr(1)); // Skip the '*' character
      iss >> src;
      graph[src] = vector<string>();
    }
    else if (line[0] == '-')
    {
      istringstream iss(line.substr(1)); // Skip the '-' character
      iss >> src >> dest;
      graph[src].push_back(dest);
    }
  }
  file.close();
  cout << "Graph File: " << filename << " has been loaded.\n";
}

// Display all nodes in the graph.
void displayNodes(const Graph &graph)
{
  cout << "\n= = Response = =\nNodes: ";
  for (auto it = graph.begin(); it != graph.end(); it++)
  {
    cout << it->first << " ";
  }
  cout << "\n";
}

// Display all edges in the graph.
void displayEdges(const Graph &graph)
{
  cout << "\n= = Response = =\nEdge List:\n";
  for (auto it = graph.begin(); it != graph.end(); it++)
  {
    for (size_t i = 0; i < it->second.size(); i++)
    {
      cout << "(" << it->first << ", " << it->second[i] << ")\n";
    }
  }
}

// Check if a node exists in the graph.
void checkNode(const string &node, const Graph &graph)
{
  bool found = false;
  for (auto it = graph.begin(); it != graph.end(); it++)
  {
    // std::this_thread::sleep_for(std::chrono::microseconds(100));
    if (it->first == node)
    {
      found = true;
      break;
    }
  }
  cout << "Response: Node " << node << (found ? " is" : " is not") << " in the graph.\n";
}

// Check if an edge exists in the graph.
void checkEdge(const string &src, const string &dest, const Graph &graph)
{
  bool found = false;
  bool srcFound = false;
  for (auto it = graph.begin(); it != graph.end(); ++it)
  {
    // std::this_thread::sleep_for(std::chrono::microseconds(100));
    if (it->first == src)
    {
      srcFound = true;
      // Once found, iterate over its neighbors.
      for (const auto &neighbor : it->second)
      {
        if (neighbor == dest)
        {
          found = true;
          break;
        }
      }
      break;
    }
  }
  cout << "Response: Edge (" << src << ", " << dest << ") "
       << (found ? "is" : "is not")
       << " in the graph.\n";
}

// Find and display a path from src to dest using Breadth-First Search (BFS).
void bfsPath(const string &src, const string &dest, const Graph &graph)
{
  map<string, bool> visited;
  map<string, string> parent;

  // Mark all nodes as unvisited.
  for (auto it = graph.begin(); it != graph.end(); it++)
    visited[it->first] = false;

  queue<string> q;
  visited[src] = true;
  q.push(src);

  bool foundPath = false;

  // BFS loop.
  while (!q.empty())
  {
    string current = q.front();
    q.pop();

    // Visit all neighbors of the current node.
    for (size_t i = 0; i < graph.at(current).size(); i++)
    {
      string neighbor = graph.at(current)[i];
      if (!visited[neighbor])
      {
        visited[neighbor] = true;
        parent[neighbor] = current;
        q.push(neighbor);

        if (neighbor == dest)
        {
          foundPath = true;
          break;
        }
      }
    }
    if (foundPath)
      break;
  }

  // Reconstruct and display the path if found.
  if (foundPath)
  {
    vector<string> path;
    string current = dest;
    while (current != src)
    {
      path.push_back(current);
      current = parent[current];
    }
    path.push_back(src);

    // Reverse the path.
    reverse(path.begin(), path.end());

    cout << "Path: ";
    for (size_t i = 0; i < path.size() - 1; i++)
    {
      cout << path[i] << " -> ";
    }
    cout << path.back() << endl;
  }
  else
  {
    cout << "No path from " << src << " to " << dest << endl;
  }
}

// Handle interactive graph queries.
void handleGraphQueries(const Graph &graph)
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

    // Start timing.
    auto startTime = high_resolution_clock::now();
    FILETIME startKernelTime, startUserTime;
    getProcessTime(startKernelTime, startUserTime);

    if (cmd == "exit")
    {
      cout << "\nExiting.";
      break;
    }
    else if (cmd == "nodes")
    {
      displayNodes(graph);
    }
    else if (cmd == "edges")
    {
      displayEdges(graph);
    }
    else if (cmd == "node")
    {
      if (iss >> src)
        checkNode(src, graph);
    }
    else if (cmd == "edge")
    {
      if (iss >> src >> dest)
        checkEdge(src, dest, graph);
    }
    else if (cmd == "path")
    {
      if (iss >> src >> dest)
        bfsPath(src, dest, graph);
    }
    else
    {
      cout << "INVALID QUERY.\n";
    }

    // Stop timing.
    auto endTime = high_resolution_clock::now();
    FILETIME endKernelTime, endUserTime;
    getProcessTime(endKernelTime, endUserTime);

    // Compute elapsed times.
    auto realTime = duration_cast<seconds>(endTime - startTime).count();
    double userTime = fileTimeToSeconds(endUserTime) - fileTimeToSeconds(startUserTime);
    double systemTime = fileTimeToSeconds(endKernelTime) - fileTimeToSeconds(startKernelTime);
    cout << "====================================" << endl;
    cout << "Real Time: " << realTime / 60 << " min " << realTime % 60 << " sec" << endl;
    cout << "User Time: " << (int)userTime / 60 << " min " << fmod(userTime, 60) << " sec" << endl;
    cout << "System Time: " << (int)systemTime / 60 << " min " << fmod(systemTime, 60) << " sec" << endl;
    cout << "====================================" << endl;
  }
}

int main(int argc, char *argv[])
{
  string graphFileName = (argc > 1) ? argv[1] : "";
  ifstream file;
  Graph graph;

  cout << "======= SEQUENTIAL IMPLEMENTATION =======" << endl;

  while (true)
  {
    if (!graphFileName.empty() && (file.open(graphFileName), file))
    {
      cout << "Graph File: " << graphFileName << endl;
      break;
    }
    if (!graphFileName.empty())
      cout << "Error: File '" << graphFileName << "' does not exist.\n";
    cout << "Enter graph file name: ";
    getline(cin, graphFileName);
  }
  file.close();

  parseGraphFile(graphFileName, graph);
  handleGraphQueries(graph);

  return 0;
}