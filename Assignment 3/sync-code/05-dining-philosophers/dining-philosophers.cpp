#include <iostream>
#include <thread>
#include <string>
#include <cmath> // sqrt()
#include <cstdlib>
#include <vector>
// #include <unistd.h>
#include <mutex>
#include <semaphore>
#include <chrono>
#include <algorithm>

using namespace std;

/* ========== DEMO 0: Thread Functions  ========== */

int chopstick_Status[5] = {0, 0, 0, 0, 0};
int philosopher_Status[5] = {0, 0, 0, 0, 0};
counting_semaphore print_m(1);

void philosophize(int id)
{
  for (int i = 0; i < 5; i++)
  {
    // Pick the two chopsticks
    chopstick_Status[(id + 0) % 5] = 1; // left chopstick at index id
    chopstick_Status[(id + 1) % 5] = 1; // right chopstick at index id+1

    print_m.acquire();
    string s = to_string(chopstick_Status[0]);
    s += to_string(chopstick_Status[1]);
    s += to_string(chopstick_Status[2]);
    s += to_string(chopstick_Status[3]);
    s += to_string(chopstick_Status[4]);
    cout << "P" << id << ": " << s << "\n";
    print_m.release();

    // Philosopher 'Eating'
    philosopher_Status[id] = 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    philosopher_Status[id] = 0;

    // Put down two chopsticks
    chopstick_Status[(id + 0) % 5] = 0; // left chopstick at index id
    chopstick_Status[(id + 1) % 5] = 0; // right chopstick at index id+1
  }
}

/* ========== DEMO 1: Thread Functions  ========== */

counting_semaphore<1> a(1);
counting_semaphore<1> b(1);
counting_semaphore<1> c(1);
counting_semaphore<1> d(1);
counting_semaphore<1> e(1);
counting_semaphore<1> *s_c[5] = {&a, &b, &c, &d, &e};
counting_semaphore m(1);
counting_semaphore<2> max_philosophers(2);

void _philosophize(int id)
{
  for (int i = 0; i < 5; i++)
  {
    max_philosophers.acquire();
    // Wait for the two chopsticks to be available
    s_c[id]->acquire();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    s_c[(id + 1) % 5]->acquire();

    // Pick the two chopsticks
    m.acquire();
    chopstick_Status[(id + 0) % 5] = 1; // left chopstick at index id
    chopstick_Status[(id + 1) % 5] = 1; // right chopstick at index id+1
    string s = to_string(chopstick_Status[0]);
    s += to_string(chopstick_Status[1]);
    s += to_string(chopstick_Status[2]);
    s += to_string(chopstick_Status[3]);
    s += to_string(chopstick_Status[4]);
    cout << "P" << id << ": " << s << "\n";
    m.release();

    // Philosopher 'Eating'
    philosopher_Status[id] = 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    philosopher_Status[id] = 0;

    // Put down two chopsticks
    m.acquire();
    chopstick_Status[(id + 0) % 5] = 0; // left chopstick at index id
    chopstick_Status[(id + 1) % 5] = 0; // right chopstick at index id+1
    m.release();

    // Release the two chopsticks / Signal that the two chopsticks are free
    s_c[id]->release();
    s_c[(id + 1) % 5]->release();

    max_philosophers.release();
  }
}

/* =============== MAIN =============== */

int main()
{
  cout << "\nMAIN: START\n";

  /* ========== DEMO 0: 5 Philosophers (no synchronization) ========== */
  cout << "\n===== DEMO 0: START =====\n";
  cout << "INFO: 5 Philosophers (No Synchronization) \n";
  cout << "Execution Sequence:\n";
  thread P0(philosophize, 0);
  thread P1(philosophize, 1);
  thread P2(philosophize, 2);
  thread P3(philosophize, 3);
  thread P4(philosophize, 4);
  P0.join();
  P1.join();
  P2.join();
  P3.join();
  P4.join();
  cout << "===== DEMO 0:END =====\n";

  /* ========== DEMO 1: 5 Philosophers (with synchronization) ========== */
  cout << "\n===== DEMO 1: START =====\n";
  cout << "INFO: 5 Philosophers (with Synchronization) \n";
  cout << "Execution Sequence:\n";
  thread _P0(_philosophize, 0);
  thread _P1(_philosophize, 1);
  thread _P2(_philosophize, 2);
  thread _P3(_philosophize, 3);
  thread _P4(_philosophize, 4);
  _P0.join();
  _P1.join();
  _P2.join();
  _P3.join();
  _P4.join();
  cout << "===== DEMO 1:END =====\n";

  cout << "\nMAIN: END\n";

  return 0;
}
