#ifndef CHAT_H
#define CHAT_H
#include <stdbool.h>

#define MAX_CHATS 200
#define MAX_CLIENTS 50

//using graph approach

typedef struct {
    char messages[MAX_CLIENTS][MAX_CLIENTS][256]; // Assuming maximum message length of 256 characters
    int num_messages[MAX_CLIENTS][MAX_CLIENTS];   // Number of messages between clients
    
} MessageGraph;

typedef struct shared {
    bool flag; // to check if send message or nah
    int to;
    int from;
    char message[256];
    bool openChat;
    bool crtgrp;
    bool grpshow;
    char fullchat[1000];
    int selectedcl[50];
    char chararrshare[10][20];
    int currcl;
    int intpass;
} SharedData;

typedef struct {
    bool isNull[MAX_CLIENTS][MAX_CLIENTS];
    char messages[MAX_CLIENTS][MAX_CLIENTS][250];
} messages;


typedef struct chat {
    int client1_id;
    int client2_id;
    char messages[MAX_CHATS][256]; // Array to store messages in the chat
    int num_messages; // Number of messages in the chat
}chat;

typedef struct{
    char name[20];
    int id;
    int clientIDs[50]; //50 limits max clients that can be added;
    int currclients;
    //can add another array to ensure client exists or nah
    char fullchat [1000];

}GC;





#endif
