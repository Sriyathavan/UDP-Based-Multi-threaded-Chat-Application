#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include "keyboardThread.h"
#include "senderThread.h"
#include "receiverThread.h"
#include "screenThread.h"
#include "main.h"

pthread_t keyboardThreadPID;
static List* senderList;

static pthread_cond_t* senderEmpty;

#define MAX_MSG_LEN 1024

void* keyboardThreadFunc(void* arg) {
    while(running) {
        // allocate memory for input
        char* input = (char*) malloc (MAX_MSG_LEN * sizeof(char));
        if (input == NULL) {
            return NULL;
        }
        // read input from keyboard

        //check if threads can access main thread info after it ends
        printf("Enter a message: \n");

        if (fgets(input, MAX_MSG_LEN, stdin) == NULL) {
            perror("Error reading input\n");
            endProgram();
            return NULL; //safety
        }

        // replace newline char with null terminating char
        int length = strlen(input)-1;
        input[length] = '\0';

        addMessage(input, senderList, senderEmpty); //critical section
    }
    
    return NULL;
}

void keyboardInit(List* list, pthread_cond_t* empty) {
    senderList = list;
    senderEmpty = empty;

    if (pthread_create(&keyboardThreadPID, NULL, keyboardThreadFunc, NULL) != 0) {
        perror("Could not create keyboard thread");
        exit(1);
    }
}

void keyboardTerminate()
{
    pthread_exit(&keyboardThreadPID);
} 