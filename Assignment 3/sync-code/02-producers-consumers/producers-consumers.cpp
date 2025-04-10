#include <iostream>
#include <thread>
#include <string>
#include <cmath> // sqrt()
#include <cstdlib>
#include <vector>
// #include <unistd.h>
#include <mutex>
#include <semaphore>

using namespace std;

/* ========== DEMO 0: Thread Functions ========== */

int counter = 0;

void produce(string thName)
{
  counter++;
  cout << thName << " count = " << counter << "\n";
}

void consume(string thName)
{
  counter--;
  cout << thName << " count = " << counter << "\n";
}

/* ========== DEMO 1: Thread Functions ========== */

counting_semaphore m(1); // Mutex for access to 'count' variable
counting_semaphore S(0); // 'Non-empty signal: S>=1 - produce

void produceSync(string thName)
{
  // 1. Wait for exclusive access to 'count' variable.
  //    (Acquire mutex lock for 'count' variable.)
  // 2. Increment 'count' variable.
  // 3. Release 'count' variable.
  m.acquire();
  counter++;
  cout << thName << " count = " << counter << "\n";
  m.release();

  // 4. Signal to 'consumer' threads.
  S.release();
}

void consumeSync(string thName)
{

  // 1. Wait for a signal from a 'producer' thread.
  S.acquire();

  // 2. Wait for exclusive access to 'count' variable.
  //    (Acquire mutex lock for 'count' variable.)
  // 3  Decrement 'count' variable.
  // 4. Release 'count' variable.
  m.acquire();
  counter--;
  cout << thName << " count = " << counter << "\n";
  m.release();
}

/* =============== MAIN =============== */

int main()
{
  cout << "\nMAIN: START\n";

  /* ===== DEMO 0: No Synchronization ===== */

  cout << "\n\n===== DEMO 0:START: No Synchronization =====\n";
  thread C1(consume, "C1");
  thread P1(produce, "P1");
  thread C2(consume, "C2");
  thread P2(produce, "P2");
  thread C3(consume, "C3");
  thread P3(produce, "P3");
  C1.join();
  P1.join();
  C2.join();
  P2.join();
  C3.join();
  P3.join();
  cout << "===== DEMO 0:END =====\n\n";

  /* ===== DEMO 1: Producer-Consumer Synchronized ===== */

  cout << "\n===== DEMO 1: START: Producer-Consumer Synchronized =====\n";
  thread C1Sync(consumeSync, "C1");
  thread P1Sync(produceSync, "P1");
  thread C2Sync(consumeSync, "C2");
  thread P2Sync(produceSync, "P2");
  thread C3Sync(consumeSync, "C3");
  thread P3Sync(produceSync, "P3");
  C1Sync.join();
  P1Sync.join();
  C2Sync.join();
  P2Sync.join();
  C3Sync.join();
  P3Sync.join();
  cout << "===== DEMO 1:END =====\n\n";

  cout << "\nMAIN: END\n";

  return 0;
}
