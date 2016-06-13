#pragma once
#include <winsock2.h>
#include <string>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512 // size of the buffer


class ClientNetwork
{
	int iResult; // for error checking function calls in Winsock library

public:
	SOCKET ConnectSocket; // socket for client to connect to server
	char *message = new char[30];

	ClientNetwork(char *ip, char *port);
	~ClientNetwork();

	int receivePackets(char *);
	void displayError(int errorCode);
};
