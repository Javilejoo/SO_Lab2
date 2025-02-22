#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define SHM_NAME "/mi_memoria_123"
#define SHM_SIZE 4096

struct message {
    int index;
    char x;
};

void cleanup(int shm_fd, char* ptr, int is_creador) {
    if (ptr != NULL && ptr != MAP_FAILED) {
        munmap(ptr, SHM_SIZE);
    }
    if (shm_fd != -1) {
        close(shm_fd);
    }
    if (is_creador) {
        shm_unlink(SHM_NAME);
    }
}

int main(int argc, char** argv) {
    if (argc != 3 || !isdigit(argv[1][0])) {
        fprintf(stderr, "Uso: %s <número> <letra>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char x = argv[2][0];
    if (!isalpha(x)) {
        fprintf(stderr, "El segundo argumento debe ser una letra\n");
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int shm_fd = -1, is_creador = 0;
    char* ptr = MAP_FAILED;

    // Intentar crear la memoria compartida
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, 0666);
    if (shm_fd != -1) {
        // exito: somos el primer proceso en ejecutarse
        is_creador = 1;
        printf("[%c] Inicializando memoria compartida...\n", x);

        if (ftruncate(shm_fd, SHM_SIZE) == -1) {
            perror("ftruncate");
            cleanup(shm_fd, ptr, is_creador);
            exit(EXIT_FAILURE);
        }
    } else {
        if (errno == EEXIST) {
            //  Ya existe: somos el segundo proceso
            printf("[%c] Conectando a memoria compartida...\n", x);
            shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
            if (shm_fd == -1) {
                perror("shm_open");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("shm_open");
            exit(EXIT_FAILURE);
        }
    }

    ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        cleanup(shm_fd, ptr, is_creador);
        exit(EXIT_FAILURE);
    }
    printf("[%c] Memoria mapeada en %p\n", x, (void*)ptr);

    if (is_creador) {
        memset(ptr, '_', SHM_SIZE);  // Inicializar memoria con '_'
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        cleanup(shm_fd, ptr, is_creador);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        cleanup(shm_fd, ptr, is_creador);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {  // Proceso padre
        close(pipe_fd[0]);

        for (int i = 0; i < SHM_SIZE; ++i) {
            if (i % n == 0) {
                struct message msg = {i, x};
                if (write(pipe_fd[1], &msg, sizeof(msg)) == -1) {
                    perror("write");
                    break;
                }
            }
        }

        close(pipe_fd[1]);
        wait(NULL);

        printf("[%c] Contenido final:\n", x);
        for (int i = 0; i < 20; ++i) {
            printf("%c ", ptr[i]);
        }
        printf("\n");

    } else {  // Proceso hijo
        close(pipe_fd[1]);

        struct message msg;
        while (read(pipe_fd[0], &msg, sizeof(msg)) > 0) {
            ptr[msg.index] = msg.x;
        }

        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    }

    cleanup(shm_fd, ptr, is_creador);
    return EXIT_SUCCESS;
}

