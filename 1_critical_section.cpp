#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <windows.h>

using namespace std;

int SharedValue = 0;
int threadCount = 2;

bool *goingCritical = new bool[threadCount];
int turn = 0;

/*================== WriteOnShared_0 ==================*/

void WriteOnShared_0(int threadID)
{
  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  SharedValue++;

  cout << thName + ": END\n";
}

/*================== WriteOnShared_1 ==================*/

void WriteOnShared_1(int threadID)
{
  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  // for loop block - increase function/thread life time
  int x = 0;
  for (int i = 0; i < 100 * 1000; i++)
  {
    x += 4324234234 * i;
    x = x % 423423432;
  }

  SharedValue++;

  cout << thName + ": END\n";
}

/*================== WriteOnShared_2 ==================*/

void WriteOnShared_2(int threadID)
{
  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  // for loop - increases no of access to SharedValue
  for (int i = 0; i < 10 * 1000; i++)
  {
    SharedValue++;
    SharedValue--;
  }

  cout << thName + ": END\n";
}

/*================== WriteOnShared_3a ==================*/
/* PETERSON'S ALGORITHM */

void WriteOnShared_3a(int threadID)
{
  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  int otherID = 1 - threadID;

  /* ===== PRE-CRITICAL =====*/
  goingCritical[threadID] = true;
  turn = otherID;

  while (goingCritical[otherID] && turn != threadID)
  {
    // busy waiting
  }

  /* ===== CRITICAL SECTION =====*/
  cout << thName + ": IN CRITICAL\n";
  for (int i = 0; i < 10 * 1000; i++)
  {
    SharedValue++;
    SharedValue--;
  }

  /* ===== POST-CRITICAL =====*/
  goingCritical[threadID] = false;
  cout << thName + ": END\n";
}

/*================== WriteOnShared_3b ==================*/
/* PETERSON'S ALGORITHM with Logs*/

void WriteOnShared_3b(int threadID)
{
  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  int otherID = 1 - threadID;

  /* ===== PRE-CRITICAL =====*/
  goingCritical[threadID] = true;
  turn = otherID;

  bool firstWait = true;
  while (goingCritical[otherID] && turn != threadID)
  {
    if (firstWait)
    {
      cout << thName + ": WAITING\n";
      firstWait = false;
    }
    // busy waiting
  }

  /* ===== CRITICAL SECTION =====*/
  cout << thName + ": IN CRITICAL\n";
  for (int i = 0; i < 10 * 1000; i++)
  {
    SharedValue++;
    SharedValue--;
  }

  /* ===== POST-CRITICAL =====*/
  goingCritical[threadID] = false;
  cout << thName + ": END\n";
}

/*================== M A I N ==================*/

int main()
{
  cout << "MAIN: START\n";
  vector<thread> myThreads;

  for (int i = 0; i < threadCount; i++)
  {
    myThreads.emplace_back(WriteOnShared_3b, i);
  }

  for (auto &thr : myThreads)
  {
    thr.join();
  }

  cout << "MAIN: SharedValue: " + to_string(SharedValue) + "\n";
  cout << "MAIN: END\n";

  return 0;
}