#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"
#include <thread>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (const int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int comp(int &a, int &b, int &notDone) {
    if (a > b) {
        // swap
        int temp = a;
        a = b;
        b = temp;
        notDone = 0;
        return 1;
    }
    return 0;
}

int main (int argc, char* argv[]) {
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);
  
  //for(int i = 0; i < n; i++) {
   //     std::cout << arr[i] << std::endl;
    //}
    //std::cout << "--- End of before ---" << std::endl;

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  int notDone = 0;
  OmpLoop oloop;
  oloop.setNbThread(nbthreads);
  while (notDone == 0) {
      notDone = 1;
  oloop.parfor<int>(2, n, 2,
        [&](int& tls) -> void{ // Before
        tls = 0; // tls not used
    },
    [&](int i, int& tls) -> void{ // evens
        tls += comp(std::ref(arr[i - 1]), std::ref(arr[i]), std::ref(notDone));
    },
    [&](int& tls) -> void{ // After
        //tls = 0; // tls not used
    }
);
  
  oloop.parfor<int>(1, n, 2,
        [&](int& tls) -> void{ // Before
        tls = 0; // tls not used
    },
    [&](int i, int& tls) -> void{ // odds
        tls += comp(std::ref(arr[i - 1]), std::ref(arr[i]), std::ref(notDone));
    },
    [&](int& tls) -> void{ // After
        //tls = 0; // tls not used
    }
    );
  
  // Last check
  if (notDone == 0) {
      int correct = 0; // 1 = not correct, 0 = correct
    for (int i = 0; i < n; i++) {
        if (arr[i] != i) {
            correct = 1;
        }
    }
    if (correct == 0) {
        notDone = 1;
        break;
    }
  }
  }
  
  
  
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end-start;

  //for(int i = 0; i < n; i++) {
     //   std::cout << arr[i] << std::endl;
    //}
  checkMergeSortResult (arr, n);
  std::cerr<<elpased_seconds.count()<<std::endl;
  
  delete[] arr;

  return 0;
}
