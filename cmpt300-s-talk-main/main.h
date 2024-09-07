#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "keyboardThread.h"
#include "senderThread.h"
#include "receiverThread.h"
#include "screenThread.h"
#include "list.h"

char* myPortNum;
char* destName;
char* destPortNum;

void endProgram(); //will be called from either sender (send exit code and checks and ends program), or receiver (recieves exit code)
void startProgram();
void destroyThreads();

void printList(List* list);

pthread_cond_t receiverEmpty;

bool running;

//maybe have the mutex stuff here
void addMessage(char* message, List* list, pthread_cond_t* empty);
char* takeMessage(List* list, pthread_cond_t* empty, bool screen);