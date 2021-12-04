//
//  lec-05-prg-03-pub-sub-basic-server.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
//

#include <iostream>
#include <string>
#include <stdlib.h> // for std::rand() and std::srand()
#include "zmq.hpp"
using namespace std;

int main(){

    cout << "Publishing updates at weather server..." << endl;
    
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_PUB);
    socket.bind ("tcp://*:5556");

    while (true) {

        int zipcode =  rand()%100001;
        int temperature = rand()%216 - 80; 
        int relhumidity = rand()%51 + 10;

        //  publish to client 
        zmq::message_t msg (25);
        snprintf ((char *) msg.data(), 20 ,
            "%05d %d %d", zipcode, temperature, relhumidity);
        
        socket.send (msg);
    }
    return 0;
    
}