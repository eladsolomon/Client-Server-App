//
// Created by erez on 07/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_CLIENT_H
#define BOOST_ECHO_CLIENT_CLIENT_H
#include "connectionHandler.h"
#include <string>
#include <mutex>
#include "map"
#include "vector"
#include "EladErezFrame.h"

using namespace std;
class Client {
private :
    std::string userName;
    map<std::string, vector<std::string>> inventory;
    map<std::string, std::string> borrowBooks;
    vector<string> want_to_borrow;
    map<std::string, std::string> topic_id;//no need to synch
    map<string,string> id_receipt;//no nned to synch
    int subscriptionId;
    int receipt_index;
    bool keepGoing;
    ConnectionHandler ch;
    std::mutex &locker;
public:
    Client(string host, short port, mutex &locker, mutex &lockerCh);
    void ReadAndWriteServer();
    void writeToServer();
    bool connect();
    bool send(string);
    void setName(string);
};


#endif //BOOST_ECHO_CLIENT_CLIENT_H
