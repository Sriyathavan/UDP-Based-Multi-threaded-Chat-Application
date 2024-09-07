#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include "list.h"
#include <pthread.h>

void* keyboardThreadFunc(void* arg);
void keyboardInit(List* list, pthread_cond_t* empty);
void keyboardTerminate();

#endif