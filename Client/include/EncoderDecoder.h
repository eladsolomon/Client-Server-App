//
// Created by user on 1/15/20.
//

#ifndef BOOST_ECHO_CLIENT_ENCODERDECODER_H
#define BOOST_ECHO_CLIENT_ENCODERDECODER_H


#include "EladErezFrame.h"

class EncoderDecoder {
public:
     static EladErezFrame encode (string);
     static string decode (EladErezFrame);
};


#endif //BOOST_ECHO_CLIENT_ENCODERDECODER_H
