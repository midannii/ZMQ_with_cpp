//
//  lec-05-prg-09-dealer-router-async-server.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
// 참고: https://makersweb.net/opensource/19596

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include "zmq.hpp"
#include "zhelpers.hpp"
using namespace std;

class server_worker {
public:
    server_worker(zmq::context_t &ctx, int n, int sock_type)
        : ctx_(ctx),
        my_id(n),
          worker_(ctx_, sock_type)
    {}
 
    void run() {
            worker_.connect("inproc://backend");
            cout << "Worker #" << my_id << " started"<< endl;
 
            while (true) {
                zmq::message_t identity;
                zmq::message_t msg;
                zmq::message_t copied_id;
                zmq::message_t copied_msg;
                worker_.recv(&identity);
                worker_.recv(&msg);
                cout << "Worker #" << my_id << " recived "<<  std::string(static_cast<char*>(msg.data()), msg.size()) << " from " << std::string(static_cast<char*>(identity.data()), identity.size()) << endl;
                copied_id.copy(&identity);
                copied_msg.copy(&msg);
                int replies = within(5); // from base code: http://zguide.wikidot.com/cpp:asyncsrv
                for (int reply = 0; reply < replies; ++reply) {
                    s_sleep(within(1000) + 1);
                    copied_id.copy(&identity);
                    copied_msg.copy(&msg);
                    worker_.send(identity, ZMQ_SNDMORE);
                    worker_.send(msg);
                    worker_.send(copied_id , ZMQ_SNDMORE);
                    worker_.send(copied_msg);
                }

                //worker_.send(copied_id, ZMQ_SNDMORE);
                //worker_.send(identity, ZMQ_SNDMORE);
                //worker_.send(copied_msg);
                //worker_.send(msg, ZMQ_SNDMORE);
                //worker_.send(msg);
                //worker_.send(identity);
                
            }
    }
 
private:
    zmq::context_t &ctx_;
    zmq::socket_t worker_;
    int my_id;
};

class server_task {
public:
    server_task(int n)
        : ctx_(1),
          frontend_(ctx_, ZMQ_ROUTER),
          backend_(ctx_, ZMQ_DEALER)
    {}
 
    //enum { kMaxThread = 5 };
 
    void run(int n) {
        int num_server = n;
        frontend_.bind("tcp://*:5570");
        backend_.bind("inproc://backend");
        std::vector<server_worker *> workers;
        //std::vector<std::thread *> worker_thread;

        for (int i = 0; i < num_server; ++i) {
            // worker = new server_worker(ctx_,i ,ZMQ_DEALER)
            server_worker worker(ctx_,i ,ZMQ_DEALER);
            worker.run();
            workers.push_back(&worker);

            //worker_thread.push_back(new std::thread(std::bind(&server_worker::work, worker)));
            // worker_thread->detach();
        }
        try {

             zmq_proxy(&frontend_, &backend_, nullptr);
             //zmq_proxy(static_cast<void*>(frontend), static_cast<void*>(backend), nullptr);
        }
        catch (std::exception &e) {}
 
    }
 
private:
    zmq::context_t ctx_;
    zmq::socket_t frontend_;
    zmq::socket_t backend_;
    
};




int main(int argc, char *argv[]){
    
    int n = argc; 
    server_task st1(n); 
    //st1.run();
    std::thread t1(&server_task::run, &n);

    t1.join();
    t1.detach();
    
    return 0; 
    
}
