#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include "receiverThread.h"
#include "main.h"

#include <netdb.h> //addrinfo struct
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

pthread_t receiverThreadPID;
// pthread_cond_t *receiverEmpty;
static List* receiverList;
static int socketDescriptor;
// static struct addrinfo dest; //should be client (we are host)

#define MAXBUFFERLENGTH 100

void* receiverThreadFunc(void* arg) {
    struct sockaddr_in receivedFrom; //trivial, we do not use this, just a courtesy
    unsigned int addrLen = sizeof(receivedFrom);

    //buffer to fill in message
    char buffer[MAXBUFFERLENGTH];
    
    while (running) {
        //NOTE: we receive up to MAXBUFFER -1. we need to set the last char to the null character

        //receives message to destination
        //param(socketDescriptor, pointer to buffer, length - 1?, flag, struct sockaddr from, pointer to int (size of from))
        //returns -1 if error

        int numOfBytes = recvfrom(socketDescriptor, buffer, MAXBUFFERLENGTH - 1, 0, (struct sockaddr*)&receivedFrom, &addrLen);
        if (numOfBytes == -1) {
            //ERROR (END?)
            printf("error: could not receive");
        }
        
        //else: valid message
        //set up the buffer
        if (numOfBytes == MAXBUFFERLENGTH) {
            numOfBytes--; //so we don't go out of bounds when setting the null char
        }
        buffer[numOfBytes] = 0; //null char, can also just set to 0

        //if exit code -> end program
        if (!strcmp(buffer, "!")) { //CHECK
            //EXIT
            printf("Other user entered !, terminating s-talk...\n");
            endProgram();
            return NULL;
        }

        // printf("message received %s\n", buffer); //TO DO: FIX RECEIVER
        //CRITICAL SECTION:
        //send to receiverList
        addMessage(buffer, receiverList, &receiverEmpty);
    }

    return NULL;
}

void receiverInit(List* list, pthread_cond_t* empty) {
    receiverList = list;

    //socket criteria struct
    struct sockaddr_in sin; 
    memset(&sin, 0, sizeof(sin)); //sets all fields to 0
    sin.sin_family = AF_INET; //ai means address info
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(atoi(myPortNum)); //CHECK: according to beej: use my ip?

    //CHECK: is this the same for reciever?
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    if (pthread_create(&receiverThreadPID, NULL, &receiverThreadFunc, (void*) list) != 0) {
        printf("Could not create receiver thread");
        endProgram();
        return;
    }
}

void receiverTerminate() {
    close(socketDescriptor);
    pthread_exit(&receiverThreadPID);
}