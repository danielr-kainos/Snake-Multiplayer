#pragma once
#include <winsock2.h>
#include <map>
#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 512 // size of the buffer


class ServerNetwork
{
	int iResult; // for error checking return values
	SOCKET ListenSocket; // socket to listen for new connections
	

public:
	SOCKET ClientSocket; // socket to give to the clients IMPORTANT: i've moved it form private section above
	std::map<unsigned int, SOCKET> sessions; // table to keep track of each client's socket

	explicit ServerNetwork(char *port);
	~ServerNetwork(void);

	bool acceptNewClient(unsigned int &id);
	int receiveData(unsigned int client_id, char *recvbuf);
	void sendToAll(char *packets, int totalSize);
};
