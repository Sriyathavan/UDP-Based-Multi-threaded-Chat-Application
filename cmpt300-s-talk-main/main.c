#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "keyboardThread.h"
#include "senderThread.h"
#include "receiverThread.h"
#include "screenThread.h"
#include "list.h"

List* senderList;
List* receiverList;

//mutex shared across all threads (both lists call from a fixed pool)
pthread_mutex_t mutex;

//condition variables
pthread_cond_t senderEmpty;


pthread_cond_t full;

void freeMessage(void* message) {
    free(message);
}

int main (int argc, char** argv) {
    running = true;

    if (argc != 4) {
        printf("User provided too many or too little arguments");
        return 1;
    }
  
    myPortNum = argv[1];
    destName = argv[2];
    destPortNum = argv[3];

    printf("Starting s-talk...\nPress ! to end s-talk\n\n");

    startProgram();
    while(running);
}

void startProgram() {
    // create two lists for the threads
    senderList = List_create();
    receiverList = List_create();

    // create mutex and condition variables for the threads
    //NULL for default attributes
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_init(&full, NULL);
    pthread_cond_init(&receiverEmpty, NULL);
    pthread_cond_init(&senderEmpty, NULL);
    
    // start the threads, mutexes will be accessed by all
    //NOTE: list is already in pointer form, cond vars are not
    senderInit(senderList, &senderEmpty);
    receiverInit(receiverList, &receiverEmpty);
    screenInit(receiverList, &receiverEmpty);

    keyboardInit(senderList, &senderEmpty); //do this before the input lock happens
}

void endProgram() {
    running = false;
    // free lists
    List_free(senderList, freeMessage);
    List_free(receiverList, freeMessage);

    receiverTerminate();
    screenTerminate();
    senderTerminate();
    keyboardTerminate();

    // destroy mutex
    pthread_mutex_destroy(&mutex);

    //destroy condition vars
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&senderEmpty);
    pthread_cond_destroy(&receiverEmpty);

    pthread_exit(NULL);
}

void printList (List* list) {
    char* message = List_first(list);
    while (message != NULL) {
        printf("[ %s ] --> ", message);
        message = List_next(list);
    }

    printf("\n");
}

//NOTE: use prepend and trim for a queue (prepend adds to front, trim takes from back)
void addMessage(char* message, List* list, pthread_cond_t* empty) {
    pthread_mutex_lock(&mutex); //lock mutex

    //List_prepend returns -1 on failure (no free nodes -> full)
    //while (full)
    //NOTE: when cond_wait ends, we get the mutex again, so list_prepend is safe
    while(List_prepend(list, (void*) message) == -1) {
        //we will wait until a message is taken
        //cond_wait gives up the mutex as well
        pthread_cond_wait(&full, &mutex); //NOTE: mutex is not in pointer form
        if(!running) {
            return;
        }
    } //gets mutex when coming out

    //produce:
    //message has already been written to in while loop

    //in case a consumer is waiting, we let them know
    pthread_cond_signal(empty);
    pthread_mutex_unlock(&mutex); //give up the mutex again
}

char* takeMessage(List* list, pthread_cond_t* empty, bool screen) {
    char* message; //to return
    pthread_mutex_lock(&mutex);

    while(List_count(list) == 0) {
        pthread_cond_wait(empty, &mutex);

        if(!running) {
            return NULL;
        }
    }

    //consume:
    message = (char*) List_trim(list); //gives void pointer

    //in case producer is waiting
    pthread_cond_broadcast(&full); //unblocks all, both producers will fight for mutex
    pthread_mutex_unlock(&mutex); //give up mutex

    return message;
}