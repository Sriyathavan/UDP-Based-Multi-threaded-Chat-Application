#ifndef _RECEIVER_H_
#define _RECEIVER_H_
#include <pthread.h>
#include "list.h"

void* receiverThreadFunc(void* arg); //shouldn't need this in the header
void receiverInit(List* list, pthread_cond_t* empty);
void receiverTerminate();

#endif
