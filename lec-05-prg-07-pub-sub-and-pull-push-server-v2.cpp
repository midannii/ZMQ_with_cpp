//
//  lec-05-prg-07-pub-sub-and-pull-push-server-v2.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
//

#include <iostream>
#include <string>
#include "zmq.hpp"
using namespace std;

int main(){
    
        //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    publisher.bind ("tcp://*:5557");
    zmq::socket_t collector (context, ZMQ_PULL);
    collector.bind ("tcp://*:5558");
    
    while (true) {

        zmq::message_t msg (25);
        collector.recv(&msg, 0);
        cout << "server: publishing update => " << std::string(static_cast<char*>(msg.data()), msg.size()) << endl;

        publisher.send (msg);
    }
    return 0;

    
}
