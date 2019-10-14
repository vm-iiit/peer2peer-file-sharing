all:
	g++ tracker.cpp -o tracker.out -pthread -lssl -lcrypto -Wall
	g++ peer.cpp -o peer.out -pthread -lssl -lcrypto -Wall