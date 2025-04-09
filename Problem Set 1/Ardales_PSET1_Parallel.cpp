// Submitted by: Darius Vincent Ardales
// STDISCM - S13
// February 15, 2024
// Problem Set 1 - Graph Query: Paths Between Nodes [Parallel Implementation]

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
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

using namespace std;
using namespace chrono;

using Graph = map<string, vector<string>>;
int POOLSIZE = 80;

class ThreadPool
{
public:
  ThreadPool(size_t numThreads) : stop(false)
  {
    for (size_t i = 0; i < numThreads; i++)
    {
      workers.emplace_back([this]()
                           {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this]() { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                } });
    }
  }

  void enqueueTask(function<void()> task)
  {
    {
      lock_guard<mutex> lock(queueMutex);
      tasks.push(task);
    }
    condition.notify_one();
  }

  ~ThreadPool()
  {
    {
      lock_guard<mutex> lock(queueMutex);
      stop = true;
    }
    condition.notify_all();
    for (thread &worker : workers)
      worker.join();
  }

private:
  vector<thread> workers;
  queue<function<void()>> tasks;
  mutex queueMutex;
  condition_variable condition;
  bool stop;
};

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
void checkNode(const string &node, const Graph &graph, ThreadPool &pool)
{
  int total = graph.size();
  int numTasks = min(POOLSIZE, total);
  atomic<bool> found(false);
  atomic<int> tasksRemaining(numTasks);
  mutex cvMutex;
  condition_variable cv;

  // Calculate how many nodes each task should check.
  int batchSize = (total + numTasks - 1) / numTasks;

  // Vector of iterators that will mark the boundaries between tasks.
  vector<Graph::const_iterator> boundaries;
  auto it = graph.begin();
  boundaries.push_back(it);
  // Set boundaries by advancing the iterator in steps of batchSize.
  for (int i = 0; i < numTasks; i++)
  {
    // For the last task, ensure we go up to graph.end()
    if (i == numTasks - 1)
    {
      boundaries.push_back(graph.end());
      break;
    }
    advance(it, batchSize);
    boundaries.push_back(it);
  }

  for (int i = 0; i < numTasks; i++)
  {
    auto startIt = boundaries[i];
    auto endIt = boundaries[i + 1];
    pool.enqueueTask([&, startIt, endIt]()
                     {
      for (auto iter = startIt; iter != endIt && !found.load(); iter++) {
        // std::this_thread::sleep_for(std::chrono::microseconds(100));
        if (iter->first == node) {
          found = true;
          break;
        }
      }
      if (--tasksRemaining == 0) {
        lock_guard<mutex> lock(cvMutex);
        cv.notify_one();
      } });
  }

  // Wait until all tasks are finished.
  {
    unique_lock<mutex> lock(cvMutex);
    cv.wait(lock, [&]()
            { return tasksRemaining.load() == 0; });
  }

  cout << "Response: Node " << node << (found.load() ? " is" : " is not") << " in the graph.\n";
}

// Check if an edge exists in the graph.
void checkEdge(const string &src, const string &dest, const Graph &graph, ThreadPool &pool)
{
  // Divide the graph into segments to parallelize searching for the src node
  int total = graph.size();
  int numTasks = min(POOLSIZE, total);
  atomic<bool> found(false);
  atomic<int> tasksRemaining(numTasks);
  mutex cvMutex;
  condition_variable cv;

  // Calculate how many nodes each task should check.
  int batchSize = (total + numTasks - 1) / numTasks;

  // Vector of iterators that will mark the boundaries between tasks.
  vector<Graph::const_iterator> boundaries;
  auto it = graph.begin();
  boundaries.push_back(it);
  // Set boundaries by advancing the iterator in steps of batchSize.
  for (int i = 0; i < numTasks; i++)
  {
    // For the last task, ensure we go up to graph.end()
    if (i == numTasks - 1)
    {
      boundaries.push_back(graph.end());
      break;
    }
    advance(it, batchSize);
    boundaries.push_back(it);
  }

  for (int i = 0; i < numTasks; i++)
  {
    auto startIt = boundaries[i];
    auto endIt = boundaries[i + 1];
    pool.enqueueTask([&, startIt, endIt]()
                     {
      for (auto iter = startIt; iter != endIt && !found.load(); iter++)
      {
        // std::this_thread::sleep_for(std::chrono::microseconds(100));
        if (iter->first == src)
        {
          // If src is found, check adjacency list for dest
          for (const auto &neighbor : iter->second)
          {
            if (neighbor == dest)
            {
              found = true;
              break;
            }
          }
        }
        if (found.load()) break;
      }
      if (--tasksRemaining == 0)
      {
        lock_guard<mutex> lock(cvMutex);
        cv.notify_one();
      } });
  }

  // Wait until all tasks are finished.
  {
    unique_lock<mutex> lock(cvMutex);
    cv.wait(lock, [&]
            { return tasksRemaining.load() == 0; });
  }

  cout << "Response: Edge (" << src << ", " << dest << ") " << (found.load() ? "is" : "is not") << " in the graph.\n";
}

