#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>

using namespace std;

// Generate node names: 0 -> "a", 1 -> "b", …, 25 -> "z", then 26 -> "a1", 27 -> "b1", etc.
string getNodeName(int id)
{
  int cycle = id / 26;
  int letter = id % 26;
  char letterChar = 'a' + letter;
  if (cycle == 0)
    return string(1, letterChar);
  else
    return string(1, letterChar) + to_string(cycle);
}

int main(int argc, char *argv[])
{
  long long numNodes = 2000000; // EDIT
  if (argc > 1)
  {
    numNodes = atoll(argv[1]);
  }

  // Arrange nodes in a square grid.
  int side = static_cast<int>(sqrt(numNodes));
  long long totalNodes = static_cast<long long>(side) * side;
  if (totalNodes != numNodes)
  {
    cout << "Adjusted number of nodes to " << totalNodes
         << " to form a perfect square grid." << endl;
    numNodes = totalNodes;
  }
  cout << "Generating graph with grid size " << side << " x " << side
       << " (" << numNodes << " nodes)" << endl;

  // Generate node names and store them in row-major order.
  vector<string> nodes;
  nodes.reserve(numNodes);
  for (int i = 0; i < side; i++)
  {
    for (int j = 0; j < side; j++)
    {
      int id = i * side + j;
      nodes.push_back(getNodeName(id));
    }
  }

  // ----------------------
  // Graph1: Directed Acyclic Graph with local forward connectivity.
  // Each node connects only to nodes "ahead" (to the right or in lower rows)
  // within a local radius (here r = 2). This ensures that the path from the
  // top-left to the bottom-right spans many levels.
  // ----------------------
  {
    ofstream out("graph1.txt");
    if (!out.is_open())
    {
      cerr << "Error opening file graph1.txt for writing." << endl;
      return 1;
    }
    // Write nodes.
    for (const auto &name : nodes)
    {
      out << "* " << name << "\n";
    }
    // Write edges.
    int r = 2; // local connectivity radius
    for (int i = 0; i < side; i++)
    {
      for (int j = 0; j < side; j++)
      {
        int currentIndex = i * side + j;
        // For di==0, only look to the right (dj >= 1);
        // for di > 0, allow dj from -r to r.
        for (int di = 0; di <= r; di++)
        {
          int ni = i + di;
          if (ni >= side)
            break; // no more rows
          int startDj = (di == 0) ? 1 : -r;
          for (int dj = startDj; dj <= r; dj++)
          {
            int nj = j + dj;
            if (nj < 0 || nj >= side)
              continue;
            int neighborIndex = ni * side + nj;
            out << "- " << nodes[currentIndex] << " "
                << nodes[neighborIndex] << "\n";
          }
        }
      }
    }
    out.close();
    cout << "graph1.txt generated." << endl;
  }

  // ----------------------
  // Graph2: Directed Graph with full local connectivity.
  // Each node connects to every other node in its local neighborhood
  // (within radius r = 2, in all directions, except itself). This creates
  // high branching factors and cycles.
  // ----------------------
  {
    ofstream out("graph2.txt");
    if (!out.is_open())
    {
      cerr << "Error opening file graph2.txt for writing." << endl;
      return 1;
    }
    // Write nodes.
    for (const auto &name : nodes)
    {
      out << "* " << name << "\n";
    }
    // Write edges.
    int r = 2;
    for (int i = 0; i < side; i++)
    {
      for (int j = 0; j < side; j++)
      {
        int currentIndex = i * side + j;
        for (int di = -r; di <= r; di++)
        {
          for (int dj = -r; dj <= r; dj++)
          {
            if (di == 0 && dj == 0)
              continue; // skip self
            int ni = i + di;
            int nj = j + dj;
            if (ni < 0 || ni >= side || nj < 0 || nj >= side)
              continue;
            int neighborIndex = ni * side + nj;
            out << "- " << nodes[currentIndex] << " "
                << nodes[neighborIndex] << "\n";
          }
        }
      }
    }
    out.close();
    cout << "graph2.txt generated." << endl;
  }

  cout << "Graph generation complete." << endl;
  return 0;
}
