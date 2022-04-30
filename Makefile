default: main

main: main.cpp Server/* Client/* PeertoPeer.cpp Utils/*
	g++ main.cpp -o main ./Client/Client.cpp ./Server/server.cpp ./Utils/ThreadPool.cpp ./PeertoPeer.cpp -lpthread -g

clean:
	rm -f main