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

using namespace std;

/* ========== DEMO 0: Thread Functions  ========== */

void _read0(string thName)
{
  // 'read' function stand-in
  for (int i = 0; i < 50; i++)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    cout << thName;
  }
}

void _write0(string thName)
{
  // 'write' function stand-in
  for (int i = 0; i < 50; i++)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    cout << thName;
  }
}

/* ========== DEMO 01: Thread Functions  ========== */

int readersNum = 0;
counting_semaphore Srw(1);
counting_semaphore Sts(1);
counting_semaphore m(1);

void _read1(string thName)
{

  Sts.acquire();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  Sts.release();

  m.acquire();
  readersNum++;
  if (readersNum == 1)
  {
    Srw.acquire();
  }
  m.release();

  // 'read' function stand-in
  for (int i = 0; i < 50; i++)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    cout << thName;
  }

  m.acquire();
  readersNum--;
  if (readersNum == 0)
  {
    Srw.release();
  }
  m.release();
}

void _write1(string thName)
{

  Sts.acquire();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  Sts.release();

  Srw.acquire();

  // 'write' function stand-in
  for (int i = 0; i < 50; i++)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    cout << thName;
  }

  Srw.release();
}

/* =============== MAIN =============== */

int main()
{
  cout << "\nMAIN: START\n";

  /* ===== DEM0 0: Read ===== */
  cout << "\n===== DEMO 0: START =====\n";
  cout << "INFO: Readers & Writers (No Synchronization)\n";
  cout << "INFO: readers - numeric characters \n";
  cout << "INFO: writers - alphabet characters\n";
  cout << "Execution Sequence:\n";
  thread R01(_read0, "1");
  thread W01(_write0, "x");
  thread R02(_read0, "2");
  thread W02(_write0, "y");
  thread R03(_read0, "3");
  thread R04(_read0, "4");
  thread R05(_read0, "5");
  R01.join();
  R02.join();
  R03.join();
  R04.join();
  R05.join();
  W01.join();
  W02.join();
  cout << "\n===== DEMO 0:END =====\n";

  /* ===== DEM0 4b: Readers-Writers Problem ===== */
  cout << "\n===== DEMO 1: START =====\n";
  cout << "INFO: Readers and Writers (with Synchronization)\n";
  cout << "INFO: readers - numeric characters \n";
  cout << "INFO: writers - alphabet characters\n";
  cout << "Execution Sequence:\n";
  thread W1(_write1, "x");
  thread R1(_read1, "1");
  thread R2(_read1, "2");
  thread W2(_write1, "y");
  thread R3(_read1, "3");
  thread R4(_read1, "4");
  thread R5(_read1, "5");
  thread R6(_read1, "6");
  thread R7(_read1, "7");
  thread R8(_read1, "8");
  thread R9(_read1, "9");
  thread R0(_read1, "0");
  W1.join();
  W2.join();
  R1.join();
  R2.join();
  R3.join();
  R4.join();
  R5.join();
  R6.join();
  R7.join();
  R8.join();
  R9.join();
  R0.join();
  cout << "\n===== DEMO 1:END =====\n";

  cout << "\nMAIN: END\n";

  return 0;
}
