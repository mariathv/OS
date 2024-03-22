#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include "structs_funcs.h"

void red () {
  printf("\033[1;31m");
}
void green() {
    printf("\033[1;32m");
}
void reset () {
  printf("\033[0m");
}
void blue() {
    printf("\033[1;34m");
}

void sendmsg(int cls, SharedData *sharedmem){
    int to_c;
    printf("to client (1-%d) : ", cls);
    scanf("%d", &to_c);
    sharedmem->flag= true;
    sharedmem->to=to_c;
    char msgbuff[258];
    printf("\n\nmessage: \n>");
    green();
    scanf("%258[^\n]", msgbuff); 
    strcpy(sharedmem->message, msgbuff); // Copy msgbuff into sharedmem->message
    free(sharedmem); // Free dynamically allocated memory when done
    reset();

}

int main(int argc, char *argv[]) {
   
    int clientID = atoi(argv[1]); // Convert command-line argument to integer
    int ch = -1;
    int shmID = atoi(argv[2]);
    int maxclients = atoi(argv[3]);

    // Reattach to the shared memory segment
    SharedData *sharedmem = shmat(shmID, NULL, 0);
    sharedmem->flag = false;
    if (sharedmem == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }   green();
    printf("-> client %d interface\n", clientID);
    printf("- client process %d attached at %d\n",clientID, shmID);
    blue();
    if (shmdt(sharedmem) == -1) {
                perror("shmdt");
                exit(EXIT_FAILURE);
            }
    while (1) {
        if (sharedmem == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
        }
        printf("1. Send Message\n");
        printf("2. Open Chat\n");
        printf("3. Create Group Chat\n");
        printf("0. Exit\n");
        reset();
        scanf("%d", &ch);
        blue();
        if (ch == 0) {
            exit(0);
        }
        switch(ch){
            case 1: sendmsg(maxclients, sharedmem);
            if (shmdt(sharedmem) == -1) {
                perror("shmdt");
                exit(EXIT_FAILURE);
            }
            break;
        }

    }

    
    
    return 0;
}