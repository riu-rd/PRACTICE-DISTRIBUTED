#include <iostream>
#include <thread>
#include <string>
#include <cmath> // sqrt()
#include <cstdlib>
#include <vector>
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

/* ========== Thread Functions ========== */

void T1()
{
  a();
  b();
}

void T2()
{
  X();
  Y();
}

/* ========== DEMO 1: Thread Functions ========== */

counting_semaphore Sa(0);

void T3()
{
  a();
  Sa.release(); // This signal/release indicates call a()'s completion.
  b();
}

void T4()
{
  X();
  Sa.acquire(); // Waiting/aqcuiring Sa is a thread barrier.
  cout << " | ";
  Y();
}

/* ========== DEMO 2: Thread Functions ========== */

counting_semaphore SA(0); // a < Y
counting_semaphore SX(0); // X < b

void T5()
{
  a();
  SA.release();

  SX.acquire();
  cout << " / ";
  b();
}

void T6()
{
  X();
  SX.release();

  SA.acquire();
  cout << " | ";
  Y();
}

/* =============== MAIN =============== */

int main()
{
  cout << "\nMAIN: START\n";

  /* ===== DEMO 0: No Thread Barrier (No Synchronization) ===== */

  cout << "\n\n===== DEMO 0:START =====\n";
  cout << "INFO: No Thread Barriers\n\n";
  thread thread1(T1);
  thread thread2(T2);
  thread1.join();
  thread2.join();
  cout << "\n\n===== DEMO 0:END =====\n\n";

  /* ===== DEMO 1: Thread Barrier Synchronization (via Semaphore) ===== */

  cout << "\n===== DEMO 1:START =====\n";
  cout << "INFO: Thread Barrier (a | Y) via Semaphore\n\n";
  thread thread3(T3);
  thread thread4(T4);
  thread3.join();
  thread4.join();
  cout << "\n\n===== DEMO 1:END =====\n\n";

  /* ===== DEMO 2: Thread Barrier Synchronization (via Semaphore) ===== */

  cout << "\n===== DEMO 2:START =====\n";
  cout << "INFO: Thread Barriers (a | Y) and (X / b) via Semaphore\n\n";
  thread thread5(T5);
  thread thread6(T6);
  thread5.join();
  thread6.join();
  cout << "\n\n===== DEMO 2:END =====\n";

  cout << "\nMAIN: END\n";

  return 0;
}
