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


int   SharedValue   = 0;
int   SharedValue1  = 0;
int   SharedValue2  = 0;
int   threadCount   = 10000;

bool* goingCritical = new bool[threadCount];
int   turn          = 0; 

mutex svLock1;
mutex svLock2;

/* =============== WriteOnShared_0 =============== */


void WriteOnShared_0( int threadID )
{
  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  SharedValue++;
  
  cout << thName + ": END\n";
}


/* =============== WriteOnShared_1 =============== */


void WriteOnShared_1( int threadID )
{
  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  // for loop block - increase function/thread life time
  int x = 0;
  for ( int i = 0; i < 100*1000; i++ )
  {
    x += 4324234234 * i;
    x  = x % 423423432;
  }

  SharedValue++;
  
  cout << thName + ": END\n";
}


/* =============== WriteOnShared_2 =============== */


void WriteOnShared_2( int threadID )
{
  string thName = "WoS2: T-" + to_string(threadID);

  cout << thName + ": START\n";

  // for loop - increases no of access to SharedValue
  for ( int i = 0; i < 10*1000; i++ )
  {
    SharedValue++;
    SharedValue--;
  }
  
  cout << thName + ": END\n";
}


/* =============== WriteOnShared_3a: With Peterson's Solution =============== */


void WriteOnShared_3a( int threadID ) {

  string thName = "T-" + to_string(threadID);

  cout << thName + ": START\n";

  int otherID = 1 - threadID;

  /* ===== PRE-CRITICAL: ENTRY SECTION ===== */
  goingCritical[threadID] = true;
  turn = otherID;

  while ( goingCritical[otherID] and 
          turn != threadID ) {
    // busy waiting 
  }

  /* ===== CRITICAL SECTION ===== */
  cout << thName + ": IN CRITICAL\n";
  for ( int i = 0; i < 10*1000; i++ ) {
    SharedValue++;
    SharedValue--;
  }
  
  /* ===== POST-CRITICAL: EXIT SECTION ===== */
  goingCritical[threadID] = false;


  /* ===== REMAINDER SECTION ===== */
  cout << thName + ": END" << endl;
}


/* =============== WriteOnShared_3b: With Peterson's Solution =============== */


void WriteOnShared_3b( int threadID ) {

  string thName = "T-" + to_string(threadID);

  cout << thName + ": START.\n";

  int othersID = 1 - threadID;

  /* ===== PRE-CRITICAL: ENTRY SECTION ===== */
  goingCritical[threadID] = true;
  turn = othersID;

  bool firstWait = true;
  while ( goingCritical[othersID] and turn != threadID ) {
    if ( firstWait ) {
      cout << thName + ": WAITING.\n" ;
      firstWait = false;
    }
    //busy waiting 
  }

  /* ===== CRITICAL SECTION ===== */
  cout << thName + ": IN CRITICAL.\n";
  for ( int i = 0; i < 1000*1000; i++ ) {
    SharedValue++;
    SharedValue--;
  }
  
  /* ===== POST-CRITICAL: EXIT SECTION ===== */
  goingCritical[threadID] = false;

  /* ===== REMAINDER SECTION ===== */
  cout << thName + ": END." << endl;
}


/* =============== WriteOnShared_4a =============== */

