#ifndef CHAT_H
#define CHAT_H
#include <stdbool.h>

#define MAX_CHATS 200

//using graph approach
typedef struct shared {
    bool flag; // to check it data exists or nah
    int to;
    char message[256];
} SharedData;

typedef struct chat {
    int client1_id;
    int client2_id;
    char messages[MAX_CHATS][256]; // Array to store messages in the chat
    int num_messages; // Number of messages in the chat
}chat;



#endif