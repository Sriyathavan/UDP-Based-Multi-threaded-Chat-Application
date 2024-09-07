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

pthread_t screenThreadPID;
static List* receiverList;
// static pthread_cond_t *receiverEmpty;

void* screenThreadFunc(void* arg) {
    char* output;
    while(running) {
        // retrieve last item in list (earliest message)
        output = takeMessage(receiverList, &receiverEmpty, 1);

        // print message to screen
        printf("message received: %s -> Reply:\n", output);
        // if (write(1, output, strlen(output)) == -1) {
        //     printf("can't write to screen\n");
        //     endProgram();
        //     return NULL;
        // }
    }

    return NULL;
}

void screenInit(List* list, pthread_cond_t* empty) {
    receiverList = list;
    // receiverEmpty = empty;

    if (pthread_create(&screenThreadPID, NULL, screenThreadFunc, NULL) != 0) {
        perror("Could not create screen thread");
        endProgram();
        //return;
    }
}

void screenTerminate()
{
    pthread_exit(&screenThreadPID);
}