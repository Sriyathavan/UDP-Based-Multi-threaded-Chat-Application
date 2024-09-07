#ifndef _SENDER_H_
#define _SENDER_H_
#include "list.h"
#include <pthread.h>

void* senderThreadFunc(void* arg);
void senderInit(List* list, pthread_cond_t* empty);
void senderTerminate();

#endif