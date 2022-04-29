#include <iostream>
#include "Server/server.h"
#include "Client/client.h"
#include "PeertoPeer.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;


void *server_function(void *arg){
    // cout << ("Server is Running: \n");
    PeerToPeer *p2p = (PeerToPeer *) arg;
    struct sockaddr *address = (struct sockaddr *) &p2p->server.address;
    socklen_t address_len = sizeof(p2p->server.address);

    while(true){
        int client = accept(p2p->server.socket_fd, address, &address_len);
        char request[256];
        memset(request, 0, 256);
        read(client, request, 256);
        char* client_address = inet_ntoa(p2p->server.address.sin_addr);

        // cout << "\t\t" << client_address << ": " << request << "\n";
        
        close(client);
        bool found = false;
        for(string route: p2p->known_hosts){
            if (strcmp(route.c_str(), client_address) == 0){
                found = true;
                break;
            }
        }
        if (!found){
            p2p->known_hosts.push_back(string(client_address));
        }
    }
    
    return NULL;
}

void *client_function(void* arg){
    PeerToPeer *p2p = (PeerToPeer*)arg;
    while (true){
        Client client = Client(p2p->domain, p2p->service, p2p->protocol, p2p->port, p2p->interface);
        char request[256];
        memset(request, 0, 256);
        fgets(request, 256, stdin);

        for(auto host: p2p->known_hosts){
            char* temp = (char*)malloc(host.size());
            strcpy(temp, host.c_str());
            client.request(temp, request, 256);
        }
        
    }
    Client client = Client(AF_INET, SOCK_STREAM, 0, 2001, INADDR_ANY);
    client.request("0.0.0.0", arg, 256);
}

int main(){
    PeerToPeer p2p = PeerToPeer(AF_INET, SOCK_STREAM, 0, 2001, INADDR_ANY, server_function, client_function);
    p2p.start();
}