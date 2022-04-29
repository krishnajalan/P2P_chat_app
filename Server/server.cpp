#include "server.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Server::Server(){
    this->domain = AF_INET;
    this->service = SOCK_STREAM;
    this->protocol = 0;
    this->port = 8080;
    this->interface = INADDR_ANY;
}

Server::Server(int domain, int service, int protocol, u_long interface, int port, int backlog)
{
    this->init(domain, service, protocol, interface, port, backlog);
};



void Server::init(int domain, int service, int protocol, u_long interface, int port, int backlog)
{
    this->domain = domain;
    this->service = service;
    this->protocol = protocol;
    this->port = port;
    this->interface = interface;

    this->address.sin_family = domain;
    this->address.sin_port = htons(port);
    this->address.sin_addr.s_addr = htonl(interface);

    this->socket_fd = socket(domain, service, protocol);
    if (this->socket_fd < 0)
    {
        perror("\n SOCKET");
        exit(1);
    }
    if ((bind(this->socket_fd, (struct sockaddr *)&this->address, sizeof(this->address))) < 0)
    {
        perror("\n BIND");
        exit(1);
    }
    if ((listen(this->socket_fd, this->backlog)) < 0)
    {
        perror("\n LISTEN");
        exit(1);
    }
};

Server::~Server()
{
    close(this->socket_fd);
}