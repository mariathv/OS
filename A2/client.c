#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include "structs_funcs.h"

void sendmsg(int cls, SharedData *sharedmem);

void red() {
    printf("\033[1;31m");
}

void green() {
    printf("\033[1;32m");
}

void reset() {
    printf("\033[0m");
}

void blue() {
    printf("\033[1;34m");
}

int main(int argc, char *argv[]) {
   
    int clientID = atoi(argv[1]); // Convert command-line argument to integer
    int ch = -1;
    int shmID = atoi(argv[2]);
    int maxclients = atoi(argv[3]);

    // Reattach to the shared memory segment
    SharedData *sharedmem = shmat(shmID, NULL, 0);
    if (sharedmem == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    sharedmem->flag = false; // Initialize flag to false

    green();
    printf("-> client %d interface\n", clientID);
    printf("- client process %d attached at %d\n", clientID, shmID);
    blue();

    while (1) {
        printf("1. Send Message\n");
        printf("2. Open Chat\n");
        printf("3. Create Group Chat\n");
        printf("0. Exit\n");
        reset();
        scanf("%d", &ch);
        blue();
        if (ch == 0) {
            break; // Exit the loop and terminate the client
        }
        switch (ch) {
            case 1:
                SharedData *sharedmem = shmat(shmID, NULL, 0);
                int to_c;
                printf("to client (1-%d) : ", maxclients);
                scanf("%d", &to_c);
                sharedmem->to = to_c;
                sharedmem->from = clientID;
                char msgbuff[258];
                printf("\n\nmessage: \n>");
                green();
                scanf(" %[^\n]", msgbuff); 
                strcpy(sharedmem->message, msgbuff); // Copy msgbuff into sharedmem->message
                sharedmem->flag = true;
                reset();
                break;
        }
    }

    // Detach from the shared memory segment when done
    if (shmdt(sharedmem) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void sendmsg(int cls, SharedData *sharedmem) {
    
}
