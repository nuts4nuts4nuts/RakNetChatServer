#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakNetTypes.h"

#include <iostream>
#include <string>
#include <ctype.h>

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

enum ChatMessages
{
    ID_CHAT_MESSAGE_1=ID_USER_PACKET_ENUM + 1,
};

int main()
{
    std::cout << "Hello there.\n\n";
    std::cout << "Would you like to run as a (c)lient or a (s)erver?\n";

    char answer;
    std::cin >> answer;
    answer = tolower(answer);

    while(answer != 'c' && answer != 's')
    {
        std::cout << "That is not a valid answer.\n";
        std::cout << "Would you like to run as a client (c) or a server (s)?\n";
        std::cin >> answer;
        answer = tolower(answer);
    }

    RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
    bool isServer;
    RakNet::Packet* packet;

    if(answer == 'c')
    {
        RakNet::SocketDescriptor sd;
        peer->Startup(1, &sd, 1);
        isServer = false;
    }
    else
    {
        RakNet::SocketDescriptor sd(SERVER_PORT, 0);
        peer->Startup(MAX_CLIENTS, &sd, 1);
        isServer = true;
    }

    if(isServer)
    {
        std::cout << "Starting up the server...\n";
        peer->SetMaximumIncomingConnections(MAX_CLIENTS);
    }
    else
    {
            std::string ipAddress;
    std::cout << "Enter the IP address for the server (entering nothing = 127.0.0.1)\n";
        std::getline(std::cin, ipAddress);

        if(ipAddress.length() <= 0)
        {
            ipAddress = "127.0.0.1";
        }

        std::cout << "Starting up the client...\n";
        peer->Connect(ipAddress.c_str(), SERVER_PORT, 0, 0);
    }

    while(true)
    {
        for(packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
        {
            switch (packet->data[0])
            {
                case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                    std::cout << "Someone has disconnected.\n";
                    break;
                case ID_REMOTE_CONNECTION_LOST:
                    std::cout << "Someone has lost connection.\n";
                    break;
                case ID_REMOTE_NEW_INCOMING_CONNECTION:
                    std::cout << "Someone has connected!\n";
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    {
                        std::cout << "We have connected!\n" << "Write a message.\n";

                        RakNet::BitStream bitStream;
                        bitStream.Write((RakNet::MessageID)ID_CHAT_MESSAGE_1);
                        std::string message;
                        std::getline(std::cin, message);
                        bitStream.Write(message.c_str());
                        peer->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                   packet->systemAddress, false);
                    }
                    break;
                case ID_NEW_INCOMING_CONNECTION:
                    std::cout << "A connection is incoming.\n";
                    break;
                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    std::cout << "The server is full.\n";
                    break;
                case ID_DISCONNECTION_NOTIFICATION:
                    if (isServer){
                        std::cout << "A client has disconnected.\n";
                    } else {
                        std::cout << "We have been disconnected.\n";
                    }
                    break;
                case ID_CONNECTION_LOST:
                    if (isServer){
                        std::cout << "A client lost the connection.\n";
                    } else {
                        std::cout << "Connection lost.\n";
                    }
                    break;
                case ID_CHAT_MESSAGE_1:
                    {
                        RakNet::RakString string;
                        RakNet::BitStream bitStream(packet->data, packet->length, false);
                        bitStream.IgnoreBytes(sizeof(RakNet::MessageID));
                        bitStream.Read(string);
                        std::cout << string.C_String() << std::endl;
                    }
                    break;
                default:
                    std::cout << "Message with identifier "
                              << packet->data[0]
                              << " has arrived.\n";
                    break;
            }
        }
    }

RakNet::RakPeerInterface::DestroyInstance(peer);

return 0;
}
