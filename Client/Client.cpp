#include "Client.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

Client::Client(int domain, int service, int protocol, int port, u_long interface)
{
    this->domain = domain;
    this->port = port;
    this->interface = interface;

    this->socket_fd = socket(domain, service, protocol);
    if (this->socket_fd < 0)
    {
        perror("\n SOCKET");
        exit(0);
    }
}

Client::~Client()
{
    close(this->socket_fd);
}

char* Client::request(const char* server_ip, void* request, unsigned long request_size)
{
    struct sockaddr_in server;
    server.sin_family = this->domain;
    server.sin_port = htons(this->port);
    server.sin_addr.s_addr = inet_addr(server_ip);

    inet_pton(this->domain, server_ip, &server.sin_addr);
    if (connect(this->socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("\n CONNECT");
        exit(0);
    }

    if (send(this->socket_fd, request, request_size, 0) < 0)
    {
        perror("\n SEND");
        exit(0);
    }

    char* response = (char*)malloc(sizeof(char) * 1024);
    if (read(this->socket_fd, response, 1024) < 0)
    {
        perror("\n RECV");
        exit(0);
    }
    close(this->socket_fd);
    return response;
}