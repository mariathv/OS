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
void lightRed() {
    printf("\033[1;91m");
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

    //initializing all the flags in shared memory
    sharedmem->flag = false; 
    sharedmem->openChat=false;
    sharedmem->crtgrp=false;
    sharedmem->grpshow=false;
    sharedmem->grpmsgsend=false;
    sharedmem->grpmsgshow=false;
    sharedmem->failflag=false;
    sharedmem->premessagesend=false;


    while (1) {
        //clearConsole();
        system("clear");
         
        green();
        printf("-> client %d interface\n", clientID);
        printf("\n\033[1;97m1. \033[1;34mSend Message (Private)\n");
        printf("\033[1;97m2. \033[1;34mOpen Chat (Private)\n");
        printf("\033[1;97m3. \033[1;34mCreate Group Chat\n");
        printf("\033[1;97m4. \033[1;34mSend Message(Group)\n");
        printf("\033[1;97m5. \033[1;34mOpen Chat (GC)\n");
        printf("\033[1;97m0. \033[1;34mExit\n");
        reset();
        scanf("%d", &ch);
        
        blue();
        if (ch == 0) {
            break; // Exit the loop and terminate the client
        }
        switch (ch) {
            case 1:
                if(maxclients==1){
                    lightRed();
                    printf("invalid request (clients size error)\n");
                    break;
                }
                SharedData *sharedmem = shmat(shmID, NULL, 0);
                sharedmem->openChat = false;
                int to_c;
                printf("\033[1;94m");
                for(int i=1; i<=maxclients; i++){
                    if(i==clientID)
                    continue;
                printf("client %d\n", i);
                }
                bool invflag=false;
                reset();
                do{
                    invflag=false;
                    
                printf("to client (id): ");
                scanf("%d", &to_c);
                    lightRed();
                if(to_c==clientID || to_c<=0 || to_c>maxclients){
                        printf("invalid request... try again\n");
                    invflag=true;
                }
                    reset();
                }while(invflag);
                sharedmem->to = to_c;
                sharedmem->from = clientID;
                char msgbuff[258];
                printf("\n\nmessage: \n> \033[1;92m");
                scanf(" %[^\n]", msgbuff); 
                reset();
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
                if(maxclients==1){
                        lightRed();
                        printf("invalid request (clients size error)\n");
                        break;
                }
                sharedmem = shmat(shmID, NULL, 0);
                reset();

                printf("\033[1;94m");
                invflag = false;

                for(int i=1; i<=maxclients; i++){
                    if(i==clientID)
                    continue;
                printf("client %d\n", i);
                }
                reset();
                int chtopen;
                do{
                    invflag=false;
                printf("\nopen chat with :\n");
                scanf("%d", &chtopen);
                reset();
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
                
                printf("\n---------------------------------------\n\n");
                
                printf("%s", clientData->fullchat);
                reset();
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
                clear_input_buffer();
                getchar();

                }else{
                    printf("[ Empty ]\n");
                }
                break;
            case 3:
                sharedmem = shmat(shmID, NULL, 0);
                blue();
                printf("\033[1;97mgroup chat name : ");
                reset();
                green();
                char tmpname[20];
                scanf("%s", tmpname);
                reset();
                sharedmem->from = clientID;
                strcpy(sharedmem->message, tmpname);
                printf("\033[1;94m");
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
                printf("enter the client numbers you want to add \033[1;97m(0 to finish): \n");
                reset();
                do{

                    scanf("%d", &sl);
                    if(sl == 0){
                        break;
                    }
                    if(sl<=0 || sl>maxclients || sl == clientID){
                        lightRed();
                        printf("invalid request... try again\n");
                        reset();
                    }else{
                        bool inv=false;
                        for(int i=0; i<tselected; i++)
                        if(selected[i]==sl){
                        lightRed();
                            printf("invalid request... try again\n");
                        reset();
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
                
                printf("\033[1;97mfetching...\n\n");reset();
                sleep(1);
                sharedmem = shmat(shmID, NULL, 0);
                if(sharedmem->intpass==-1){
                    lightRed();
                printf("no gcs joined/created.\n");
                reset();
                break;
                }
                else{
                    green();
                    for(int i=0; i<sharedmem->intpass; i++)
                    printf("%d. %s\n",i+1, sharedmem->chararrshare[i]);
                    reset();
                }
                int choicegc=0;
                printf("\n\033[1;97mselect: ");reset();
                scanf("%d",&choicegc);
                if(choicegc>sharedmem->intpass || choicegc <=0){
                    lightRed();
                    printf("invalid input\n"); reset();
                    break;
                }
                printf("\n");

                // -- //

                sharedmem->from=clientID;
                strcpy(sharedmem->message1, sharedmem->chararrshare[choicegc-1]);
                //copied the name of the group (stored in shared memory by the server to message 1)
                blue();
                reset();
                sharedmem->premessagesend=true;
                printf("\033[1;97mloading...\n"); reset();
                sleep(1);
                SharedData *clientData = shmat(shmID, NULL, 0); //created another one to check for error
                

                if(clientData->failflag==true){
                    lightRed();
                    printf("invalid request :: failed.\n");
                    reset();
                    clientData->failflag=false;
                    break;

                }else{
                char msgbuffer[258];
                clear_input_buffer();
                printf("\n\nmessage: \n>");
                green();
                scanf(" %[^\n]", msgbuffer); 
                strcpy(sharedmem->message, msgbuffer); // Copy msgbuff into sharedmem->message
                sharedmem->grpmsgsend=true;
                //sharedmem->flag = true;
                reset();
                }
                if (shmdt(clientData) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
                if (shmdt(sharedmem) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
            break;
            case 5:
                sharedmem = shmat(shmID, NULL, 0);
                sharedmem->from = clientID;
                sharedmem->grpshow=true;
                if (shmdt(sharedmem) == -1) {
                     perror("shmdt");
                     printf("\n> press any key to return");
                     getchar();
                     exit(EXIT_FAILURE);
                }
                printf("\033[1;97mfetching...\n\n");reset();
                sleep(1);
                sharedmem = shmat(shmID, NULL, 0);
                if(sharedmem->intpass==-1){
                    lightRed();
                printf("no gcs joined/created.\n");
                reset();
                break;
                }
                else{
                    green();
                    for(int i=0; i<sharedmem->intpass; i++)
                    printf("%d. %s\n",i+1, sharedmem->chararrshare[i]);
                    reset();
                }
                choicegc=0;
                printf("\n\033[1;97mselect: ");reset();
                clear_input_buffer();
                scanf("%d",&choicegc);
                printf("\n");
                sharedmem->from=clientID;
                strcpy(sharedmem->message1, sharedmem->chararrshare[choicegc-1]);
                sharedmem->grpmsgshow=true;
                printf("\033[1;97mloading...\n"); reset();
                sleep(1);
                if(sharedmem->failflag==true){
                    lightRed();
                    printf("invalid request :: failed.\n");
                    reset();
                    sharedmem->failflag=false;
                    break;
                }
                blue();
                printf("opening group chat...\n");
                sleep(1);
                reset();

                clientData = shmat(shmID, NULL, 0);
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
                clear_input_buffer();
                printf("\n> press any key to return");
                getchar();
            break;
        }
    }
    if (shmdt(sharedmem) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}
