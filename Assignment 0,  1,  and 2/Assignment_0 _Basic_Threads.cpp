// Submitted by: Darius Vincent Ardales
// STDISCM - S13
// January 24, 2025

#include <iostream>
#include <thread>
#include <string>
#include <windows.h>
using namespace std;

// ANSI color codes
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"

int loop_flag = 1;

void print_fast(string text, string color, int count)
{
  for (int i = 0; i < count - 1; i++)
  {
    cout << color << text << RESET << endl;
    Sleep(50);
  }
  cout << color << text << "<--" << RESET << endl;
}

void print_norm(string text, string color, int count)
{
  for (int i = 0; i < count - 1; i++)
  {
    cout << color << text << RESET << endl;
    Sleep(150);
  }
  cout << color << text << "<--" << RESET << endl;
}

void print_slow(string text, string color, int count)
{
  for (int i = 0; i < count - 1; i++)
  {
    cout << color << text << RESET << endl;
    Sleep(450);
  }
  cout << color << text << "<--" << RESET << endl;
}

void print_loop(string text, string color)
{
  while (loop_flag == 1)
  {
    cout << color << text << RESET << endl;
    Sleep(450);
  }
  cout << color << text << "<--" << RESET << endl;
}

void print_norm_x2(string text, string color, int count)
{
  thread thread_1(print_norm, text + "-1", color, count);
  thread thread_2(print_norm, text + "-2", color, count);
  thread_1.join();
  thread_2.join();
}

// ----------------------------- DEMOS ----------------------------- //

int main()
{
  // // ===== DEMO 00 ===== //
  // cout << endl;
  // cout << "===== DEMO 00: Sequential Execution of Fast, Normal, and Slow Printing (Function Testing) =====" << endl;

  // print_fast("AAAA", RED, 10);
  // print_norm("BBBB", BLUE, 10);
  // print_slow("CCCC", GREEN, 10);

  // // ===== DEMO 01 ===== //
  // cout << endl;
  // cout << "===== DEMO 01: Running Two Threads and Calling join() for Both Threads =====" << endl;

  // thread thread_A(print_norm, "AAAA", RED, 10);
  // thread thread_B(print_norm, "BBBB", BLUE, 10);

  // thread_A.join();
  // thread_B.join();

  // cout << YELLOW << "MAIN" << RESET << endl;

  // // ===== DEMO 02 ===== //
  // cout << endl;
  // cout << "===== DEMO 02: Running Three Threads and Calling join() for All Threads =====" << endl;

  // thread thread_A(print_norm, "AAAA", RED, 10);
  // thread thread_B(print_norm, "BBBB", BLUE, 10);
  // thread thread_C(print_norm, "CCCC", GREEN, 10);

  // thread_A.join();
  // thread_B.join();
  // thread_C.join();

  // cout << YELLOW << "MAIN" << RESET << endl;

  // // ===== DEMO 03 ===== //
  // cout << endl;
  // cout << "===== DEMO 03: Running Threads with Different Speeds (Fast, Normal, Slow) =====" << endl;

  // thread thread_A(print_fast, "AAAA", RED, 10);
  // thread thread_B(print_norm, "BBBB", BLUE, 10);
  // thread thread_C(print_slow, "CCCC", GREEN, 10);

  // thread_A.join();
  // thread_B.join();
  // thread_C.join();

  // cout << YELLOW << "MAIN" << RESET << endl;

  // // ===== DEMO 04 ===== //
  // cout << endl;
  // cout << "===== DEMO 04: Concurrent Execution of Threads with Parallel Main Thread =====" << endl;

  // thread thread_A(print_fast, "AAAA", RED, 10);
  // thread thread_B(print_norm, "BBBB", BLUE, 10);
  // thread thread_C(print_slow, "CCCC", GREEN, 10);

  // print_norm("MAIN", YELLOW, 10);

  // thread_A.join();
  // thread_B.join();
  // thread_C.join();

  // cout << YELLOW << "MAIN-end" << RESET << endl;

  // // ===== DEMO 05 ===== //
  // cout << endl;
  // cout << "===== DEMO 05: Joining Threads Sequentially After Main Thread Executes =====" << endl;

  // thread thread_A(print_fast, "AAAA", RED, 12);
  // thread thread_B(print_norm, "BBBB", BLUE, 12);
  // thread thread_C(print_slow, "CCCC", GREEN, 12);

  // print_norm("MAIN", YELLOW, 3);

  // thread_A.join();
  // print_norm("MAIN-after-A", YELLOW, 3);

  // thread_B.join();
  // print_norm("MAIN-after-B", YELLOW, 3);

  // thread_C.join();
  // print_norm("MAIN-after-C", YELLOW, 3);

  // cout << YELLOW << "MAIN-end" << RESET << endl;

  // // ===== DEMO 06 ===== //
  // cout << endl;
  // cout << "===== DEMO 06: Joining Threads in Reverse Order After Main Thread Executes =====" << endl;

  // thread thread_A(print_fast, "AAAA", RED, 12);
  // thread thread_B(print_norm, "BBBB", BLUE, 12);
  // thread thread_C(print_slow, "CCCC", GREEN, 12);

  // print_norm("MAIN", YELLOW, 3);

  // thread_C.join();
  // print_norm("MAIN-after-C", YELLOW, 3);

  // thread_B.join();
  // print_norm("MAIN-after-B", YELLOW, 3);

  // thread_A.join();
  // print_norm("MAIN-after-A", YELLOW, 3);

  // cout << YELLOW << "MAIN-end" << RESET << endl;

  // // ===== DEMO 07 ===== //
  // cout << endl;
  // cout << "===== DEMO 07: Detached Threads Running Independently from the Main Thread =====" << endl;

  // thread thread_A(print_slow, "AAAA", RED, 10);
  // thread thread_B(print_slow, "BBBB", BLUE, 10);
  // thread thread_C(print_slow, "CCCC", GREEN, 10);

  // thread_A.detach();
  // thread_B.detach();
  // thread_C.detach();
  // print_norm("MAIN", YELLOW, 10);

  // cout << YELLOW << "MAIN-end" << RESET << endl;

  // // ===== DEMO 08 ===== //
  // cout << endl;
  // cout << "===== DEMO 08: Infinite Loop Thread with Detached Execution =====" << endl;

  // thread thread_A(print_loop, "AAAA", RED);
  // thread_A.detach();

  // print_norm("MAIN", YELLOW, 20);

  // cout << YELLOW << "MAIN-end" << RESET << endl;

  // // ===== DEMO 09 ===== //
  // cout << endl;
  // cout << "===== DEMO 09: Infinite Loop Thread Controlled by User Input (loop_flag) =====" << endl;

  // thread thread_A(print_loop, "AAAA", RED);
  // thread_A.detach();

  // print_norm("MAIN", YELLOW, 20);
  // cout << YELLOW << "Enter loop_flag value: " << RESET << endl;
  // cin >> loop_flag;
  // cout << YELLOW << "New loop_flag value: " << loop_flag << RESET << endl;

  // print_slow("MAIN", YELLOW, 20);

  // cout << YELLOW << "MAIN-end" << RESET << endl;

  // ===== DEMO 10 ===== //
  cout << endl;
  cout << "===== DEMO 10: Running Two Subthreads Concurrently Within a Thread =====" << endl;

  thread thread_A(print_norm_x2, "AAAA", RED, 10);

  print_norm("MAIN", YELLOW, 20);
  thread_A.join();

  cout << YELLOW << "MAIN-end" << RESET << endl;

  // ===== END ===== //

  cout << endl;

  return 0;
}

// Thanks for watching! :D