#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"
#include "senderThread.h"
#include "main.h"

#include <netdb.h> //addrinfo struct
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void* screenThreadFunc(void* arg);
void screenInit(List* list, pthread_cond_t* empty);
void screenTerminate();

#endif