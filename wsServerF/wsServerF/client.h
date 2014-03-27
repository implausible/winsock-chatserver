#ifndef CLIENT_H_
#define CLIENT_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
using namespace std;

class Client {
public:
	Client();
	Client(SOCKET clSock, string pName);
	unsigned int GetID();
	string GetName();
	SOCKET GetSock();
	bool operator==(Client param);
private:
	static unsigned int counter;
	unsigned int clID;
	string clName;
	SOCKET clSocket;
};
#endif