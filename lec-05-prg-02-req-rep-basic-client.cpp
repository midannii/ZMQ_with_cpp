//
//  lec-05-prg-02-req-rep-basic-client.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
//

#include <iostream>
#include <string>
# include <unistd.h>
#include <stdlib.h> 
#include <time.h>
#include "zmq.hpp"
using namespace std;

int main () {
    
    int request;
    for (request=0; request<10; request++){
        cout << "Sending request " << request << " ..." << endl;
        
        zmq::message_t req (5);
        memcpy ( (void *) req.data(), "hello", 5);
        //zmq::message_t req((void*)"Hello", 5, NULL);
        char* reqmsg = new char [5];
        memcpy (reqmsg, req.data(), req.size());
        socket.send (req);

        
        // get the reply
        zmq::message_t reply(5);
        socket.recv(&reply, 0);
        
        //cout << "Received reply " << request << "[" << reply.data() << "]" <<endl;
        cout << "Received reply " << request << "[" << std::string(static_cast<char*>(reply.data()), reply.size())<< "]" <<endl;
    }
}
