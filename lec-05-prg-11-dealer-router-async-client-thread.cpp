//
//  lec-05-prg-11-dealer-router-async-client-thread.cpp
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
          socket_ (ctx_, ZMQ_DEALER),
          my_id(id)
    {}
    
    void recvHandler(){


        zmq::pollitem_t items[] = {
            { &socket_, 0, ZMQ_POLLIN, 0 } };

        zmq::message_t msg;
        while (true){
            // sockets = dict(self.poll.poll(1000))
            //if self.socket in sockets:
            //    msg = self.socket.recv()
            //    print('{0} received: {1}'.format(self.identity, msg))
            if (zmq_poll(items, 1, 100)>0 && ZMQ_POLLIN) {
                socket_.recv(&msg,0);
                    cout << my_id << " received: " 
                        << std::string(static_cast<char*>(msg.data()), msg.size()) << endl;
                }
            }

        }


    void start() {

        char identity[10];
        //std::sprintf(identity, "%d", my_id);
        strcpy(identity, my_id.c_str());
        socket_.connect("tcp://localhost:5570");

        cout << "Client " << identity << " started " << endl;
 
        zmq::pollitem_t items[] = {
            { &socket_, 0, ZMQ_POLLIN, 0 } };
        
        
        thread clientThread(&client_task::recvHandler);
        clientThread.join();

        int reqs = 0;
            while (true) {

                reqs++;

                cout << "Req " << reqs << " sent ..." << endl;

                string mmsg = "request " + to_string(reqs);
                zmq::message_t msg (25);
                memcpy ( (void *) msg.data(), &mmsg, 25);
                char* requestmsg = new char [25];
                memcpy (requestmsg, msg.data(), msg.size());
                socket_.send(msg);

                sleep(1);
            }
            // socket_.close();
        }

 
private:
    zmq::context_t ctx_;
    zmq::socket_t socket_;
    string my_id; 
};

int main(int argc, char *argv[]){
    string id = argv[1];
    client_task ct1(id); 
    ct1.start();
    return 0;
}
