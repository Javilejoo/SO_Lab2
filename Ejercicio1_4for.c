#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h> 

int main() {
    for (int i = 0; i < 4; i++) {
        fork(); // Cada iteración duplica los procesos
    }

    printf("PID: %d\n", getpid()); // Todos los procesos imprimen su PID

    // Esperar a que todos los procesos hijos terminen
    while (wait(NULL) > 0); 

    return 0;
}
