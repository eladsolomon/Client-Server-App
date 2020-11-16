
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../include/Client.h"
#include "../include/EncoderDecoder.h"
#include "../include/EladErezFrame.h"

using namespace std;
using boost::asio::ip::tcp;
Client::Client(string host, short port, mutex &locker, mutex &lockerCh) : userName(),inventory(),borrowBooks(),want_to_borrow(),topic_id(),id_receipt()
                                                         ,subscriptionId(1), receipt_index(0),keepGoing(true),ch(host, port,lockerCh),locker(locker) { }

void Client::writeToServer() {
    map<string,string>::iterator it ;
    map<string,vector<string>>::iterator it2 ;
    vector<string> words;
    do {
        const short bufsize(1024);
        char buf[bufsize];
        cin.getline(buf, bufsize);
        string line(buf);
        boost::split(words, line, boost::is_any_of(" "));

        if(words[0]=="join") {
            string type ="SUBSCRIBE";
            map<string,string> headers;
            int s = line.find(" ");
            headers["destination"]= line.substr(s+1);
            headers["id"] = to_string(subscriptionId);
            string receipt = to_string(receipt_index);
            headers["receipt"] = receipt;
            receipt_index++;
            EladErezFrame frame(type,headers);
            string frameString=EncoderDecoder::decode(frame);
            it = topic_id.find(line.substr(s+1));
            if (it == topic_id.end())//if the topic doesnt  exist
            {
                string answer = "join club "+ line.substr(s+1);
                id_receipt[receipt]=answer;
                ch.sendLine(frameString);
            }
        }
        else if(words[0]=="add") {
            string type ="SEND";
            map<string,string> headers;
            headers["destination"]= words[1];
            int s = line.find(" ");
            string temp = line.substr(s+1);
            int s2 = temp.find(" ");
            string book = temp.substr(s2+1);
            string body= this->userName+" has added the book "+book;
            EladErezFrame frame(type,headers,body);
            string frameString=EncoderDecoder::decode(frame);
            it2 = inventory.find(words[1]);
            if (it2 != inventory.end())//if the topic  exist
            {
                locker.lock();
                if(find(inventory.find(words[1])->second.begin(),inventory.find(words[1])->second.end(),book)==inventory.find(words[1])->second.end())
                {
                    inventory.at(words[1]).push_back(book);
                    ch.sendLine(frameString);
                }
                locker.unlock();
            }
            else{
                vector<string> temp;
                temp.push_back(book);;
                inventory[words[1]]=temp;
                ch.sendLine(frameString);
            }

        }
        else if(words[0]=="borrow") {
            string type ="SEND";
            map<string,string> headers;
            headers["destination"]= words[1];
            int s = line.find(" ");
            string temp = line.substr(s+1);
            int s2 = temp.find(" ");
            string book = temp.substr(s2+1);
            string body= this->userName+" wish to borrow "+book;
            locker.lock();
            want_to_borrow.push_back(book);
            locker.unlock();
            EladErezFrame frame(type,headers,body);
            string frameString=EncoderDecoder::decode(frame);
            ch.sendLine(frameString);
        }
        else if(words[0]=="return") {
            string type ="SEND";
            map<string,string> headers;
            headers["destination"]= words[1];
            int s = line.find(" ");
            string temp = line.substr(s+1);
            int s2 = temp.find(" ");
            string book = temp.substr(s2+1);
            locker.lock();
            auto it = inventory.find(words[1]);
            if (it != inventory.end()){
                auto it3 = find(it->second.begin(), it->second.end(), book);
                if (it3 != it->second.end()) {
                    string body = "Returning " + book + " to " + borrowBooks[book];
                borrowBooks.erase(book);
                this->inventory.find(words[1])->second.erase(
                        remove(inventory.find(words[1])->second.begin(), inventory.find(words[1])->second.end(), book));
                  locker.unlock();

                EladErezFrame frame(type, headers, body);
                string frameString = EncoderDecoder::decode(frame);
                ch.sendLine(frameString);
                }
                else{
                    locker.unlock();
                    cout<<"you dont have this book"<<endl;
                }
            } else{
                locker.unlock();
                cout<<"you dont have this book"<<endl;
            }
        }
        else if(words[0]=="status") {
            string type ="SEND";
            map<string,string> headers;
            headers["destination"]= words[1];
            string body= "book status";
            EladErezFrame frame(type,headers,body);
            string frameString=EncoderDecoder::decode(frame);
            ch.sendLine(frameString);
        }
        else if(words[0]=="exit") {
            int s = line.find(" ");
            it = topic_id.find(line.substr(s+1));
            if (it != topic_id.end())//if the topic  exist
            {
                string type ="UNSUBSCRIBE";
                map<string,string> headers;
                string subID = topic_id[line.substr(s+1)];
                headers["id"] =subID;
                string receipt = to_string(receipt_index);
                headers["receipt"] = receipt;
                receipt_index++;
                EladErezFrame frame(type,headers);
                string frameString=EncoderDecoder::decode(frame);
                string answer = "Exit club "+ line.substr(s+1);
                id_receipt.insert(pair<string,string>(receipt,answer));
                ch.sendLine(frameString);
            }

        }
        else if(words[0]=="logout") {
            string type ="DISCONNECT";
            map<string,string> headers;
            string receipt = to_string(receipt_index);
            headers["receipt"]= receipt;
            receipt_index++;
            EladErezFrame frame(type,headers);
            string frameString=EncoderDecoder::decode(frame);
            string answer = "logout";
            id_receipt.insert(pair<string,string>(receipt,answer));
            ch.sendLine(frameString);

        }

    }while(words[0]!="logout"&&keepGoing);
}

