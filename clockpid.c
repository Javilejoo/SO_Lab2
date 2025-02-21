#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2, pid3;
    clock_t clock1, clock2;

    clock1 = clock();
    pid1 = fork();  

    if (pid1 == 0) {  // Proceso hijo
        pid2 = fork();  

        if (pid2 == 0) {  // Proceso nieto
            pid3 = fork();  

            if (pid3 == 0) {  // Proceso bisnieto
                for (long i = 0; i < 1000000; i++) {
                    printf("Bisnieto PID %d, i = %ld\n", getpid(), i);
                }
                printf("Bisnieto PID %d ha terminado\n", getpid());
                return 0;
            }
            
            for (long i = 0; i < 1000000; i++) {
                printf("Nieto PID %d, i = %ld\n", getpid(), i);
            }
            wait(NULL); // Esperar al bisnieto
            printf("Nieto PID %d ha terminado\n", getpid());
            return 0;
        }
        
        for (long i = 0; i < 1000000; i++) {
            printf("Hijo PID %d, i = %ld\n", getpid(), i);
        }
        wait(NULL); // Esperar al nieto
        printf("Hijo PID %d ha terminado\n", getpid());
        return 0;
    }
    
    while (wait(NULL) > 0); 
    clock2 = clock();  
    printf("Tiempo total: %lf segundos\n", (double)(clock2 - clock1) / CLOCKS_PER_SEC);
    printf("Proceso padre ha terminado.\n");
    return 0;
}

