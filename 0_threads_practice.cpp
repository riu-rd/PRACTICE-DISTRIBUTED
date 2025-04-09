#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <windows.h>
#include <chrono>

using namespace std;
using namespace chrono;

// Thread Pool Information

int poolSize = 8;
thread *threadPool = new thread[poolSize];
uint64_t currentTask[8] = {0, 0, 0, 0, 0, 0, 0, 0};
bool tasksDone = false;

// Task Information

int completedTasks = 0;
int primeCount = 0;
vector<uint64_t> primeCheckTasks;

void getProcessTime(FILETIME &kernelTime, FILETIME &userTime)
{
  FILETIME createTime, exitTime;
  GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime);
}

double fileTimeToSeconds(const FILETIME &ft)
{
  ULARGE_INTEGER ui;
  ui.LowPart = ft.dwLowDateTime;
  ui.HighPart = ft.dwHighDateTime;
  return (double)ui.QuadPart / 10'000'000; // Convert to seconds
}

bool isPrime(uint64_t n, string runner)
{
  bool is_prime = true;
  uint64_t sqrt_n = (uint64_t)ceil(sqrt(n));

  if (n == 0 || n == 1)
  {
    return false;
  }

  for (uint64_t i = 2; i <= sqrt_n; i++)
  {
    if (n % i == 0)
    {
      is_prime = false;
      break;
    }
  }

  if (is_prime)
  {
    cout << runner << ": " << n << " is PRIME." << endl;
    primeCount++;
  }

  return is_prime;
}

void primeTask(int index)
{
  uint64_t numToCheck = 0;
  string threadName = "T-" + to_string(index);
  while (!tasksDone)
  {
    numToCheck = currentTask[index];
    if (numToCheck != 0)
    {
      isPrime(numToCheck, threadName);
      currentTask[index] = 0;
    }
  }
}

void demo_01(size_t listSize, uint64_t *numberList)
{
  for (int i = 0; i < listSize; i++)
  {
    isPrime(numberList[i], "MAIN");
  }
}

void demo_02(size_t listSize, uint64_t *numberList)
{
  vector<thread> workerThreads;

  for (int i = 0; i < listSize; i++)
  {
    workerThreads.emplace_back(isPrime, numberList[i], "T-" + to_string(i));
  }

  for (auto &t : workerThreads)
  {
    t.join();
  }
}

void demo_03(size_t listSize, uint64_t *numberList)
{
  for (int i = 0; i < listSize; i++)
  {
    primeCheckTasks.push_back(numberList[i]);
  }

  for (int i = 0; i < poolSize; i++)
  {
    threadPool[i] = thread{primeTask, i};
  }

  while (primeCheckTasks.size() != 0)
  {
    for (int i = 0; i < poolSize; i++)
    {
      if (currentTask[i] == 0)
      {
        currentTask[i] = primeCheckTasks[0];
        primeCheckTasks.erase(primeCheckTasks.begin());
        break;
      }
    }
  }

  tasksDone = true;

  for (int i = 0; i < poolSize; i++)
  {
    threadPool[i].join();
  }
}

int main()
{
  int play;

  cout << "Select Demo: ";
  cin >> play;

  cout << endl
       << "===== MAIN: START ===== " << endl;

  size_t listSize = 80;
  uint64_t *numberList = new uint64_t[listSize];

  for (size_t i = 0; i < listSize; i++)
  {
    if (i % 8 == 0)
    {
      numberList[i] = (uint64_t)9446744074328015681;
      continue;
    }
    numberList[i] = (uint64_t)9446744073709551617;
  }

  // Start timing
  auto startTime = high_resolution_clock::now();
  FILETIME startKernelTime, startUserTime;
  getProcessTime(startKernelTime, startUserTime);

  switch (play)
  {
  case 1:
    demo_01(listSize, numberList);
    break;
  case 2:
    demo_02(listSize, numberList);
    break;
  case 3:
    demo_03(listSize, numberList);
    break;
  }

  // Stop timing
  auto endTime = high_resolution_clock::now();
  FILETIME endKernelTime, endUserTime;
  getProcessTime(endKernelTime, endUserTime);

  // Compute elapsed times
  auto realTime = duration_cast<seconds>(endTime - startTime).count();
  double userTime = fileTimeToSeconds(endUserTime) - fileTimeToSeconds(startUserTime);
  double systemTime = fileTimeToSeconds(endKernelTime) - fileTimeToSeconds(startKernelTime);

  cout << "Prime Count: " << primeCount << endl;
  cout << "===== MAIN: END =====" << endl;
  cout << "Real Time: " << realTime / 60 << " min " << realTime % 60 << " sec" << endl;
  cout << "User Time: " << (int)userTime / 60 << " min " << fmod(userTime, 60) << " sec" << endl;
  cout << "System Time: " << (int)systemTime / 60 << " min " << fmod(systemTime, 60) << " sec" << endl;

  return 0;
}