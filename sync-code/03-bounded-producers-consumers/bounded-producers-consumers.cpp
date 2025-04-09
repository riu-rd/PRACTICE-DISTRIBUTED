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

counting_semaphore m(1);
counting_semaphore Sbuff(3);
counting_semaphore Sp(0);

void _produce(string thName) {

  Sbuff.acquire();

  m.acquire();
  count++;
  cout << thName << " count = " << count << "\n";
  m.release();

  Sp.release();
}

void _consume(string thName) {

  Sp.acquire();

  m.acquire();
  count--;
  cout << thName << " count = " << count << "\n";
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
  thread _C1(_consume, "C1");
  thread _P1(_produce, "P1");
  thread _P2(_produce, "P2");
  thread _P3(_produce, "P3");
  thread _P4(_produce, "P4");
  thread _P5(_produce, "P5");
  thread _C2(_consume, "C2");
  thread _C3(_consume, "C3");
  thread _C4(_consume, "C4");
  thread _C5(_consume, "C5");
  _C1.join();
  _P1.join();
  _C2.join();
  _P2.join();
  _C3.join();
  _P3.join();
  _C4.join();
  _P4.join();
  _C5.join();
  _P5.join();
  cout << "===== DEMO 1:END =====\n\n";



  cout << "\nMAIN: END\n"; 

  return 0;
}




