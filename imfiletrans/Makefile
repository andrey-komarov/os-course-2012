all : server sender receiver

sender: sender.cpp
	g++ -Wall -g sender.cpp -o sender

receiver: sender.cpp
	g++ -Wall -g receiver.cpp -o receiver

server: sender.cpp
	g++ -Wall -g server.cpp -o server
