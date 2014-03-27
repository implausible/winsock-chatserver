#include "application.h"

// called anytime there is a new client added to the server
void clientMessageLoop(void *pParams) {
	params *args = (params *)pParams;
	SOCKET mlClient = args->mSocket;
	server *mChat = args->mChat;

	string name = mChat->getName(mlClient);
	Client newClient(mlClient, name);
	mChat->addClient(newClient);
	string message = mChat->recvMsgs(newClient);
	while (message != "" && message != "disconnected" && message != "error") {
		cout << name << ": " << message << endl << ">";
		message = mChat->recvMsgs(newClient);
	}
	cout << name << " has disconnected from the server." << endl << ">";
}

// waiting for clients
void waitForClients(void *pParams) {
	params *args = (params *)pParams;
	server *mChat = args->mChat;

	SOCKET clientSocket = mChat->waitForClient();
	while (clientSocket != INVALID_SOCKET) {
		cout << "A client has joined the server." << endl << ">";
		args->mSocket = clientSocket;
		_beginthread(clientMessageLoop, 0, (void *)args);
		clientSocket = mChat->waitForClient();
	}
}

// may need to give this its own file soon.
// should definitely expand this to become modular.
bool commandParse(server *mChat, string str) {
	bool flag = true;
	string arg0 = "";
	if (str[0] == '/') {
		size_t found;
		found = str.find(" ");
		// string::npos = -1
		// meaning there exists a space in found that isn't outside the bounds of storable memory
		if (found != string::npos)
		{
			// int(size_t) typecasts found to int
			for(int i = 1; i < int(found); i++)
				arg0 += str[i];
			if (arg0 == "quit")
				flag = false;
			else if (arg0 == "list") {
				cout << mChat->listAllClients() << endl;
				flag = true;
			}
		}
		
	}
	return flag;
}

// this is obvious
void mainLoop() {
	server *mChat = new server;
	params args;
	args.mChat = mChat;
	args.mSocket = NULL;

	bool active = mChat->isListening();
	string messageRcvd;
	if (active) {
		cout << "Server is active." << endl;
		_beginthread(waitForClients, 0, (void *)&args);
		cout << "type /quit to quit." << endl << ">";
		char console[64];
		cin.getline(console,64);
		messageRcvd = console;
		if (messageRcvd == "")
			messageRcvd = "NULL";
		messageRcvd = messageRcvd.append("  ");
		while (commandParse(mChat, messageRcvd)) {
			cout << ">";
			cin.getline(console,64);
			messageRcvd = console;
			if (messageRcvd == "")
			messageRcvd = "NULL";
			messageRcvd = messageRcvd.append("  ");
		}
		mChat->killServer();
	}
}