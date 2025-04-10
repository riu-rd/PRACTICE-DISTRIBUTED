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

/* ========== Print Functions ========== */

void a() {
  for (int i = 0; i < 100; i++) {
    cout << "a";
  }
}

void b() {
  for (int i = 0; i < 100; i++) {
    cout << "b";
  }
}

void X() {
  for (int i = 0; i < 100; i++) {
    cout << "X";
  }
}

void Y() {
  for (int i = 0; i < 100; i++) {
    cout << "Y";
  }
}

/* ========== DEMO 0: Thread Functions ========== */

void T1() {
  a();
  b();
}

void T2() {
  X();
  Y();
}

/* ========== DEMO 1: Thread Functions ========== */

// To be used for a() < Y()
counting_semaphore sem_aY(0);

void T3() {
  a();
  sem_aY.release();
  b();
}

void T4() {
  X();
  sem_aY.acquire();
  cout << " | ";
  Y();
}

/* ========== DEMO 2: Thread Functions ========== */

counting_semaphore sem_aY2(0); // a < Y
counting_semaphore sem_Xb2(0); // X < b

void T5() {
  a();
  sem_aY2.release();
  
  sem_Xb2.acquire();
  cout << " / ";
  b();
}

void T6() {
  X();
  sem_Xb2.release();
  
  sem_aY2.acquire();
  cout << " | ";
  Y();
}

/* ========== DEMO 3: ========== */

// 'Common Resource'
int resources[3] = {0,0,0};
int availFlag[3] = {1,1,1};

counting_semaphore sema_reso(1);

void produce(int produceCount) {
  int i = 0;
  int j = 0;
  while(j < 1000){

    sema_reso.acquire();
    cout << "P" << endl;
    resources[i]++;
    sema_reso.release();
    //usleep(50*1000);
    j++;
  }
}

void consume(int consumeCount) {
  int i = 0;
  int j = 0;
  while(j < 1000)
  {
    sema_reso.acquire();
    cout << "C" << endl;
    resources[i]--;
    sema_reso.release();
    //usleep(50*1000);
    j++;
  }  
}

/* ========== DEMO 4a: read & write  ========== */

void _read0(string thName) {
  // 'read' function stand-in
  for(int i = 0; i < 30; i++) {
    cout << thName;
  }
}

void _write0(string thName) {
  // 'write' function stand-in
  for(int i = 0; i < 30; i++) {
    cout << thName;
  }
}

/* ========== DEMO 4b: read & write  ========== */

int readersNo = 0;
counting_semaphore mutex_readWrite(1);
counting_semaphore mutex_readersNo(1);



void _read1(string thName) {

  mutex_readersNo.acquire();
  readersNo++;
  if ( readersNo == 1 ) {
    mutex_readWrite.acquire(); 
  }
  mutex_readersNo.release();


  // 'read' function stand-in
  for(int i = 0; i < 30; i++) {
    cout << thName;
  }


  mutex_readersNo.acquire();
  readersNo--;
  if ( readersNo == 0 ) {
    mutex_readWrite.release();
  }
  mutex_readersNo.release();
}


void _write1(string thName) {

  mutex_readWrite.acquire();

  // 'write' function stand-in
  for(int i = 0; i < 30; i++) {
    cout << thName;
  }
  
  mutex_readWrite.release();
}

/* =============== MAIN =============== */

int main()
{
  cout << "\nMAIN: START\n"; 

  /* ===== DEMO 0: No Thread Barrier (No Synchronization) ===== */

  #if true
  cout << "\n\n===== DEMO 0:START =====\n";
  cout << "INFO: No Thread Barriers\n\n";
  thread thread1(T1);
  thread thread2(T2);
  thread1.join();
  thread2.join();
  cout << "\n\n===== DEMO 0:END =====\n\n";
  #endif
  
  /* ===== DEMO 1: Thread Barrier Synchronization (via Semaphore) ===== */
  
  #if true
  cout << "\n===== DEMO 1:START =====\n";
  cout << "INFO: Thread Barrier (a | Y) via Semaphore\n\n";
  thread thread3(T3);
  thread thread4(T4);
  thread3.join();
  thread4.join();
  cout << "\n\n===== DEMO 1:END =====\n\n";
  #endif

  /* ===== DEMO 2: Thread Barrier Synchronization (via Semaphore) ===== */

  #if true
  cout << "\n===== DEMO 2:START =====\n";
  cout << "INFO: Thread Barriers (a | Y) and (X / b) via Semaphore\n\n";
  thread thread5(T5);
  thread thread6(T6);
  thread5.join();
  thread6.join();
  cout << "\n\n===== DEMO 2:END =====\n";
  #endif

  /* ===== DEMO 3: Producer-Consumer (No Synchronization) ===== */
  #if false
  cout << "\n===== DEMO 3:START =====\n";
  cout << "INFO: Multiplex\n\n";
  thread P1(produce, 1);
  //thread P2(produce, 1);
  //thread P3(produce, 1);
  thread C1(consume, 1);
  //thread C2(consume, 1);
  //thread C3(consume, 1);
  //thread C4(consume, 1);
  //thread C5(consume, 1);
  P1.join();  
  //P2.join();  
 // P3.join();  
  C1.join();  
  //C2.join();  
 // C3.join();  
  //C4.join();  
  //C5.join();  
  cout << "\nresources[0] = " << resources[0] << "\n";
  #endif
  
  /* ===== DEM0 4a: Reader-Writer Problem ===== */
  #if false
  cout << "\n===== DEMO 4a:START =====\n";
  cout << "INFO: readers - numeric characters \n";
  cout << "INFO: writers - alphabet characters\n\n";
  cout << "Execution Sequence:\n";
  thread R01(_read0, "1");
  thread R02(_read0, "2");
  thread W01(_write0, "x");
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
  cout << "\n\n===== DEMO 4a:END =====\n";
  #endif
  
  /* ===== DEM0 4b: Reader-Writer Problem ===== */
  #if false
  cout << "\n===== DEMO 4b:START =====\n";
  cout << "INFO: readers - numeric characters \n";
  cout << "INFO: writers - alphabet characters\n\n";
  cout << "Execution Sequence:\n";
  thread R1(_read1, "1");
  thread R2(_read1, "2");
  thread W1(_write1, "x");
  thread W2(_write1, "y");
  thread R3(_read1, "3");
  thread R4(_read1, "4");
  thread R5(_read1, "5");
  R1.join();
  R2.join();
  R3.join();
  R4.join();
  R5.join();
  W1.join();
  W2.join();
  cout << "\n\n===== DEMO 4b:END =====\n";
  #endif



  cout << "\nMAIN: END\n"; 

  return 0;
}




