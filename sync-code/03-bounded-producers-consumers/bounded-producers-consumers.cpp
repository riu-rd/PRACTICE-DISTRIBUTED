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

counting_semaphore m(1);
counting_semaphore Sbuff(3);
counting_semaphore Sp(0);

void produceSync(string thName)
{

  Sbuff.acquire();

  m.acquire();
  counter++;
  cout << thName << " count = " << counter << "\n";
  m.release();

  Sp.release();
}

void consumeSync(string thName)
{

  Sp.acquire();

  m.acquire();
  counter--;
  cout << thName << " count = " << counter << "\n";
  m.release();

  Sbuff.release();
}

/* =============== MAIN =============== */

int main()
{
  cout << "\nMAIN: START\n";

  /* ===== DEMO 0: No Synchronization ===== */

  cout << "\n\n===== DEMO 0:START: No Synchronization =====\n";
  thread C1(consume, "C1");
  thread P1(produce, "P1");
  thread P2(produce, "P2");
  thread P3(produce, "P3");
  thread P4(produce, "P4");
  thread P5(produce, "P5");
  thread C2(consume, "C2");
  thread C3(consume, "C3");
  thread C4(consume, "C4");
  thread C5(consume, "C5");
  C1.join();
  P1.join();
  C2.join();
  P2.join();
  C3.join();
  P3.join();
  C4.join();
  P4.join();
  C5.join();
  P5.join();
  cout << "===== DEMO 0:END =====\n\n";

  /* ===== DEMO 1: Bounded Producer-Consumer Synchronized ===== */

  cout << "\n===== DEMO 1: START: Bounded Producer-Consumer Synchronized =====\n";
  thread C1Sync(consumeSync, "C1");
  thread P1Sync(produceSync, "P1");
  thread P2Sync(produceSync, "P2");
  thread P3Sync(produceSync, "P3");
  thread P4Sync(produceSync, "P4");
  thread P5Sync(produceSync, "P5");
  thread C2Sync(consumeSync, "C2");
  thread C3Sync(consumeSync, "C3");
  thread C4Sync(consumeSync, "C4");
  thread C5Sync(consumeSync, "C5");
  C1Sync.join();
  P1Sync.join();
  C2Sync.join();
  P2Sync.join();
  C3Sync.join();
  P3Sync.join();
  C4Sync.join();
  P4Sync.join();
  C5Sync.join();
  P5Sync.join();
  cout << "===== DEMO 1:END =====\n\n";

  cout << "\nMAIN: END\n";

  return 0;
}
