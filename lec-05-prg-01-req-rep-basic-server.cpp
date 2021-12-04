//
//  lec-05-prg-01-req-rep-basic-server.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
// ref: https://makersweb.net/opensource/19422


#include <iostream>
#include <string>
# include <unistd.h>
#include "zmq.hpp"
using namespace std;

int main () {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t request(5);

        //  Wait for next request from client
        socket.recv (&request, 0);
        cout << "Received request: " << std::string(static_cast<char*>(request.data()), request.size()) << endl;

        //  Do some 'work'
        sleep (1);

        //  Send reply back to client
        //zmq::message_t reply(5);
        //memcpy ((void *) reply.data (), "World", 5);
        zmq::message_t reply (5);
        memcpy ( (void *) reply.data(), "World", 5);
        char* requestmsg = new char [5];
        memcpy (requestmsg, reply.data(), reply.size());
        
        socket.send (reply);
    }
    return 0;
}
