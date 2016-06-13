#pragma once
#include <ctime>
#include "ServerNetwork.h"
#include "Networking.h"
#include "Player.h"


#define MAX_PLAYERS 4
// game board size in tiles
#define MAX_X 64
#define MAX_Y 46


class ServerGame
{
	ServerNetwork* network;
	static unsigned int player_count;
	char network_data[MAX_PACKET_SIZE]; // data buffer

	Player players[MAX_PLAYERS];
	bool board[MAX_X][MAX_Y]; // game board for fast collision checking

public:
	explicit ServerGame(char *port);
	~ServerGame();

	void update();
	void receiveFromClients();
	void gameLobby();

private:
	void handleActionPacket(unsigned char id, int direction);

	void initializePlayer(unsigned char id);

	void sendInitialPacket(unsigned char id) const;
	void sendNewPlayerPacket(unsigned char id) const;

	void createInitialPacket(unsigned char id, char packet_data[]) const;
	void createNewPlayerPacket(unsigned char id, char packet_data[]) const;
};
