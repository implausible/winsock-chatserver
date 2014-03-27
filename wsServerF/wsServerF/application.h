#ifndef APPLICATION_H_
#define APPLICATION_H_
#include "server.h"
#include <iostream>
using namespace std;

struct params {
	server *mChat;
	SOCKET mSocket;
};

void mainLoop();
bool commandParse(string str);
void waitForClients(void *pParams);
void clientMessageLoop(void *pParams);

#endif