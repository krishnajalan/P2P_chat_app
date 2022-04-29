default: main

main: main.cpp Server/server.cpp Client/client.cpp
	g++ main.cpp -o main ./Client/Client.cpp ./Server/server.cpp ./PeertoPeer.cpp -lpthread

clean:
	rm -f main