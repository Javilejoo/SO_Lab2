#include <stdio.h>
#include <time.h>

int main(){
  clock_t loop1, loop2;
  
  loop1 = clock();
  for (int i=0; i<1000001;i++){
  }
  for (int i=0; i<1000001;i++){
  }
  for (int i=0; i<1000001;i++){
  }
  loop2 = clock();
  
  double duration = (double)(loop1-loop2);
  printf("%f",duration/CLOCKS_PER_SEC);
  
  
  return 0;
}
