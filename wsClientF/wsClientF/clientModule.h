#ifndef CLIENTMODULE_H
#define CLIENTMODULE_H

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 4096
#define DEFAULT_NAMELEN 24

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdlib>
#include <string>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

class ClientModule {
public:
	ClientModule();
	ClientModule(char *addr);
	~ClientModule();
	void connectTo(char *addr);
	void sendMessage(char *message);
	bool isOnline();
	string recvMsgs();
private:
	void init(char *test);
	WSADATA wsaData;
	SOCKET server;
	bool isConnected;
};

#endif