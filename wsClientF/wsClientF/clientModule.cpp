#include "clientModule.h"

// Notes: Still learning about c-strings
ClientModule::ClientModule() {
	//char *addr = "25.34.3.168";
	isConnected = false;
	//init(addr);
}
ClientModule::ClientModule(char *addr) {
	isConnected = false;
	init(addr);
}

void ClientModule::connectTo(char *addr) {
	init(addr);
}

void ClientModule::init(char *addr) {
	int iResult;
	struct addrinfo *result = NULL, hints;

	// using winsock 2.2
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) { // oh no :O
		exit(1);
		return;
	}
	
	ZeroMemory(&hints, sizeof(hints)); // obvious

	hints.ai_family = AF_UNSPEC; // either ipv4 or ipv6 is acceptable
	hints.ai_socktype = SOCK_STREAM; // stream over datagram socket
	hints.ai_protocol = IPPROTO_TCP; // using tcp/ip

	// using the above config, assign address information to result, not iResult.
	iResult = getaddrinfo(addr, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) { // uh oh
		WSACleanup();
		exit(1);
		return;
	}

	server = INVALID_SOCKET; // default socket
	server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	// if no change
	if (server == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		exit(1); // blow up
		return;
	}

	// establish connection
	iResult = connect(server, result->ai_addr, (int)result->ai_addrlen);

	// bad connection
	if (iResult == SOCKET_ERROR) {
		closesocket(server);
		server = INVALID_SOCKET;
		return;
	}
	// clear memory
	freeaddrinfo(result);
	// oh fuck, still invalid socket?
	if (server == INVALID_SOCKET) {
		WSACleanup();
		exit(1);
		return;
	}
	//we're online
	isConnected = true;
}

// should add error checking to this
void ClientModule::sendMessage (char *message) {
	int iResult;

	// this sends c-string of length strlen(message)
	// to our server socket.
	iResult = send(server, message, (int) strlen(message), 0);

	if (iResult == SOCKET_ERROR) {
		closesocket(server);
		WSACleanup();
	}
}

string ClientModule::recvMsgs() {
	char msg[DEFAULT_BUFLEN+DEFAULT_NAMELEN]; // sizeof expected server message
	string message = ""; // easier for me to use than c-strings
	// 
	int iResult = recv(server,msg,DEFAULT_BUFLEN,0);
	if (iResult > 0) {
		//send message out of object.
		for (int i = 0; i < iResult; i++) // cut off nullspace from msg
			message += msg[i]; // store in message
		return message;
	} else if (iResult == 0) { // error
		return "";
	} else { // disconnected
		return "";
	}
}

//obvious
bool ClientModule::isOnline() {
	return isConnected;
}

// largely obvious
ClientModule::~ClientModule() {
	isConnected = false;
	int iResult = shutdown(server, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		closesocket(server);
		WSACleanup();
	}
	closesocket(server);
	WSACleanup();
}