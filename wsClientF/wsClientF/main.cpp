#include "main.h"

using namespace std;

ClientModule chatSrv;

bool quitMessage(char *message);

// pParams are from _beginthread, see last argument
void runRecvThread(void *pParams) {
	string messages = chatSrv.recvMsgs();
	while (messages != "") {
		cout << messages << endl << ">";
		messages = chatSrv.recvMsgs();
	}
	exit(0);
}

int main(int argc, char** argv) {
	// parse the string to make sure it is an ip address... please
	string in;
	cout << ">Enter the address you wish to join:" << endl << ">";
	cin >> in;
	char *addr = (char *)in.c_str();
	chatSrv.connectTo(addr);

	// Ovious design
	if (!chatSrv.isOnline())
	{
		cout << ">Error connecting to server." << endl
			 << ">Press any key to exit...";
		cin.ignore(2);
		return 1;
	}

	// displays messages received from the server
	_beginthread(runRecvThread, 0, NULL);

	// input array of size DEFAULT_BUFLEN
	char input[DEFAULT_BUFLEN];

	cout << ">Enter your name: ";
	char name[DEFAULT_NAMELEN];
	cin >> name;
	chatSrv.sendMessage(name);

	// obvious design
	cout << ">Welcome to the discussion." << endl
		 << ">(Use /quit to exit.)" << endl
		 << ">";
	// For future writing...
	cin.getline(input, DEFAULT_BUFLEN);
	while (!quitMessage(input)) {
		chatSrv.sendMessage(input);
		cin.getline(input, DEFAULT_BUFLEN);
		cout << ">";
	}
	return 0;
}

bool quitMessage(char *message) {
	string test = message;
	if (test.substr(0,5) == "/quit")
		return true;
	return false;
}