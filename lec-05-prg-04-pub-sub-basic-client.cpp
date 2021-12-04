//
//  lec-05-prg-04-pub-sub-basic-client.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
//

#include <iostream>
#include <string>
#include "zmq.hpp"
using namespace std;

int main(int argc, char *argv[]){
    // Socket to talk to server
    zmq::context_t context (1);
    cout << "Connecting updates from weather server .. " << endl;
    
    zmq::socket_t socket (context, ZMQ_SUB);
    socket.connect("tcp://localhost:5556");
    
    // Subscribe to zipcode, default is NYC, 10001
    const char *filter = ( argc > 1 )? argv [1]: "10001 ";
    socket.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

    // Process 5 updates

    int update_nbr;
    long total_temp = 0;
    for (update_nbr = 0; update_nbr < 100; update_nbr++) {

        zmq::message_t update;
        int zipcode, temperature, relhumidity;

        socket.recv(&update);

        std::istringstream iss(static_cast<char*>(update.data()));
        iss >> zipcode >> temperature >> relhumidity ;

        total_temp += temperature;

        cout<< "Receive temperature for zipcode '"<< filter
          <<"' was "<< temperature <<"F"<< endl;
    }

    cout<< "Average temperature for zipcode '"<< filter
          <<"' was "<<(int) (total_temp / update_nbr) <<"F"<< endl;

    
}
