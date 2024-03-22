#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include "structs_funcs.h"

#define MAX_CLIENTS 50
#define MAX_GROUPS 50 //non-dynamic programming thus taking 50 as max



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

    messages msges;
    GC grps[cls][10]; //a single client is limited to creating only 10 groups
    int currgcs[cls];

    // Initialize message struct
    for(int i = 0; i < MAX_CLIENTS; i++) {
        for(int j = 0; j < MAX_CLIENTS; j++) {
            msges.isNull[i][j] = true;
            msges.messages[i][j][0] = '\0'; // Initialize as empty strings
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
            printf("\033[0m---\n");
            printf("\033[1;34mclient %d \033[0msends a _private_ message to \033[1;34mclient %d\n", serverData->from, serverData->to);
            printf("\033[1;34mclient %d \033[0mto \033[1;34mclient %d: \033[1;32m %s\n", serverData->from, serverData->to, serverData->message);
            printf("\033[0m---\n");
            
            // Update messages struct with the new message
            int from = serverData->from;
            int to = serverData->to;
            if (from >= 0 && from < MAX_CLIENTS && to >= 0 && to < MAX_CLIENTS) {
                char actmsg[250];
                actmsg[0] = '\0';
                sprintf(actmsg, "from client %d: ", from);
                strcat(actmsg, serverData->message);
                strcat(actmsg, "\n");
                // Concatenate the new message with the existing ones
                strcat(msges.messages[from][to], actmsg);
                strcat(msges.messages[to][from], actmsg);
                // Mark the message as non-null
                msges.isNull[from][to] = false;
                msges.isNull[to][from] = false;
                // Display the chat
               
            } else {
                printf("Error: Invalid client IDs\n");
            }

            // Reset the flag after reading the message
            serverData->flag = false;
        }

        if(serverData->openChat == true){
            int from = serverData->from;
            int to = serverData->to;
            if(msges.isNull[to][from]!=true){
            printf("opening chat between client %d and client %d\n", to, from);
            char msgchat[500]; msgchat[0]='\0';
            strcpy(msgchat, msges.messages[to][from]);
            strcpy(serverData->fullchat, msgchat);
            }else{
                strcpy(serverData->fullchat, "failed");
                printf("request of opening chat between client %d and client %d failed\n", to, from);
            }
            serverData->openChat=false;
        }

        if(serverData->crtgrp == true){
            int reqID = serverData->from;
            strcpy(grps[reqID][currgcs[reqID]].name, serverData->message);
            grps[reqID][currgcs[reqID]].id=currgcs[reqID];
            for(int i=0; i<serverData->currcl; i++){
                grps[reqID][currgcs[reqID]].clientIDs[i]=serverData->selectedcl[i];
            }
            grps[reqID][currgcs[reqID]].currclients=serverData->currcl;
            printf("added clients::\n");
                green();
                for(int i=0; i<serverData->currcl; i++){
                    printf("client %d\n",grps[reqID][currgcs[reqID]].clientIDs[i]);
                }
                reset();
            currgcs[reqID]++;
            printf("> created group chat '%s' requested by \033[1;31mclient %d\n",serverData->message, reqID);
            reset();
            serverData->crtgrp=false;
        }
        if (shmdt(serverData) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
        usleep(10000); // 10 milliseconds
    }
    
    return 0;
}
