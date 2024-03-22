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
    sharedmem->openChat=false;
    sharedmem->crtgrp=false;
    sharedmem->grpshow=false;

    green();
    printf("-> client %d interface\n", clientID);
    printf("- client process %d attached at %d\n", clientID, shmID);

    while (1) {
        blue();
        printf("\n1. Send Message (Private)\n");
        printf("2. Open Chat\n");
        printf("3. Create Group Chat\n");
        printf("4. Send Message(Group)\n");
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
                sharedmem->openChat = false;
                int to_c;
                red();
                for(int i=1; i<=maxclients; i++){
                    if(i==clientID)
                    continue;
                printf("Client %d\n", i);
                }
                bool invflag=false;
                reset();
                do{
                    invflag=false;
                    
                printf("to client : ");
                scanf("%d", &to_c);
                if(to_c==clientID){
                        printf("invalid request... try again");
                    invflag=true;
                }
                }while(invflag);
                sharedmem->to = to_c;
                sharedmem->from = clientID;
                char msgbuff[258];
                printf("\n\nmessage: \n>");
                green();
                scanf(" %[^\n]", msgbuff); 
                strcpy(sharedmem->message, msgbuff); // Copy msgbuff into sharedmem->message
                sharedmem->flag = true;
                reset();
                if (shmdt(sharedmem) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
                
                break;
            case 2:
                sharedmem = shmat(shmID, NULL, 0);
                reset();

                red();
                invflag = false;

                for(int i=1; i<=maxclients; i++){
                    if(i==clientID)
                    continue;
                printf("Client %d\n", i);
                }
                reset();
                int chtopen;
                do{
                    invflag=false;
                printf("\nopen chat with :\n");
                scanf("%d", &chtopen);
                if(chtopen == clientID || chtopen > maxclients || chtopen<=0){
                    printf("invalid request... try again\n");
                    invflag=true;
                }
                }while(invflag);

                sharedmem->from=clientID;
                sharedmem->to=chtopen;
                sharedmem->openChat=true;
                //test if it works fine without sleep

                blue();
                printf("opening chat...\n");
                sleep(1);
                reset();
                printf("chat \033[1;32m[ client %d & client %d]\n", clientID, chtopen);
                reset();
                if(strcmp(sharedmem->fullchat, "failed")!=0){
                SharedData *clientData = shmat(shmID, NULL, 0);
                green();
                printf("\n---------------------------------------\n\n");
                reset();
                printf("%s", clientData->fullchat);
                green();
                printf("\n---------------------------------------\n");
                reset();
                if (shmdt(clientData) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
                sharedmem->openChat=false;

                printf("\n> press any key to return");
                getchar();
                }else{
                    printf("[ Empty ]\n");
                }
                break;
            case 3:
                sharedmem = shmat(shmID, NULL, 0);
                blue();
                printf("group chat name : ");
                reset();
                char tmpname[20];
                scanf("%s", tmpname);
                sharedmem->from = clientID;
                strcpy(sharedmem->message, tmpname);
                red();
                for (int i = 1; i <= maxclients; i++) {
                    if(i == clientID)
                    continue;
                    printf("client %d\n",i);
                }
                reset();
                int selected[50];
                int tselected=0;
                // Prompt the user to choose clients
                int sl;
                printf("enter the client numbers you want to add (0 to finish): \n");
                do{

                    scanf("%d", &sl);
                    if(sl == 0){
                        break;
                    }
                    if(sl<=0 || sl>maxclients || sl == clientID){
                        printf("invalid request... try again\n");
                    }else{
                        bool inv=false;
                        for(int i=0; i<tselected; i++)
                        if(selected[i]==sl){
                            printf("invalid request... try again\n");
                            inv = true;
                            break;
                        }
                        if(!inv)
                        selected[tselected++]=sl;
                    }
                }while(1);

                

                for(int i=0; i<tselected; i++){
                    sharedmem->selectedcl[i]=selected[i];
                }
                sharedmem->currcl=tselected;
                sharedmem->crtgrp=true;

                
                if (shmdt(sharedmem) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
            break;
            case 4:
                sharedmem = shmat(shmID, NULL, 0);
                sharedmem->from = clientID;
                sharedmem->grpshow=true;
                if (shmdt(sharedmem) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
                printf("fetching...\n\n");
                sleep(1);
                sharedmem = shmat(shmID, NULL, 0);
                if(sharedmem->intpass==0)
                printf("no gcs joined/created.\n");
                else{
                    green();
                    for(int i=0; i<sharedmem->intpass; i++)
                    printf("%d. %s",i+1, sharedmem->chararrshare[i]);
                    reset();
                }
                printf("\n");
                
                if (shmdt(sharedmem) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
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
