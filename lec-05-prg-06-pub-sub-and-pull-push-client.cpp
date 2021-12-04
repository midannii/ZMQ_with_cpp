//
//  lec-05-prg-06-pub-sub-and-pull-push-client.cpp
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
    
    zmq::context_t context (1);
    zmq::socket_t subscriber (context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    subscriber.connect("tcp://localhost:5557");
    zmq::socket_t publisher (context, ZMQ_PUSH);
    publisher.connect("tcp://localhost:5558");

    srand ( time(NULL) );

zmq::pollitem_t items[] = {
   { &subscriber, 0, ZMQ_POLLIN, 0 },
    { &publisher, 0, ZMQ_POLLIN, 0 }}
 ;

   while (true){
        if (zmq_poll(items, 1, 100)>0 && ZMQ_POLLIN){
   
            zmq::message_t msg (5);
            subscriber.recv(&msg, 0);
            cout << "I: received message: " << std::string(static_cast<char*>(msg.data()), msg.size()) << endl;

        }
        else {
            int ran = rand()%101;
            if (ran<10){
                sleep(1);
                string mmsg = to_string(ran);
                zmq::message_t msg (5);
                memcpy ( (void *) msg.data(), &mmsg, 5);
                char* requestmsg = new char [5];
                memcpy (requestmsg, msg.data(), msg.size());
                publisher.send(msg);
                cout << "I: sending message " << mmsg <<  endl;

            }
        }
    
  }
  return 0;
}

