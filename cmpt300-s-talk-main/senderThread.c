#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"
#include "senderThread.h"
#include "main.h"
#include <string.h> //includes memset/memcpy, as well as strcmp

#include <netdb.h> //addrinfo struct
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//static => internal linkage
pthread_t senderThreadPID;
static pthread_cond_t *senderEmpty;
static List* senderList;

static int socketDescriptor;
struct addrinfo *result, *current; //dedstination points to the valid struct

void* senderThreadFunc(void* arg) {
    while (running) {
        char* message;
        int messageLength; //try strlen(message) -> stops counting when seeing the null character
            
        message = takeMessage(senderList, senderEmpty, 0); //critical section
        messageLength = strlen(message);

        //send message to destination
        //param(socketDescriptor, pointer to data, data length, flags, destination struct sockaddr, size of said struct)
        //returns -1 if error
        if (sendto(socketDescriptor, message, messageLength, 0, current->ai_addr, current->ai_addrlen) == -1) {
            //ERROR
            printf("could not send message\n");
            return NULL;
        }

        //check if message is exit (works with !)
        // terminate approriate threads if user typed !
        if (messageLength == 1 && message[0] == '!') {
            printf("User typed !, terminating s-talk...\n");
                //exits program in sender
            endProgram();
            return NULL;
        }
    }

    return NULL;
}

void senderInit(List* list, pthread_cond_t* empty) {
    senderList = list;
    senderEmpty = empty;

    //socket criteria struct
    struct addrinfo sin; 
    memset(&sin, 0, sizeof sin); //sets all fields to 0
    sin.ai_family = AF_INET; //ai means address info
    sin.ai_socktype = SOCK_DGRAM;
    
    //param(des t name, des t port, pointer to socket criteria struct, pointer to pointer -> set to LL of possible address structs)
    //returns 0 on success
    int success = getaddrinfo(destName, destPortNum, &sin, &result);

    if(success != 0) {
        //fails (success == 0)
        printf("\ncould not get addrinfo\n");
        return;
    }

    //socket() creates an int that will be associated with a socket, we need to get a valid socket (not -1)
    //if -1 -> fail
    current = result;
    while (current != NULL) {
        //protocol should be 0
        socketDescriptor = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
        if (socketDescriptor == -1) {
            //invalid socket
            current = current->ai_next; //goes to the next socket (we are in a linked list of sockets)
            continue; //loops
        }

        break;
    }

    if (current == NULL) {
            //program fails (no sockets to bind to)
            printf("not sockets to get, exiting\n");
    }

    //start the sender thread
    if (pthread_create(&senderThreadPID, NULL, &senderThreadFunc, NULL) != 0) {
        printf("Could not create sender thread");
        return;
    }
}

void senderTerminate() {
    freeaddrinfo(result);
    close(socketDescriptor); //ends connection
    pthread_exit(&senderThreadPID);
}