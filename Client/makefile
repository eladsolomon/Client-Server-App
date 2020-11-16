CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -pthread

all: StompBookClubClient
	g++ -o bin/StompBookClubClient bin/connectionHandler.o bin/echoClient.o bin/Client.o bin/mainClient.o bin/EladErezFrame.o bin/EncoderDecoder.o $(LDFLAGS) 

StompBookClubClient: bin/connectionHandler.o bin/echoClient.o bin/Client.o bin/mainClient.o bin/EladErezFrame.o bin/EncoderDecoder.o
	
bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/Client.o: src/Client.cpp
	g++ $(CFLAGS) -o bin/Client.o src/Client.cpp

bin/mainClient.o: src/mainClient.cpp
	g++ $(CFLAGS) -o bin/mainClient.o src/mainClient.cpp
	
bin/EncoderDecoder.o: src/EncoderDecoder.cpp
	g++ $(CFLAGS) -o bin/EncoderDecoder.o src/EncoderDecoder.cpp

bin/EladErezFrame.o: src/EladErezFrame.cpp
	g++ $(CFLAGS) -o bin/EladErezFrame.o src/EladErezFrame.cpp

.PHONY: clean
clean:
	rm -f bin/*
