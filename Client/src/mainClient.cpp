//
// Created by erez on 07/01/2020.
//

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../include/mainClient.h"
#include "string"
#include "vector"
#include "iostream"
#include "../include/Client.h"
#include "../include/EladErezFrame.h"
#include <boost/lexical_cast.hpp>
#include <thread>

using namespace std;

int main (int argc, char *argv[]) {
    vector<string> words;
    do{
         words.clear();
         cout<<"Please Log in"<<endl;
        const unsigned short bufsize(1024);
        char buf[bufsize];
        cin.getline(buf, bufsize);
        string line(buf);
        boost::split(words, line, boost::is_any_of(" "));
    }while(words[0] != "login");
             vector<string> login_words;
            boost::split(login_words, words[1], boost::is_any_of(":"));
            unsigned short port = boost::lexical_cast<unsigned short>(login_words[1]);
            string host = login_words[0];
            mutex mutex,lockerCh;
            Client client(host, port, mutex, lockerCh);
            bool succeed = client.connect();
            if (succeed) {
                string type = "CONNECT";
                map<string, string> headers;
                headers["accept-version"]= "1.2";
                headers["host"] = host;
                headers["login"] = words[2];
                headers["passcode"] = words[3];
                EladErezFrame frame(type, headers);
                client.setName(words[2]);
                client.send(frame.toString());

                thread readAndWrite(&Client::ReadAndWriteServer, &client);
                thread writeToServer(&Client::writeToServer, &client);
                readAndWrite.join();
                writeToServer.join();

            } else {
                cout << "Could not connect to server" << endl;
            }
        }

