#ifndef Server_h
#define Server_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <cstring>


struct cmp_str{
   bool operator()(char const *a, char const *b) const
   {
      return strcmp(a, b) < 0;
   }
};


class ServerRoute{
    public:
    char *(*route_function)(void *arg);
};

class Server{
    public:
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;
    struct sockaddr_in address;
    int socket_fd;
    // std::map<std::string, ServerRoute> routes;

    void init(int domain, int service, int protocol, u_long interface, int port, int backlog);
    Server();
    Server(int domain, int service, int protocol, u_long interface, int port, int backlog);
    
    ~Server();
};


#endif
