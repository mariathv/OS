#ifndef CHAT_H
#define CHAT_H
#include <stdbool.h>

#define MAX_CHATS 200
#define MAX_CLIENTS 50

//using graph approach



typedef struct shared {
    bool flag; // to check if send message or nah
    int to;
    int from;
    char message[256];
    bool openChat;
    bool crtgrp;
    bool grpshow;
    bool grpmsgsend;
    bool grpmsgshow;
    char fullchat[1000];
    int selectedcl[50];
    char chararrshare[10][20];
    int currcl;
    int intpass;
    char message1[256];
    bool failflag;
    bool premessagesend;
    int grpjoinedinfo [3];
} SharedData;

typedef struct {
    bool isNull[MAX_CLIENTS][MAX_CLIENTS];
    char messages[MAX_CLIENTS][MAX_CLIENTS][250];
} messages;

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

typedef struct{
    char name[20];
    int id;
    int clientIDs[50]; //50 limits max clients that can be added;
    int currclients;
    //can add another array to ensure client exists or nah
    char fullchat [1000];

}GC;
void clearConsole() {
    printf("\033[2J\033[1;1H");
}




#endif