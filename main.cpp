#include "Server/server.h"
#include "Client/client.h"
#include "Utils/ThreadPool.h"
#include "PeertoPeer.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <list>

using namespace std;

#define POOL_SIZE 4
#define MSG_SIZE 256


enum Protocols{
    IP='I',
    MSG='M',
    DROP='D',
};

struct ServerArg{
    int client;
    Server *server;
    list<string> known_hosts;
    list<string> new_hosts;
};

struct Request{
    int protocol;
    const char *message;  
};

Request parser(char* msg, int size){
    if (size < 1)
        return Request{};

    Request request;
    request.protocol = msg[0];
    request.message = msg + 1;
    return request;
}

char* serializer(Request request){
    char *msg = (char*)malloc(sizeof(char) * MSG_SIZE);
    memset(msg, 0, MSG_SIZE);
    msg[0] = request.protocol;
    strcat(msg, request.message);
    return msg;
}

void* server_thread(void* arg)
{
    ServerArg *server_arg = (ServerArg*) arg;

    char request[MSG_SIZE];
    memset(request, 0, MSG_SIZE);
    read(server_arg->client, request, MSG_SIZE);

    Request req = parser(request, MSG_SIZE);

    char *client_address = inet_ntoa(server_arg->server->address.sin_addr);
    
    switch (req.protocol)
    {
        case IP: server_arg->known_hosts.push_back(client_address); break;
        case MSG: cout << "\t" << client_address << " says : " << req.message << "\n"; break;
        case DROP: cout << "\t" << client_address << " dropped the connection\n"; break;

        default: cout << "Unknown protocol\n";
    }
    

    close(server_arg->client);

    bool found = false;
    for (string route : server_arg->known_hosts){
        if (strcmp(route.c_str(), client_address) == 0){
            found = true;
            break;
        }
    }
    if (!found){
        server_arg->new_hosts.push_back(string(client_address));
    }

    return NULL;

}

void *server_function(void *arg){
    
    PeerToPeer *p2p = (PeerToPeer *)arg;
    struct sockaddr *address = (struct sockaddr *)&p2p->server.address;
    socklen_t address_len = sizeof(p2p->server.address);

    cout << "Server is Running: " << inet_ntoa(p2p->server.address.sin_addr) << endl;


    ThreadPool threadpool = ThreadPool(POOL_SIZE);

    while (true)
    {
        struct ServerArg server_arg;
        server_arg.client = accept(p2p->server.socket_fd, address, &address_len);

        server_arg.server = &p2p->server;
        server_arg.known_hosts = p2p->known_hosts;
        
        ThreadJob server_job = ThreadJob(server_thread, &server_arg);
        threadpool.add_work(&server_job);
        
    }
    return NULL;
}


void *client_function(void *arg)
{
    PeerToPeer *p2p = (PeerToPeer *)arg;
    while (true)
    {
        Client client = Client(p2p->domain, p2p->service, p2p->protocol, p2p->port, p2p->interface);
        char* response;
        for (auto host : p2p->known_hosts){

            for(auto new_host: p2p->new_hosts){
                char* msg = serializer(Request{IP, new_host.c_str()});

                client.request(host.c_str(), msg, MSG_SIZE);

            }

        }

        p2p->new_hosts.clear();

        char request[MSG_SIZE];
        memset(request, 0, MSG_SIZE);
        fgets(request, MSG_SIZE, stdin);
        
        char* msg = serializer(Request{MSG, request});

        for (auto host : p2p->known_hosts){
            client.request(host.c_str(), msg, MSG_SIZE);
        }
    }
    
    return NULL;
}


int main(){
    PeerToPeer p2p = PeerToPeer(AF_INET, SOCK_STREAM, 0, 2001, INADDR_ANY, server_function, client_function);
    p2p.start();
}