// Find and display a path from src to dest using Breadth-First Search (BFS).
void bfsPath(const string &src, const string &dest, const Graph &graph, ThreadPool &pool)
{
  map<string, bool> visited;
  map<string, string> parent;
  for (const auto &entry : graph)
    visited[entry.first] = false;
  visited[src] = true;

  vector<string> frontier = {src};
  atomic<bool> found(false);

  // Process BFS level by level.
  while (!frontier.empty() && !found.load())
  {
    int total = frontier.size();
    int numTasks = min(POOLSIZE, total);
    atomic<int> tasksRemaining(numTasks);
    // Each task will record its discovered nodes as (neighbor, parent) pairs.
    vector<vector<pair<string, string>>> localCandidates(numTasks);
    mutex cvMutex;
    condition_variable cv;
    int batchSize = (total + numTasks - 1) / numTasks;

    // Launch tasks to process batches of the current frontier.
    for (int i = 0; i < numTasks; i++)
    {
      int start = i * batchSize;
      int end = min(start + batchSize, total);
      pool.enqueueTask([&, i, start, end]()
                       {
                vector<pair<string, string>> candidates;
                for (int j = start; j < end && !found.load(); j++)
                {
                    string node = frontier[j];
                    // Process all neighbors of this node.
                    for (const auto &neighbor : graph.at(node))
                    {
                        if (found.load()) break;  // Early exit if dest already found.
                        // Collect the candidate without locking.
                        candidates.push_back({ neighbor, node });
                        if (neighbor == dest)
                            found = true;
                    }
                }
                localCandidates[i] = move(candidates);
                if (--tasksRemaining == 0)
                {
                    lock_guard<mutex> lock(cvMutex);
                    cv.notify_one();
                } });
    }
    // Wait for all tasks for this level to complete.
    {
      unique_lock<mutex> lock(cvMutex);
      cv.wait(lock, [&]()
              { return tasksRemaining.load() == 0; });
    }

    // Merge the local candidate lists and update visited and parent.
    vector<string> nextFrontier;
    for (const auto &candList : localCandidates)
    {
      for (const auto &p : candList)
      {
        const string &neighbor = p.first;
        const string &par = p.second;
        if (!visited[neighbor])
        {
          visited[neighbor] = true;
          parent[neighbor] = par;
          nextFrontier.push_back(neighbor);
        }
      }
    }
    frontier = move(nextFrontier);
  }

  // Reconstruct and display the path if found.
  if (found.load())
  {
    vector<string> path;
    string current = dest;
    while (current != src)
    {
      path.push_back(current);
      current = parent[current];
    }
    path.push_back(src);
    reverse(path.begin(), path.end());

    cout << "Path: ";
    for (size_t i = 0; i < path.size() - 1; i++)
      cout << path[i] << " -> ";
    cout << path.back() << endl;
  }
  else
  {
    cout << "No path from " << src << " to " << dest << endl;
  }
}

// Handle interactive graph queries.
void handleGraphQueries(const Graph &graph, ThreadPool &pool)
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
        checkNode(src, graph, pool);
    }
    else if (cmd == "edge")
    {
      if (iss >> src >> dest)
        checkEdge(src, dest, graph, pool);
    }
    else if (cmd == "path")
    {
      if (iss >> src >> dest)
        bfsPath(src, dest, graph, pool);
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

  cout << "======= PARALLEL IMPLEMENTATION =======" << endl;

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

  ThreadPool pool(POOLSIZE);
  handleGraphQueries(graph, pool);

  return 0;
}
