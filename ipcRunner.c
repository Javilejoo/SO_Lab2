#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {  // Proceso hijo: ejecuta 'B'
        execl("./ipc", "ipc", "5", "B", NULL);
        perror("execl");
        return 1;
    } else if (pid > 0) {  // Proceso padre: ejecuta 'A'
        execl("./ipc", "ipc", "3", "A", NULL);
        perror("execl");
        return 1;
    } else {
        perror("fork");
        return 1;
    }
}
