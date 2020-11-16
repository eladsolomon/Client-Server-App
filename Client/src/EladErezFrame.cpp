


#include "../include/EladErezFrame.h"

EladErezFrame::EladErezFrame(string type, map<string, string> headers): type(type), headers(headers),body("") {
}
EladErezFrame::EladErezFrame(string type, map<string, string> headers, string body):type(type), headers(headers),body(body) {

}
EladErezFrame::EladErezFrame() : type(),headers(),body("") {}
string EladErezFrame::getType() {
    return this->type;
}
map<string, string> EladErezFrame::getHeaders() {
    return this->headers;
}
string EladErezFrame::toString() {
    string ans = "";
    ans += type + "\n";
    for (std::map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        ans += it->first + ":" + it->second + "\n";
    }
    ans += '\n';
    if (!body.empty()) {
        ans += body;
    }//MAGIC
    ans+=end;
    return ans;
}
string EladErezFrame::getBody() {
    return this->body;
}
