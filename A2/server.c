#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include "structs_funcs.h"

void red() {
    printf("\033[1;31m");
}

void reset() {
    printf("\033[0m");
}

void blue() {
    printf("\033[1;34m");
}

int main() {
    red();
    printf("> enter the number of clients: ");
    int cls;
    scanf("%d", &cls);
    printf("\n");
    int clspids[cls];

    int shmID;
    
    shmID = shmget((key_t)2004, 1024, 0666 | IPC_CREAT);
    if (shmID == -1) {
        perror("SHAREDMEM :: ");
        exit(1);
    }
    reset();
    printf("> shared memory key :: %d\n", shmID);

    for (int i = 0; i < cls; i++) {
        // Parent will be the only one running (childs exit through exec command)
        pid_t pid = fork();
        if (pid == -1) {
            perror("::! error creating client\n");
            exit(0);
        }
        if (pid == 0) {
            // Child
            blue();
            printf("- client process %d attached at %d\n", i+1, shmID);
            printf("client %d connected. (pid. %d)\n", i+1, getpid());
            char arg[10]; 
            sprintf(arg, "%d", i+1);
            char clss[10]; 
            sprintf(clss, "%d", cls);
            char shmID_str[10]; 
            sprintf(shmID_str, "%d", shmID); 

            execlp("gnome-terminal", "gnome-terminal", "--", "./client", arg, shmID_str, clss, NULL);
            perror("::! exec failed!\n");
            exit(0);
        }
    }
    reset();
    for (int i = 0; i < cls; i++) {
        wait(NULL);
    }

    printf("%c", 12);

    int MAX_CLIENTS = 100; // Define your maximum number of clients
    chat chats[cls][cls];

    // Chat initialization
    for (int i = 0; i < cls; i++) {
        for (int j = 0; j < cls; j++) {
            chats[i][j].client1_id = -1;
            chats[i][j].client2_id = -1;
            chats[i][j].num_messages = 0;
        }
    }

    printf("-> server interface\n");
    
    // Continuously check for new messages
    while (1) {
        // Attach to shared memory
        SharedData *serverData = shmat(shmID, NULL, 0);
        if (serverData == (void *) -1) {
            perror("shmat");
            exit(EXIT_FAILURE);
        }
        
        // Check if a message is available
        if (serverData->flag == true) {
          printf("---\n");
          printf("\033[1;34mclient %d \033[0msends a _private_ message to \033[1;34mclient %d\n", serverData->from, serverData->to);
          printf("\033[1;34mclient %d \033[0mto \033[1;34mclient %d: \033[1;32m %s\n", serverData->from, serverData->to, serverData->message);
          printf("---\n");
            // Reset the flag after reading the message
            serverData->flag = false;
        }
        
        // Detach from shared memory
        if (shmdt(serverData) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
        
        // Introduce a small delay to avoid busy-waiting
        usleep(10000); // 10 milliseconds
    }
    
    return 0;
}
