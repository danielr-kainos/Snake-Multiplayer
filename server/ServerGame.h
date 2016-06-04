#pragma once
#include "ServerNetwork.h"
#include "Networking.h"


class ServerGame
{
	ServerNetwork* network;
	static unsigned int client_id; // IDs for the clients connecting
	char network_data[MAX_PACKET_SIZE]; // data buffer
	int numberOfClients = 0; // number of players needed to start a new game


public:
	explicit ServerGame(char *port);
	~ServerGame();

	void update();
	void receiveFromClients(static unsigned int client_id);
	void sendActionPackets() const;
	void gameLobby(static unsigned int client_id);
	void sendInitialPackets();
};
