#ifndef Client_h
#define Client_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class Client{
    public:
        int socket_fd;
        int domain;
        int service;
        int protocol;
        int port;
        u_long interface;
        
        char *request(char * server_ip, void *request, unsigned long int request_size);
        Client(int domain, int service, int protocol, int port, u_long interface);
        ~Client();
};

#endif