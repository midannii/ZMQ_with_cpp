//
//  lec-05-prg-10-dealer-router-async-client.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
//

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include "zmq.hpp"
#include "zmq.h"
#include "zhelpers.hpp"
using namespace std;

class client_task {
public:
    client_task(string id )
        : ctx_(1),
          client_socket_(ctx_, ZMQ_DEALER),
          my_id(id)
    {}
 
    void start() {
        // generate random identity
        // string identity = 'u'+my_id;
        char identity[10];
        strcpy(identity, my_id.c_str());
        //sprintf(identity, "%04X-%04X", within(0x10000), within(0x10000));
        //printf("%s\n", identity);
        //client_socket_.setsockopt(ZMQ_IDENTITY, identity, strlen(identity));
        client_socket_.setsockopt(ZMQ_IDENTITY, identity, strlen(identity));//identity.size());
        client_socket_.connect("tcp://localhost:5570");
        cout << "Client " << identity << " started " << endl;
 
        zmq::pollitem_t items[] = {
       //     { static_cast<void*>(client_socket_), 0, ZMQ_POLLIN, 0 } };
            { &client_socket_, 0, ZMQ_POLLIN, 0 } };
        
        
        int request_nbr = 0;
            while (true) {

                request_nbr++;
                cout << "Req " << request_nbr << " sent ..." << endl;
                string mmsg = "request " + to_string(request_nbr);
                zmq::message_t msg (25);
                memcpy ( (void *) msg.data(), &mmsg, 25);
                char* requestmsg = new char [25];
                memcpy (requestmsg, msg.data(), msg.size());
                client_socket_.send(msg);
                sleep(1);

                if (zmq_poll(items, 1, 100)>0 && ZMQ_POLLIN){
                    
                    client_socket_.recv(&msg,0);
                    cout << identity << " received: " 
                        << std::string(static_cast<char*>(msg.data()), msg.size()) << endl;
                }

            }
        }

 
private:
    zmq::context_t ctx_;
    zmq::socket_t client_socket_;
    string my_id; 
};

int main(int argc, char *argv[]){
    string id = argv[1];
    client_task ct1(id); 
    ct1.start();
    return 0;
}
