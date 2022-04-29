#include <iostream>

#include "Client/Client.h"
#include "Server/server.h"
#include "PeertoPeer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


using namespace std;

char* known_hosts_f(void* arg);

PeerToPeer::PeerToPeer(int domain, int service, int protocol, int port, u_long interface, void *(*server_function)(void *arg), void *(*client_function)(void *arg)) {
    this->domain = domain;
    this->service = service;
    this->protocol = protocol;
    this->port = port;
    this->interface = interface;

    this->known_hosts.push_back("127.0.0.1");
    this->server.init(domain, service, protocol, interface, port, 20);
    this->server_function = server_function;
    this->client_function = client_function;
};

void PeerToPeer::start(){
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, this->server_function, (void *) this);
    this->client_function((void *)this);
};

char* known_hosts_f(void* arg){
    cout << "known host requested: " << endl;
    PeerToPeer *peer_to_peer = (PeerToPeer *) arg;
    char *hosts = (char*)malloc(peer_to_peer->known_hosts.size() * 17);
    memset(hosts, 0, peer_to_peer->known_hosts.size() * 17);
    for (auto host: peer_to_peer->known_hosts){
        strcat(hosts, host.c_str());
        strcat(hosts, ",");
    }
    return hosts;

}

// void user_portal(PeerToPeer* peer_to_peer);
// char* known_hosts(void* arg);
