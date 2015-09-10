#include "RakPeerInterface.h"
#include <iostream>
#include <ctype.h>

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

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

    system("pause");

    RakNet::RakPeerInterface::DestroyInstance(peer);

    return 0;
}
