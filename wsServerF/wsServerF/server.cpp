#include "server.h"

// starts a listening server on port 27015
server::server() {
	isLstn = false;
	bufferLen = DEFAULT_BUFLEN;
	int nRet;
	char *port = "27015";
	struct addrinfo *result = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));

	// using winsock 2.2
	nRet = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (nRet != 0) // errors
		return;

	//config for socket
	hints.ai_family = AF_INET;				// over TCP/IP
	hints.ai_socktype = SOCK_STREAM;		// see above
	hints.ai_protocol = IPPROTO_TCP;		// see above 
	hints.ai_flags = AI_PASSIVE;			// determine address of socket for us

	nRet = getaddrinfo(NULL, port, &hints, &result); // using config from hints, store all required info in result

	if (nRet != 0) { // errors?
		WSACleanup();
		return;
	}

	lstn = INVALID_SOCKET;

	lstn = socket(result->ai_family,result->ai_socktype,result->ai_protocol); // create the socket from result

	if (lstn == INVALID_SOCKET) { // we're unable to create the socket
		freeaddrinfo(result);
		WSACleanup();
		closesocket(lstn);
		return;
	}

	nRet = bind(lstn, result->ai_addr, (int)result->ai_addrlen); // bind the socket for listen

	if (nRet == SOCKET_ERROR) { // errors OH GOD
		freeaddrinfo(result);
		WSACleanup();
		closesocket(lstn);
		return;
	}


	freeaddrinfo(result);
	// somaxconn is going to handle the maximum number of queued connections
	nRet = listen(lstn, SOMAXCONN);
	
	if (nRet == SOCKET_ERROR) { // still broken?
		WSACleanup();
		closesocket(lstn);
		return;
	}
	isLstn = true;
}

void server::addClient(Client &pClient) {
	lClients.push_back(pClient);
}

bool server::isListening() {
	return isLstn;
}


// keep for now, not very dynamic
string server::getName(SOCKET pClient) {
	int nRet;
	char recvBuffer[DEFAULT_NAMELEN];
	string name = "";
	nRet = recv(pClient, recvBuffer, DEFAULT_NAMELEN, 0);
	if (nRet > 0) { //nRet is the number of bits recieved
		for (int i = 0; i < nRet; i++)
			name += recvBuffer[i];
		return name;
	} else if (nRet == 0) {
		return "";
	} else {
		return "";
	}
}


//This will list all the clients currently connected to the server.
string server::listAllClients() {
	stringstream result;
	result << ">Listing all clients connected:";
	for(list<Client>::iterator i = lClients.begin(); i != lClients.end(); ++i) {
		result << endl << ">" << ((Client)*i).GetID() << ": " << ((Client)*i).GetName();
	}
	return result.str();
}

// return the socket the client has connected on
// so that main can start a new thread for recvMsgs
// note instead of doing this, return the socket as before, but instead implement the creation of the client object in recvmessagethread in main.cpp
SOCKET server::waitForClient() {

	clientSocket = INVALID_SOCKET;
	clientSocket = accept(lstn,NULL,NULL);

	if (clientSocket == INVALID_SOCKET) {
		closesocket(clientSocket);
		return INVALID_SOCKET;
	}
	return clientSocket;
}

// create an object for controlling text in the window so that this object doesn't have to open iostream

// most of this is obvious
string server::recvMsgs(Client &pClient) {
	int nRet;
	char recvBuffer[DEFAULT_BUFLEN];
	string message = "";
	SOCKET clSocket = pClient.GetSock();
	nRet = recv(clSocket, recvBuffer, bufferLen, 0);
	if (nRet > 0) { //nRet is the number of bits recieved
		for (int i = 0; i < nRet; i++) // cut off null characters
			message += recvBuffer[i];
		string sendMessage = pClient.GetName() + ": " + message;
		nRet = sendMessage.length();
		char *sendBuffer = (char *)sendMessage.c_str();
		sendToAll(sendBuffer,nRet);
		return message;
	} else if (nRet == 0) {
		removeClient(pClient);
		return "error";
	} else {
		removeClient(pClient);
		return "disconnected";
	}
}

void server::removeClient(Client pClient) {
	closesocket(pClient.GetSock());
	lClients.remove(pClient);
}

// obvious design
void server::sendToAll(char *message, int length) {
	int nSend;
	// loop to all clients
	for(list<Client>::iterator toClient = lClients.begin(); toClient != lClients.end(); toClient++) {
		Client sendTo = (Client) *toClient;
		SOCKET outSock = sendTo.GetSock();
		nSend = send(outSock, message, length, 0);
		if (nSend == SOCKET_ERROR) {
			removeClient(sendTo);
		}
	}
}

// obvious design
void server::closeAllClients() {
	int nSend;
	// shutdown all the open sockets
	for(list<Client>::iterator killClient = lClients.begin(); killClient != lClients.end(); killClient++) {
		Client kill = (Client)*killClient;
		SOCKET outSocket = kill.GetSock();
		nSend = shutdown(outSocket,SD_SEND);
		closesocket(outSocket);
	}
	// empty the list
	lClients.clear();
}

void server::killServer() {
	closeAllClients();
	WSACleanup();
	closesocket(lstn);
}

server::~server() {
}