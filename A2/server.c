#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include "structs_funcs.h"


void writemessage(int sender_id, int receiver_id, const char* message);

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



//add sleep later
int main(){
    red();
    printf("> enter the number of clients: ");
    int cls;
    scanf("%d", &cls);
    printf("\n");
    int clspids[cls];

    int tmp;
    SharedData *sharedmem;
    char buff [258];
    int shmID;
    
    shmID = shmget((key_t)2004, 1024,0666|IPC_CREAT);
    if(shmID == -1){
      perror("SHAREDMEM :: ");
      exit(1);
    }
    sharedmem = shmat(shmID,NULL,0);
    printf("- server process attached at %p\n", sharedmem);
    reset();
    printf("> shared memory key :: %d\n", shmID);

    for(int i=0; i<cls; i++){
        //parent will be the only one running (childs exit thru exec comd)
        pid_t pid = fork();
        if(pid == -1){
            perror("::! error creating client\n");
            exit(0);
        }
        if(pid == 0){
            //child
            blue();
            printf("- client process %d attached at %p\n",i+1, sharedmem);
            printf("client %d connected. (pid. %d)\n",i+1, getpid());
            clspids[i]=getpid();
            char arg[10]; 
            sprintf(arg, "%d", i+1);
            char clss[10]; 
            sprintf(arg, "%d", cls);
            char shmID_str[10]; 
            sprintf(shmID_str, "%d", shmID); 
            

            execlp("gnome-terminal", "gnome-terminal", "--", "./client", arg, shmID_str, clss,NULL);
            perror("::! exec failed!\n");
            exit(0);
        }
    }
    reset();
    for(int i=0 ; i<cls; i++)
    wait(NULL);

    printf("%c", 12);
    chat chats[cls][cls];

    //chat 
    for (int i = 0; i < cls; i++) {
        for (int j = 0; j < cls; j++) {
            chats[i][j].client1_id = -1;
            chats[i][j].client2_id = -1;
            chats[i][j].num_messages = 0;
        }
    }
    if (shmdt(sharedmem) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    printf("-> server interface\n");
    SharedData *serverData;
    while(1){
      //only exits with kill ctrl+c
      serverData = shmat(shmID, NULL, 0);
      if(serverData->flag==true){
        printf("message: %s", serverData->message);
      }
    }
    //strcpy(sharedmem,cls); 
    if (shmdt(sharedmem) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

// void writemessage(int sender_id, int receiver_id, const char* message) {
//     int chat_exists = 0;
//     for (int i = 0; i < MAX_CLIENTS; i++) {
//         if ((chats[i][0].client1_id == sender_id && chats[i][0].client2_id == receiver_id) ||
//             (chats[i][0].client1_id == receiver_id && chats[i][0].client2_id == sender_id)) {
//             // Chat already exists
//             chat_exists = 1;
//             snprintf(chats[i][0].messages[chats[i][0].num_messages], sizeof(chats[i][0].messages[0]), "%s", message);
//             chats[i][0].num_messages++;
//             break;
//         }
//     }
//     if (!chat_exists) {
//         // Create a new chat
//         for (int i = 0; i < MAX_CLIENTS; i++) {
//             if (chats[i][0].client1_id == -1 && chats[i][0].client2_id == -1) {
//                 chats[i][0].client1_id = sender_id;
//                 chats[i][0].client2_id = receiver_id;
//                 snprintf(chats[i][0].messages[chats[i][0].num_messages], sizeof(chats[i][0].messages[0]), "%s", message);
//                 chats[i][0].num_messages++;
//                 break;
//             }
//         }
//     }
// }
