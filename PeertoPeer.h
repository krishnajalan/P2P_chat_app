#ifndef PeerToPeer_h
#define PeerToPeer_h

#include "Client/Client.h"
#include "Server/server.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>

class PeerToPeer{
    public:
        Server server;
        std::list<std::string> known_hosts;
        int domain;
        int service;
        int protocol;
        int port;
        u_long interface;
        
        PeerToPeer(
                    int domain,
                    int service,
                    int protocol, 
                    int port, 
                    u_long interface, 
                    void * (*server_function)(void *arg), 
                    void * (*client_function)(void *arg)
                    );
        void start();
        void *(*server_function)(void *arg);
        void *(*client_function)(void *arg);
};


#endif