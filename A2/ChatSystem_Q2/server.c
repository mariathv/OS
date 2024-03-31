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
void lightRed() {
    printf("\033[1;91m");
}

void blue() {
    printf("\033[1;34m");
}
int main() {
    
    
    printf("\033[1;97m> enter the number of \033[1;94mclients\033[1;97m:: ");
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
    printf("> \033[1;97mshared memory key :: \033[1;35m%d\n\n", shmID);
    reset();
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
            printf("\033[1;94mclient\033[0m process %d attached at \033[1;35m%d\n", i+1, shmID); reset();
            printf("\033[1;94mclient %d \033[1;32mconnected. \033[1;35m(pid. %d)\n", i+1, getpid()); reset();
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
    for(int i=0 ; i<cls; i++){
        currgcs[i]=0;
    }

    for(int i=0; i<cls; i++){
        for(int j=0; j<10; j++)
        for(int k=0; k<50; k++)
            grps[i][j].clientIDs[k]=-1;
    }
    

    // Initialize message struct
    for(int i = 0; i < MAX_CLIENTS; i++) {
        for(int j = 0; j < MAX_CLIENTS; j++) {
            msges.isNull[i][j] = true;
            msges.messages[i][j][0] = '\0'; // Initialize as empty strings
        }
    }
    
    printf("\033[1;97m_SERVER STARTED_\n"); reset();
    
    // Continuously check for new messages
    while (1) {
        //check flags in the shared memory and perform requests from client accordingly
        SharedData *serverData = shmat(shmID, NULL, 0);
        if (serverData == (void *) -1) {
            perror("shmat");
            exit(EXIT_FAILURE);
        }
        
        // Check if a message is available
        if (serverData->flag == true) {
            printf("\033[0m---\n");
            printf("\033[1;94mclient %d \033[0msends a \033[1;97m_private_\033[0m message to \033[1;94mclient %d\n", serverData->from, serverData->to);
            printf("\033[1;94mclient %d \033[0mto \033[1;94mclient %d: \033[1;92m %s\n", serverData->from, serverData->to, serverData->message);
            printf("\033[0m---\n");
            
            // Update messages struct with the new message
            int from = serverData->from;
            int to = serverData->to;
            if (from >= 0 && from < MAX_CLIENTS && to >= 0 && to < MAX_CLIENTS) {
                char actmsg[250];
                actmsg[0] = '\0';
                sprintf(actmsg, "\033[1;97mfrom client %d: \033[1;92m", from);
                strcat(actmsg, serverData->message);
                strcat(actmsg, "\n");

                strcat(msges.messages[from][to], actmsg);
                strcat(msges.messages[to][from], actmsg);
                
                msges.isNull[from][to] = false;
                msges.isNull[to][from] = false;
               
            } else {
                printf("Error: Invalid client IDs\n");
            }

            // Reset the flag after reading the message
            serverData->flag = false;
        }
        //open chat request from client
        if(serverData->openChat == true){
            printf("\033[0m---\n");
            int from = serverData->from;
            int to = serverData->to;
            printf("open chat request between \033[1;94mclient %d \033[0mand \033[1;94mclient %d\n", to, from); reset();
            if(msges.isNull[to][from]!=true){
                printf("open chat request between \033[1;94mclient %d \033[0mand \033[1;94mclient %d successful\n", to, from); reset();
            char msgchat[500]; msgchat[0]='\0';
            strcpy(msgchat, msges.messages[to][from]);
            strcpy(serverData->fullchat, msgchat);
            }else{
                strcpy(serverData->fullchat, "failed");
                lightRed();
                printf("open chat request between \033[1;94mclient %d \033[0mand \033[1;94mclient %d failed\n", to, from); reset();
            }
            
            serverData->openChat=false;
            printf("\033[0m---\n");
        }
        //create group request from clients
        if(serverData->crtgrp == true){
            printf("\033[0m---\n");
            int reqID = serverData->from;
            reqID--;
            strcpy(grps[reqID][currgcs[reqID]].name, serverData->message);
            
            grps[reqID][currgcs[reqID]].id=currgcs[reqID];
            for(int i=0; i<serverData->currcl; i++){
                grps[reqID][currgcs[reqID]].clientIDs[i]=serverData->selectedcl[i];
            }
            grps[reqID][currgcs[reqID]].currclients=serverData->currcl;
            printf("added clients::\n");
            printf("\033[1;94m");
            for(int i=0; i<serverData->currcl; i++){
                printf("client %d\n",grps[reqID][currgcs[reqID]].clientIDs[i]);
            }
            reset();
            currgcs[reqID]++; //CAUSES PROBLEMO
            
            printf("created group chat \033[1;35m'%s' \033[0mrequested by \033[1;94mclient %d\n",serverData->message, reqID+1);
            reset();
            serverData->crtgrp=false;
            printf("\033[0m---\n");
        }
        //just a helper function to send all the joined groups of clients
        if(serverData->grpshow==true){
            printf("\033[0m---\n");
            int reqID=serverData->from;
            reqID--;
            printf("group chat list request from \033[1;94mclient %d\n", reqID+1);
            reset(); int left=0;
            for(int i=0; i<currgcs[reqID]; i++){
                strcpy(serverData->chararrshare[i],grps[reqID][i].name);
                left=i;
            }
            bool none = true; int joinedgcs=0;
            for(int i=0; i<cls; i++){
                if(i==reqID)
                continue;
                for(int j=0; j<currgcs[i]; j++)
                for(int k=0; k<grps[i][j].currclients; k++){
                //printf("%s - %d %d %d\n",grps[i][j].name, i,j,k);
                if(grps[i][j].clientIDs[k]==reqID+1){
                    //printf("innn %s - %d %d %d\n",grps[i][j].name, i,j,k);
                    strcpy(serverData->chararrshare[left++],grps[i][j].name);
                    none=false;
                    joinedgcs++;
                    }
                }
            }
            reset();
            if(currgcs[reqID]==0 && none){
                red();
                printf("client %d group list retrieval error\n", reqID+1);
                reset();
                serverData->intpass=-1;
            }else{
            serverData->intpass=currgcs[reqID]+joinedgcs;
            }

            serverData->grpshow=false;
            printf("\033[0m---\n");
        }
        //message send to group chat request from clients
        if(serverData->grpmsgsend==true || serverData->premessagesend == true){
            printf("\033[0m---\n");
            char gcname [20];
            int reqID=serverData->from;
            reqID--;
            strcpy(gcname,serverData->message1);
            int slcgc; bool found = false; bool nothost=false;
            //checking within its own created groups
            for(int i=0; i<currgcs[reqID]; i++){
                if(strcmp(gcname,grps[reqID][i].name)==0){
                    slcgc=i;
                    found = true;
                    break;
                }
            }
            //checking within its joined groups
            int hostID, grpID;
            if(!found){
                for(int i=0; i<cls; i++){
                    if(i==reqID)
                    continue;
                    for(int j=0; j<currgcs[i]; j++)
                    for(int k=0; k<grps[i][j].currclients; k++){
                    if(grps[i][j].clientIDs[k]==reqID+1 && strcmp(grps[i][j].name,gcname)==0){ //cmp reqID and gcname
                        found = true;nothost=true;
                        hostID=i; slcgc=j;
                        break;
                        }
                    }
                }
            }


            if(!found){
               printf("::error connecting to grp %s\n", gcname);
               serverData->failflag=true;
                
            }else{
                printf(":: connected to \033[1;92mgroup %s \033[0mrequested by \033[1;94mclient %d\n",gcname,reqID+1);
                reset();
                if(serverData->grpmsgsend){
                green();
                printf("\033[1;94mclient %d\033[0m sends a message to \033[1;92mgroup %s \n", reqID+1, gcname);
                reset();
                printf("\033[1;94mclient %d \033[0m : \033[1;92m %s\n",reqID+1,serverData->message);
                reset();
                char actmsg[250];
                actmsg[0] = '\0';



                sprintf(actmsg, "\033[1;97mfrom client %d: \033[1;92m", reqID+1);
                strcat(actmsg, serverData->message);

                strcat(actmsg, "\n");
                if(!nothost){
                    strcat(grps[reqID][slcgc].fullchat,actmsg);
                    strcpy(serverData->fullchat, grps[reqID][slcgc].fullchat);
                }else{
                    strcat(grps[hostID][slcgc].fullchat,actmsg);
                    strcpy(serverData->fullchat, grps[hostID][slcgc].fullchat);
                }
                }
            }
            
            serverData->grpmsgsend=false;
            serverData->premessagesend=false;
            printf("\033[0m---\n");
        }
        //group message chat open request from client

        if(serverData->grpmsgshow==true){
            char gcname [20];
            int reqID=serverData->from;
            reqID--;
            strcpy(gcname,serverData->message1);
            int slcgc; bool found = false; bool nothost = false;
            //search within its own
            for(int i=0; i<currgcs[reqID]; i++){
                if(strcmp(gcname,grps[reqID][i].name)==0){
                    slcgc=i;
                    found = true;
                    break;
                }
            }
            //checking within its joined groups
            int hostID, grpID;
            if(!found){
                for(int i=0; i<cls; i++){
                    if(i==reqID)
                    continue;
                    for(int j=0; j<currgcs[i]; j++)
                    for(int k=0; k<grps[i][j].currclients; k++){
                    if(grps[i][j].clientIDs[k]==reqID+1 && strcmp(grps[i][j].name,gcname)==0){ //cmp reqID and gcname
                        found = true;nothost=true;
                        hostID=i; slcgc=j;
                        break;
                        }
                    }
                }
            }

            if(!found){
                red();
                printf("::error connecting to grp %s\n", gcname);
                reset();
                serverData->failflag=true;
                
            }else{
                if(!nothost){
                printf("\033[1;92mconnected to group %s\n", grps[reqID][slcgc].name);
                reset();
                strcpy(serverData->fullchat,grps[reqID][slcgc].fullchat);
                }else{
                    printf("\033[1;92mconnected to group %s\n", grps[hostID][slcgc].name);
                    reset();
                    strcpy(serverData->fullchat,grps[hostID][slcgc].fullchat);
                }
            }
            serverData->grpmsgshow=false;
        }
        if (shmdt(serverData) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
        usleep(10000); // 10 milliseconds
    }
    
    return 0;
}
