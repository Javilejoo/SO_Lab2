#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
  pid_t p = fork();
  
  if (p<0){
    perror("fork fail");
    exit(1);
  }
    // si devuelve 0 es hijo
  else if (p==0){
    for (int i = 1; i<= 4000000; i++){
      printf("%d\n", i);
      if (i % 10000 == 0){
        usleep(3000);
        }
    }
    exit(0);
    }
    // si no es un valor 0 es padre
  else{
    while(1){
    }
  }
return 0;
}
