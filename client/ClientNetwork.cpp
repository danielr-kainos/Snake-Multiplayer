#include <stdio.h>
#include <ws2tcpip.h>
#include "ClientNetwork.h"
#include "Networking.h"

void ClientNetwork::displayError(int errorCode)
{
	if (errorCode != 0)
	{
		std::string s = std::to_string(errorCode);
		char const *pchar = s.c_str();
		char *result = new char[100];
		strcpy(result, message);
		strcat(result, pchar);
		message = result;
	}
	MessageBox(NULL, message, "Error", MB_OK);
}

ClientNetwork::ClientNetwork(char *ip, char *port)
{
	WSADATA wsaData;
	ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = nullptr, *ptr = nullptr, hints;

	// initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		message = "WSAStartup failed with error: %d\n";
		displayError(iResult);
	}

	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  // TCP connection

									  // resolve server address and port 
	iResult = getaddrinfo(ip, port, &hints, &result);

	if (iResult != 0)
	{
		message = "getaddrinfo failed with error: %d\n";
		displayError(iResult);
		WSACleanup();
	}

	// attempt to connect to an address until one succeeds
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{

		// create a socket for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET)
		{
			message = "socket failed with error: %ld\n";
			displayError(WSAGetLastError());
			WSACleanup();
		}

		// connect to server
		iResult = connect(ConnectSocket, ptr->ai_addr, int(ptr->ai_addrlen));

		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			message = "The server is down... did not connect";
			displayError(0);
		}
	}

	// no longer need address info for server
	freeaddrinfo(result);

	// check if connection failed
	if (ConnectSocket == INVALID_SOCKET)
	{
		message = "Unable to connect to server!\n";
		displayError(0);
		WSACleanup();
	}

	// set the socket to be nonblocking
	u_long iMode = 1;

	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR)
	{
		message = "ioctlsocket failed with error: \n";
		displayError(WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}

	// disable Nagle's algorithm to send actions immediately instead of combining packets
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
}


ClientNetwork::~ClientNetwork()
{
}


int ClientNetwork::receivePackets(char *recvbuf)
{
	iResult = NetworkServices::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

	if (iResult == 0)
	{
		message = "Connection closed\n";
		displayError(0);
		closesocket(ConnectSocket);
		WSACleanup();
	}

	return iResult;
}
