#include <iostream>
#include <thread>
#include <string>
#include <cmath> // sqrt()
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <mutex>
#include <semaphore>


using namespace std;

/* ========== DEMO 0: Thread Functions ========== */

int count = 0;

void produce(string thName) {
  count++;
  cout << thName << " count = " << count << "\n";
}

void consume(string thName) {
  count--;
  cout << thName << " count = " << count << "\n";
}

/* ========== DEMO 1: Thread Functions ========== */

counting_semaphore m(1); // Mutex for access to 'count' variable
counting_semaphore S(0); // 'Non-empty signal: S>=1 - produce

void _produce(string thName) {
  // 1. Wait for exclusive access to 'count' variable.
  //    (Acquire mutex lock for 'count' variable.)
  // 2. Increment 'count' variable.
  // 3. Release 'count' variable.
  m.acquire();
  count++;
  cout << thName << " count = " << count << "\n";
  m.release();

  // 4. Signal to 'consumer' threads.
  S.release();
}


void _consume(string thName) {

  // 1. Wait for a signal from a 'producer' thread.
  S.acquire();

  // 2. Wait for exclusive access to 'count' variable.
  //    (Acquire mutex lock for 'count' variable.)
  // 3  Decrement 'count' variable.
  // 4. Release 'count' variable.
  m.acquire();
  count--;
  cout << thName << " count = " << count << "\n";
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
  thread _C1(_consume, "C1");
  thread _P1(_produce, "P1");
  thread _C2(_consume, "C2");
  thread _P2(_produce, "P2");
  thread _C3(_consume, "C3");
  thread _P3(_produce, "P3");
  _C1.join();
  _P1.join();
  _C2.join();
  _P2.join();
  _C3.join();
  _P3.join();
  cout << "===== DEMO 1:END =====\n\n";


  cout << "\nMAIN: END\n"; 

  return 0;
}




