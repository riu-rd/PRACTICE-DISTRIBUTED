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

/* ========== Print Functions ========== */

void a()
{
  for (int i = 0; i < 100; i++)
  {
    cout << "a";
  }
}

void b()
{
  for (int i = 0; i < 100; i++)
  {
    cout << "b";
  }
}

void c()
{
  for (int i = 0; i < 100; i++)
  {
    cout << "c";
  }
}

void X()
{
  for (int i = 0; i < 100; i++)
  {
    cout << "X";
  }
}

void Y()
{
  for (int i = 0; i < 100; i++)
  {
    cout << "Y";
  }
}

void Z()
{
  for (int i = 0; i < 100; i++)
  {
    cout << "Z";
  }
}

/* ========== Thread Functions ========== */

void T1()
{
  a();
  b();
  c();
}

void T2()
{
  X();
  Y();
  Z();
}

/* ========== Thread Functions ========== */

// Binary Semaphore
// Initial Value: 1 (permit)
counting_semaphore S(1);

void T3()
{
  S.acquire();
  a();
  b();
  c();
  S.release();
}

void T4()
{
  S.acquire();
  X();
  Y();
  Z();
  S.release();
}
/* =============== MAIN =============== */

int main()
{
  cout << "\n=============== MAIN: START ===============\n";

  /* ===== DEMO 0: No Synchronization / No Mutual Exclusion ===== */

  cout << "\n\n===== DEMO 0: No Mutual Exclusion =====\n";
  thread thread1(T1);
  thread thread2(T2);
  thread1.join();
  thread2.join();
  cout << "\n===== DEMO 0:END =====\n\n";

  /* ===== DEMO 1: Has Mutual Exclusion ===== */

  cout << "\n===== DEMO 1: START: Has Mutual Exclusion =====\n";
  thread thread3(T3);
  thread thread4(T4);
  thread3.join();
  thread4.join();
  cout << "\n===== DEMO 1:END =====\n\n";

  cout << "\n=============== MAIN: END ===============\n";

  return 0;
}
