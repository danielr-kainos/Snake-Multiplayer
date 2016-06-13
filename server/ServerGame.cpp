#include "ServerGame.h"


unsigned int ServerGame::player_count;


ServerGame::ServerGame(char *port)
{
	network = new ServerNetwork(port);
}


ServerGame::~ServerGame()
{
}


void ServerGame::update()
{
	// get new clients
	if (player_count < MAX_PLAYERS && network->acceptNewClient(player_count))
	{
		printf("client %d has connected to the server\n", ++player_count);
		gameLobby();
		initializePlayer(player_count);
		sendInitialPacket(player_count);
		sendNewPlayerPacket(player_count);
	}

	receiveFromClients();
}

void ServerGame::gameLobby()
{
	if (player_count != 4) {
		printf("Players in lobby: %d.\nNeed: %d more to start.\n", player_count, 4 - player_count);
	}
	else {
		printf("All players are connected, waiting for a game to start. \n");
	}
}

void ServerGame::receiveFromClients()
{
	Packet packet;
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end(); ++iter)
	{
		int data_length = network->receiveData(iter->first, network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		int i = 0;
		while (i < data_length)
		{
			packet.deserialize(&network_data[i]);
			i += sizeof(Packet);

			switch (packet.packet_type) {
			case ACTION_EVENT:
				handleActionPacket(iter->first - 1, network_data[4]);
				break;
			default:
				printf("error in packet types\n");
				break;
			}
		}
	}
}


void ServerGame::handleActionPacket(unsigned char id, int direction)
{
	auto prevDirection = players[id].direction;
	auto curDirection = DirectionEnum(direction);

	if (prevDirection == curDirection) return;

	switch (curDirection)
	{
	case UP:
		if (prevDirection == DOWN) return;
		break;
	case RIGHT:
		if (prevDirection == LEFT) return;
		break;
	case DOWN:
		if (prevDirection == UP) return;
		break;
	case LEFT:
		if (prevDirection == RIGHT) return;
		break;
	default:
		break;
	}

	players[id].direction = curDirection;

#ifdef _DEBUG
	printf("client %d changed direction to %d\n", id + 1, curDirection);
#endif
}


void ServerGame::initializePlayer(unsigned char id)
{
	auto dir = DirectionEnum(rand() % LEFT);
	unsigned char x = MAX_X / 5 + rand() % MAX_X / 5 + 2 * ((id + 1) % 2) * MAX_X / 5;
	unsigned char y = MAX_Y / 5 + rand() % MAX_Y / 5;
	if (id > 2) y += 2 * MAX_Y / 5;

	Player p = Player(id, dir, Position(x, y));

	#ifdef _DEBUG
		printf("Created player %d with direction %d and position (%d, %d)\n", p.id, p.direction, p.positions[0].x, p.positions[0].y);
	#endif

	players[id - 1] = p;
	board[x][y] = true;
}


void ServerGame::sendInitialPacket(unsigned char id) const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_PACKET;
	createInitialPacket(id, packet.data);

	packet.serialize(packet_data);
	network->sendToOne(id, packet_data, packet_size);
}


void ServerGame::sendNewPlayerPacket(unsigned char id) const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];


	Packet packet;
	packet.packet_type = NEW_PLAYER_CONNECTED;
	createNewPlayerPacket(id, packet.data);

	packet.serialize(packet_data);
	network->sendToAllButOne(id, packet_data, packet_size);
}


void ServerGame::createInitialPacket(unsigned char id, char packet_data[]) const
{
	int index = 0;

	packet_data[index++] = id; // first bit -> player's id

	// store data about every player's position
	for (unsigned int i = 0; i < player_count; i++)
	{
		packet_data[index++] = players[i].id;
		packet_data[index++] = players[i].positions[0].x;
		packet_data[index++] = players[i].positions[0].y;
	}

	// add two -1s at the end for easier parsing on client side
	packet_data[index + 1] = packet_data[index] = -1;

	#ifdef _DEBUG
		printf("Created initial packet for client %d:\n", id);
		for (unsigned int i = 0; i < (1 + player_count) * 3; i++)
			printf("%d,", packet_data[i]);
		printf("\n");
	#endif
}


void ServerGame::createNewPlayerPacket(unsigned char id, char packet_data[]) const
{
	packet_data[0] = players[id - 1].positions[0].x;
	packet_data[1] = players[id - 1].positions[0].y;

	#ifdef _DEBUG
		printf("Created new player packet: %d, %d\n", packet_data[0], packet_data[1]);
	#endif
}