void Client::ReadAndWriteServer() {
    vector<string> words;
    map<string,vector<string>>::iterator it2 ;
    do {
       string stringFrame;
       if(this->ch.getLine(stringFrame))
       {
       if(stringFrame!= "")
       {
           EladErezFrame frame=EncoderDecoder::encode(stringFrame);
           if(frame.getType()=="CONNECTED")
           {
                cout<<"login succesfuly"<<endl;
           }
           else if(frame.getType()=="MESSAGE")
           {
               if(frame.getBody().find("added")!=string::npos)//if someone added book
               {
                 //  cout<<frame.getBody()<<endl;
               }
               else if(frame.getBody().find("borrow")!=string::npos)
               {
                   vector<string> tempWords;
                   boost::split(tempWords, stringFrame, boost::is_any_of(" "));
                   int s = frame.getBody().find("borrow");
                   string bookName = frame.getBody().substr(s+7,frame.getBody().length()-4);
                   string topic=frame.getHeaders()["destination"];
                   if(find(inventory.find(topic)->second.begin(),inventory.find(topic)->second.end(),bookName)!=inventory.find(topic)->second.end())
                   {
                       string type ="SEND";
                       map<string,string> headers;
                       headers["destination"]= frame.getHeaders()["destination"];
                       string body= this->userName + " has "+ bookName;
                       EladErezFrame frame(type,headers,body);
                       string frameString=EncoderDecoder::decode(frame);
                       ch.sendLine(frameString);
                   }
               }
               else if(frame.getBody().find("Taking")!=string::npos)
               {
                   vector<string> tempWords;
                   string tempBody = frame.getBody();
                   boost::split(tempWords, tempBody, boost::is_any_of(" "));
                   int indexOfFrom=0;
                   int index=0;
                   for(int i =0 ; (unsigned)i<tempWords.size();i++)
                   {

                       if(tempWords[i].compare("from")==0)
                       {
                           indexOfFrom=index;
                       }
                       index+=tempWords[i].length()+1;
                   }
                   int start = tempBody.find(" ");
                   int end = indexOfFrom;
                   string bookname = tempBody.substr(start+1,((end-2)-(start)));
                   string username = tempBody.substr(end+5);//TODO check index
                   if(this->userName==username) {
                       string topic= frame.getHeaders()["destination"];
                       locker.lock();
                       this->inventory.find(topic)->second.erase(remove(inventory.find(topic)->second.begin(),inventory.find(topic)->second.end(),bookname));
                       locker.unlock();
                   }

               }
               else if(frame.getBody().find("has")!=string::npos) {
                   int s = frame.getBody().find(" ");
                   string temp = frame.getBody().substr(s+1);
                   int s2 = temp.find(" ");
                   string book = temp.substr(s2+1,frame.getBody().length()-4);
                   string wishBook = book;
                    if(find(want_to_borrow.begin(),want_to_borrow.end(),wishBook)!=want_to_borrow.end()) {
                        string type ="SEND";
                        map<string,string> headers;
                        headers["destination"]= frame.getHeaders()["destination"];
                        vector<string> tempWords2;
                        string bodyTemp = frame.getBody();
                        boost::split(tempWords2, bodyTemp, boost::is_any_of(" "));
                        string fromUser = tempWords2[0];
                        string body="Taking "+wishBook+" from "+fromUser;
                        locker.lock();
                        borrowBooks.insert(pair<string,string>(book,fromUser));
                        want_to_borrow.erase(remove(want_to_borrow.begin(),want_to_borrow.end(),book), want_to_borrow.end());
                        locker.unlock();
                        //add to inventory
                        it2 = inventory.find(frame.getHeaders()["destination"]);
                        if (it2 != inventory.end())//if the topic  exist
                        {
                            inventory.at(frame.getHeaders()["destination"]).push_back(book);
                        }
                        else{
                            vector<string> temp;
                            temp.push_back(book);;
                            inventory[frame.getHeaders()["destination"]]=temp;
                        }

                        EladErezFrame frame(type,headers,body);
                        string frameString=EncoderDecoder::decode(frame);
                        ch.sendLine(frameString);
                    }
               }
               else if(frame.getBody().find("Returning")!=string::npos) {
                   vector<string> tempWords2;
                   string tempBody = frame.getBody();
                   boost::split(tempWords2,tempBody , boost::is_any_of(" "));
                   int indexOfTo=0;
                   int index=0;
                   for(int i =0 ; (unsigned)i<tempWords2.size();i++)
                   {

                       if(tempWords2[i].compare("to")==0)
                       {
                           indexOfTo=index;
                       }
                       index+=tempWords2[i].length()+1;
                   }
                   int start = tempBody.find(" ");
                   int end = indexOfTo;
                   string bookname = tempBody.substr(start+1,((end-1)-(start+1)));
                   string username = tempBody.substr(end+3);
                   string destination = frame.getHeaders()["destination"];
                   if(this->userName==username) {
                       string book =bookname;
                       inventory[destination].push_back(book);
                   }
               }
               else if(frame.getBody().find("status")!=string::npos) {
                   string type ="SEND";
                   map<string,string> headers;
                   headers["destination"]= frame.getHeaders()["destination"];
                   string body=this->userName + ":";
                   for (auto i = inventory[frame.getHeaders()["destination"]].begin(); i != inventory[frame.getHeaders()["destination"]].end(); ++i)
                       body += *i +",";
                   body.pop_back();
                   EladErezFrame frame(type,headers,body);
                   string frameString=EncoderDecoder::decode(frame);
                   ch.sendLine(frameString);
               }
                 cout<<frame.getHeaders()["destination"]+": "+frame.getBody()<<endl;
               }
           else  if(frame.getType()=="RECEIPT")
           {
               string index_receipt=frame.getHeaders()["receipt-id"];
               string ans = id_receipt[index_receipt];
               if(ans.find("Exit") != string::npos) {
                   int s = ans.find("b");
                   topic_id.erase(ans.substr(s + 2));
                   inventory.erase(ans.substr(s + 2));
               }
               else if(ans.find("join") != string::npos) {
                   vector<string> temp;
                   boost::split(temp, ans, boost::is_any_of(" "));
                   string topic =temp[2];
                   topic_id[topic]=to_string(subscriptionId);
                   this->subscriptionId++;
               }
               if(ans.find("logout") != string::npos) {
                   ch.close();
                   cout<<"Disconnect from the server succefully"<<endl;
                   this->keepGoing = false;
               }
               if(keepGoing)
                    cout<<ans<<endl;
           }
           if(frame.getType()=="ERROR")
           {
               this->keepGoing= false;
               cout<<"ERROR - "+frame.getHeaders()["message"]<<endl;
           }
           }
       } else
       {
           this->keepGoing=false;
           cerr<<"lost Connection to the server"<<endl;
       }
    }
    while(keepGoing);


}
bool Client::connect() {
    return this->ch.connect();
}

bool Client::send(string line) {
    return this->ch.sendLine(line);
}
void Client::setName(string username) {
    this->userName=username;
}

