//
// Created by user on 1/15/20.
//

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "EncoderDecoder.h"
#include "vector"
using namespace std;
string EncoderDecoder::decode(EladErezFrame frame) {
    return frame.toString();

}
EladErezFrame EncoderDecoder::encode(string basicString) {
        vector <string> lines;
        boost::split(lines, basicString, boost::is_any_of("\n"));
        string type = lines[0];

        std::map<string, string> headers; //= new map<string, string>();
        int counter = 1;
        for (;(unsigned)counter< lines.size()&& !lines[counter].empty(); counter++) {
            if(lines[counter].find(":")!=string::npos){
                vector <string> header;
                boost::split(header, lines[counter], boost::is_any_of(":"));
                headers[header[0]]= header[1];
            }
        }
        counter++;
        string body = "";
        while (lines.size()>(unsigned)counter&&!lines[counter].empty()) {
            body += lines[counter];
            counter++;
        }
        EladErezFrame returnFrame(type, headers, body);
        return returnFrame;
}