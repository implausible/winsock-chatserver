#include "client.h"

// for foo clients
Client::Client() {
	clSocket = INVALID_SOCKET;
	clName = "";
	clID = -1;
}

// for real clients
Client::Client(SOCKET clSock, string pName) {
	clSocket = clSock;
	clName = pName;
	clID = counter;
	++counter;
}

// for assigning an id
unsigned int Client::counter = 0;

// return id
unsigned int Client::GetID() { return clID; }

// return name
string Client::GetName() { return clName; }

// return socket
SOCKET Client::GetSock() { return clSocket; }

// necessary to compare clients
bool Client::operator==(Client param) { return GetID() == param.GetID(); }
