#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define DEFAULT_BUFLEN 4096
#define DEFAULT_NAMELEN 24
#ifndef SERVER_H_
#define SERVER_H_
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <list>
#include <process.h>
#include <cstring>
#include <sstream>
#include "client.h"

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

class server {
public:
	server();
	~server();
	void addClient(Client &pClient);
	string getName(SOCKET pClient);
	bool isListening();
	string listAllClients();
	SOCKET waitForClient();
	string recvMsgs(Client &pClient);
	void killServer();
private:
	// Shared WS2 data
	WSADATA wsaData;
	SOCKET lstn;
	list<Client> lClients;
	SOCKET clientSocket;

	// Variables
	int bufferLen;
	bool isLstn;

	// Functions
	void closeAllClients();
	void removeClient(Client pClient);
	void sendToAll(char *msg, int size);
};
#endif