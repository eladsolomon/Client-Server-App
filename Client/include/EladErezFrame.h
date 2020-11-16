//
// Created by erez on 06/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_ELADEREZFRAME_H
#define BOOST_ECHO_CLIENT_ELADEREZFRAME_H

#include <string>
#include "map"
using namespace std;
class EladErezFrame {
private:
    string type;
    map<string, string> headers;
    string body;
    string end = "\u0000";

public:
    EladErezFrame(string type, map<string, string> headers);
    EladErezFrame();
    EladErezFrame(string type, map<string, string> headers, string body);

    map<string, string> getHeaders();

    string getType();
    string getBody();
    string toString();

};
#endif //BOOST_ECHO_CLIENT_ELADEREZFRAME_H
