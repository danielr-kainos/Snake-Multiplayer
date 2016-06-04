#include "ServerGame.h"


unsigned int ServerGame::client_id;


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
	if (network->acceptNewClient(client_id))
	{
		printf("client %d has been connected to the server\n", client_id);
		client_id++;
	}

	receiveFromClients(client_id);
}

void ServerGame::gameLobby(static unsigned int client_id)
{
	if (client_id != 4) {
		printf("Players in lobby: %d.\nNeed: %d more to start.\n", client_id, 4 - client_id);
	}
	else {
		printf("All players are connected, waiting for a game to start. \n");
		sendInitialPackets(); // this line should initialize a new game
	}
}

void ServerGame::receiveFromClients(static unsigned int client_id)
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
			case INIT_CONNECTION:
				printf("server received init packet from client\n");
				gameLobby(client_id);
				sendActionPackets();
				break;
			case ACTION_EVENT:
				printf("server received action event packet from client\n");
				sendActionPackets();
				break;
			default:
				printf("error in packet types\n");
				break;
			}
		}
	}
}


void ServerGame::sendActionPackets() const
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	network->sendToAll(packet_data, packet_size);
}

void ServerGame::sendInitialPackets()
{
	// send initial packet
	// TODO: write an initializer for game start, im not sure if the code below works properly
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];
	Packet packet;
	packet.packet_type = INIT_CONNECTION;
	packet.serialize(packet_data);
	NetworkServices::sendMessage(network->ClientSocket, packet_data, packet_size);
}