void WriteOnShared_4a( int threadID )
{
  string thName = "WoS4a: T-" + to_string(threadID);

  cout << thName + ": START\n";

  int x = 0; // internal (non-shared) variable
  int loopCount1 = 10;
  int loopCount2 = 1 * 1000;
  int loopCount3 = 1 * 1000;

  for ( int k = 0; k < loopCount1; k++ ) {
    /* ===== NON-CRITICAL SECTION ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      x += 4324234234 * j;
      x  = x % 423423432;
    }

    /* ===== CRITICAL SECTION ===== */
    for ( int i = 0; i < loopCount2; i++ ) {
      SharedValue1++;
      SharedValue1--;
    }
    
    /* ===== NON-CRITICAL SECTION ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      x += 4324234234 * j;
      x  = x % 423423432;
    }
  }
  
  cout << thName + ": END\n";
}



/* =============== WriteOnShared_4b =============== */

void WriteOnShared_4b( int threadID )
{
  string thName = "WoS4b: T-" + to_string(threadID);

  cout << thName + ": START\n";

  int x = 0; // internal (non-shared) variable
  int loopCount1 = 10;
  int loopCount2 = 1 * 1000;
  int loopCount3 = 1 * 1000;

  for ( int k = 0; k < loopCount1; k++ ) {
    /* ===== NON-CRITICAL SECTION ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      x += 4324234234 * j;
      x  = x % 423423432;
    }

    svLock1.lock(); // PRE-CRITICAL: ENTRY SECTION
    /* ===== CRITICAL SECTION ===== */
    for ( int i = 0; i < loopCount2; i++ ) {
      SharedValue1++;
      SharedValue1--;
    }
    svLock1.unlock(); //POST-CRITICAL: EXIT SECTION
    
    /* ===== NON-CRITICAL SECTION ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      x += 4324234234 * j;
      x  = x % 423423432;
    }
  }
  
  cout << thName + ": END\n";
}


/* =============== WriteOnShared_4c =============== */

void WriteOnShared_4c( int threadID )
{
  string thName = "WoS4c: T-" + to_string(threadID);

  cout << thName + ": START\n";

  int x = 0; // internal (non-shared) variable
  int loopCount1 = 10;
  int loopCount2 = 1 * 1000;
  int loopCount3 = 1 * 1000;

  for ( int k = 0; k < loopCount1; k++ ) {
    /* ===== NON-CRITICAL SECTION ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      x += 4324234234 * j;
      x  = x % 423423432;
    }

    svLock1.lock(); // PRE-CRITICAL: ENTRY SECTION
    /* ===== CRITICAL SECTION ===== */
    for ( int i = 0; i < loopCount2; i++ ) {
      SharedValue1++;
      SharedValue1--;
    }
    svLock1.unlock(); //POST-CRITICAL: EXIT SECTION
    
    /* ===== CRITICAL SECTION 2 ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      SharedValue2++;
      SharedValue2--;
    }
  }
  
  cout << thName + ": END\n";
}


/* =============== WriteOnShared_4d =============== */

void WriteOnShared_4d( int threadID )
{
  string thName = "WoS4d: T-" + to_string(threadID);

  cout << thName + ": START\n";

  int x = 0; // internal (non-shared) variable
  int loopCount1 = 10;
  int loopCount2 = 1 * 1000;
  int loopCount3 = 1 * 1000;

  for ( int k = 0; k < loopCount1; k++ ) {
    /* ===== NON-CRITICAL SECTION ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      x += 4324234234 * j;
      x  = x % 423423432;
    }

    svLock1.lock(); // PRE-CRITICAL: ENTRY SECTION
    /* ===== CRITICAL SECTION ===== */
    for ( int i = 0; i < loopCount2; i++ ) {
      SharedValue1++;
      SharedValue1--;

    }
    svLock1.unlock(); //POST-CRITICAL: EXIT SECTION
    
    svLock2.lock(); // PRE-CRITICAL: ENTRY SECTION
    /* ===== CRITICAL SECTION 2 ===== */
    for ( int j = 0; j < loopCount3; j++ ) {
      SharedValue2++;
      SharedValue2--;
    }
    svLock2.unlock(); //POST-CRITICAL: EXIT SECTION
  }
  
  cout << thName + ": END\n";
}

counting_semaphore sema1(0);              // (1)

void TA() {
  int i = 0;
  for ( ; i < 5; i++ ) {
    cout << "A1\n";
  }

  cout << "A2-----A\n";
  sema1.release();

  for ( i = 0 ; i < 5; i++ ) {
    cout << "A3\n";
  }
}

void TB() {
  int i = 0;
  for ( ; i < 5; i++ ) {
    cout << "B1\n";
  }

  sema1.acquire();
  cout << "B2-----B\n";

  for ( i = 0 ; i < 5; i++ ) {
    cout << "B3\n";
  }
}


int buffSize = 5; // usable capacity = buffsize -1
int buffer[5];   // usable capacity == buffsize -1
int front = 0;
int back  = 0;

counting_semaphore sema2(1);

void produce(string tName) {
  srand((unsigned) time(NULL)); 
  int randNo = 0;


  /* ===== CRITICAL ===== */
  for ( int i = 0; i < 5; i++ ) {
    //cout << "produce: front: " + to_string(front) + " back: " + to_string(back) + "\n"; 
    sema2.acquire();
    if ( front == (back + 1) % buffSize ) {
      cout << tName + ": produce: FULL\n";
    }
    else  {
      randNo = rand() % 100;
      cout << tName +": produce: " + to_string(randNo) + "\n";
      buffer[back] = randNo;
      back = (back + 1) % buffSize;
    }
    sema2.release();
    //usleep(50*1000);
  }


}

void consume(string tName) {

  /* ===== CRITICAL ===== */
  for (int j = 0; j < 5; j++ ) {
    //cout << "consume: front: " + to_string(front) + " back: " + to_string(back) + "\n"; 
    sema2.acquire();
    if ( front == back ) {
      cout << tName + ": consume: EMPTY\n";
    }
    else {
      cout << tName  + ": consume: " + to_string(buffer[front]) + "\n";
      front = (front + 1) % buffSize; 
    }
    sema2.release();
    //usleep(100*1000);
  }
}

/* =============== MAIN =============== */


int main()
{
  cout << "MAIN: START\n"; 

  /* ===== DEMO 1: Simple Synchronization ===== 
  thread tb(TB);
  thread ta(TA);

  ta.join();
  tb.join(); */

  /* ===== DEMO 2: Producer-Consumer ===== */
  thread tprod1(produce, "P1");
  //thread tprod2(produce, "P2");
  thread tcons1(consume, "C1");
  //thread tcons2(consume, "C2");
  tprod1.join();
  //tprod2.join();
  tcons1.join();
  //tcons2.join();


  /* ===== Sequential Version ===== 
  for ( int i = 0; i < threadCount; i++ ){
    WriteOnShared_4(-1); // ThreadID = -1 for MAIN
  } */


  /* ===== Multithreaded Version ===== 
  vector<thread> myThreads;
  for ( int i = 0; i < threadCount; i++ ) {
    if ( i % 2 == 0) {
      myThreads.emplace_back(WriteOnShared_4d, i);
      continue;
    } 
    myThreads.emplace_back(WriteOnShared_4b, i);
  }

  for( auto& thr : myThreads ) {
    thr.join();
  }*/

  //cout << "MAIN: SharedValue1: " + to_string(SharedValue1) + "\n"; 
  //cout << "MAIN: SharedValue2: " + to_string(SharedValue2) + "\n"; 
  cout << "MAIN: END\n"; 

  return 0;
}




