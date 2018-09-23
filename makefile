all: peer tracker

peer: peer.cpp
	g++ peer.cpp -std=c++11 -lpthread -lcrypto  -Wall -o peer

tracker: tracker.cpp
	g++ tracker.cpp -std=c++11  -Wall -o tracker

clean: rm -f peer tracker
