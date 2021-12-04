//
//  lec-05-prg-08-pub-sub-and-pull-push-client-v2.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
//

#include <iostream>
#include <string>
#include <stdlib.h> 
# include <unistd.h>
#include <time.h>
#include "zmq.hpp"
using namespace std;

int main(int argc, char *argv[]){
    
    zmq::context_t context (1);
    zmq::socket_t subscriber (context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    subscriber.connect("tcp://localhost:5557");
    zmq::socket_t publisher (context, ZMQ_PUSH);
    publisher.connect("tcp://localhost:5558");   

    string clientID = argv[1];
    //cout << clientID << endl;
    srand ( time(NULL) );
    zmq::pollitem_t items[2] = {
        //{ static_cast<void*>(subscriber), 0, ZMQ_POLLIN, 0 },
        //{ static_cast<void*>(publisher), 0, ZMQ_POLLIN, 0 }
        { &subscriber, 0, ZMQ_POLLIN, 0 },
        { &publisher, 0, ZMQ_POLLIN, 0 }
    };


    while(true){

        
        if (zmq_poll(items, 1, 100)>0 && ZMQ_POLLIN){
        //if (items[0].revents && ZMQ_POLLIN){
            zmq::message_t msg (25);
            subscriber.recv(&msg, 0);
            cout << clientID << ": received message: " << std::string(static_cast<char*>(msg.data()), msg.size()) << endl;

        }
        else {
            int ran = rand()%101;
            if (ran<10){
                sleep (1);
                string mmsg = "(" + clientID + ": ON)";
        
                zmq::message_t msg (25);
                memcpy ( (void *) msg.data(), &mmsg, 25);
                char* requestmsg = new char [25];
                memcpy (requestmsg, msg.data(), msg.size());
                publisher.send(msg);
                cout << clientID <<": send status - activated " << endl;

            }
            else if  (ran>90){
                sleep(1);
                string mmsg = "(" + clientID + ": OFF)";
                zmq::message_t msg (25);
                memcpy ( (void *) msg.data(), &mmsg, 25);
                char* requestmsg = new char [25];
                memcpy (requestmsg, msg.data(), msg.size());
                publisher.send(msg);
                cout << clientID <<": send status - deactivated " << endl;
            }
           
        }

    }

    return 0;
}
