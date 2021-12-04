//
//  lec-05-prg-12-p2p-dechat.cpp
//  ZMQ
//
//  Created by 심미단 on 2021/11/13.
//

#include <iostream>
#include <string>
# include <unistd.h>
#include <stdlib.h> 
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include <thread>
#include<unistd.h>
#include<arpa/inet.h>
#include "zmq.hpp"
 #include "zhelpers.hpp"
using namespace std;

bool global_flag_shutdown = true;
std::string mmsg;

/*std::vector<std::string> split(std::string s, std::string divid) {
	std::vector<std::string> v;

	char* c = strtok((char*)s.c_str(), divid.c_str());
	while (c) {
		v.push_back(c);
		c = strtok(NULL, divid.c_str());
	}
	return v;
}*/

vector<string> split(string input, string delimiter) {
    vector<string> answer;
    stringstream ss(input);
    string temp;
    char* delimiter2; 
    std::strcpy(delimiter2, delimiter.c_str());
    //while (getline(ss, temp, delimiter2)) {
    while (getline(ss, temp)) {
        answer.push_back(temp);
    }
 
    return answer;
}

std::string search_nameserver(std::string ip_mask, std::string local_ip_addr, int port_nameserver){

    zmq::context_t context (1);
    zmq::socket_t req (context, ZMQ_SUB);
    for (int last=1; last<255; last++){
        std::string target_ip_addr = "tcp://" + ip_mask + "."+ std::to_string(last) + ":"+ std::to_string(port_nameserver);
        if (target_ip_addr != local_ip_addr || target_ip_addr == local_ip_addr){
            req.connect(target_ip_addr);}
        req.setsockopt(ZMQ_RCVTIMEO, "NAMESERVER", 2000); // 이게 맞나 ??
        }
    try{
        std::string res; 
        req.recv(&res, 0);
        std::vector<std::string> res_list = split(res, ':');
        if (res_list[0] == "NAMESERVER"){
            return res_list[1];
        }
        else{
            return " ";
        };
    } catch (int err){
        return " ";
    };

    return " ";
}


void beacon_nameserver(std::string local_ip_addr, int port_nameserver){

    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_PUB);
    std::string temp = "tcp://"+local_ip_addr+std::to_string(port_nameserver);
    socket.bind(temp);
    std::cout << "local p2p name server bind to tcp://" << local_ip_addr << ":" << port_nameserver << std::endl;
    while(true){
        try {
            sleep(1);
            mmsg = "NAMESERVER:" +local_ip_addr;
            zmq::message_t msg (25);
                memcpy ( (void *) msg.data(), &mmsg, 25);
                char* requestmsg = new char [25];
                memcpy (requestmsg, msg.data(), msg.size());
                socket.send(msg);
        }
        catch(int err){
            break;
        }
    }
}

void user_manager_nameserver(std::string local_ip_addr, std::string port_subscribe){
    std::vector<std::vector<std::string> > user_db; 
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    std::string temp = "tcp://"+local_ip_addr+":"+port_subscribe;
    socket.bind(temp);
    std::cout << "local p2p db server activated at tcp://" << local_ip_addr << ":"<<port_subscribe << std::endl;
    while(true){
        try{
            zmq::message_t msg (25);
            socket.recv(&msg,0);
            mmsg = std::string(static_cast<char*>(msg.data()), msg.size());
            std::vector<std::string> user_req = split(mmsg, ":");
            user_db.push_back(user_req);
            std::cout << "user registration " << user_req[0] <<  " from " << user_req[1] << std::endl;

            mmsg = "ok";
                zmq::message_t msg2 (25);
                memcpy ( (void *) msg2.data(), &mmsg, 25);
                char* requestmsg = new char [25];
                memcpy (requestmsg, msg2.data(), msg2.size());
                socket.send(msg2);
        }
        catch(int err){
            break;
        }

    }
}

void relay_server_nameserver(std::string local_ip_addr, std::string port_chat_publisher, std::string port_chat_collector){
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    std::string temp = "tcp://"+local_ip_addr + ":" + port_chat_publisher; 
    publisher.bind(temp);
    zmq::socket_t collector (context, ZMQ_PULL);
    std::string temp2 = "tcp://"+local_ip_addr + ":" + port_chat_collector; 
    collector.bind(temp2);
    std::cout << "local p2p relay server activated at tcp://" << local_ip_addr << ":" << port_chat_publisher << "&" <<  port_chat_collector << std::endl;

    while(true){
        try{
            zmq::message_t msg (25);
            collector.recv(&msg,0);
            mmsg = std::string(static_cast<char*>(msg.data()), msg.size());
            std::cout << "p2p-relay: <==>" << mmsg << std::endl;
            zmq::message_t msg2 (25);
                memcpy ( (void *) msg2.data(), &mmsg, 25);
                char* requestmsg = new char [25];
                memcpy (requestmsg, msg2.data(), msg2.size());
                publisher.send(msg2);
        }
        catch(int err){
            break;
        }
    }
}

std::string get_local_ip(){
        int socketserver =  socket(AF_INET, SOCK_DGRAM, 0);
        int option = 1;
        setsocketspt(socketserver, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
     
     try{   //sock.connect("8.8.8.8");
     int conn = connect(sock, "8.8.8.8");
        return to_string(sock.getsockopt());
    }
    catch(int err){
        try{
            return to_string(socket::gethostname(sock));
        }
        catch(int err2){
            return "127.0.0.1";
        }
    }

//        sock.close();


}

int main(int argc, char *argv[]){
    
    if (strlen(argv[1])>1){

        std::cout << "starting p2p chatting program." << std::endl;

    int port_nameserver = 9001;
    int port_chat_publisher = 9002;
    int port_chat_collector = 9003;
    int port_subscribe = 9004;
    std::string ip_addr_p2p_server = "";
    std::string user_name = argv[1];

    //ip_addr = get_local_ip();
    std::string ip_addr = "127.0.0.1";
    //ip_mask = ip_addr.rsplit('.', 1)[0];
    std::string ip_mask = "1";

    std::cout << "Searching for P2P server ... " << std::endl;

    std::string name_server_ip_addr = search_nameserver(ip_mask, ip_addr, port_nameserver); /////
    if (name_server_ip_addr == " "){
        std::string ip_addr_p2p_server = ip_addr; 
        // ip_addr_p2p_server = ip_addr; 
        std::cout << "p2p server is not found, and p2p server mode is activated." << std::endl;
        //thread beacon_thread, db_thread, relay_thread; 
        //beacon_thread = Thread(target=beacon_nameserver, args=(ip_addr, port_nameserver,))
        //thread beacon_thread(beacon_nameserver, ip_addr, port_nameserver);
        std::thread beacon_thread(&beacon_nameserver, &ip_addr, &port_nameserver);
        
        //beacon_thread.start()
        beacon_thread.join();
        std::cout << "p2p beacon server is activated." << std::endl;
        //db_thread = Thread(target=user_manager_nameserver, args=(ip_addr, port_subscribe, ))
        std::thread db_thread(&user_manager_nameserver, &ip_addr, &port_subscribe);
        //db_thread.start()
        db_thread.join();
        std::cout << "p2p subsciber database server is activated." << std::endl;
        // relay_thread = Thread(target=relay_server_nameserver, args=(ip_addr, port_chat_publisher, port_chat_collector, ))
        thread relay_thread(relay_server_nameserver, ip_addr, port_chat_publisher, port_chat_collector);
        // relay_thread.start()
        relay_thread.join();
        std::cout << "p2p message relay server is activated." << std::endl;
    }
    else{
        // ip_addr_p2p_server = name_server_ip_addr
        std::string ip_addr_p2p_server = name_server_ip_addr;
        std::cout << "p2p server found at " << ip_addr_p2p_server << " and p2p client mode is activated." << std::endl;
    }

    std::cout << "starting user registration procedure." << std::endl; 


    //db_client_context = zmq.Context()
    zmq::context_t db_client_context (1);
    //db_client_socket = db_client_context.socket(zmq.REQ)
    zmq::socket_t db_client_socket (db_client_context, ZMQ_REQ);
    //db_client_socket.connect("tcp://{0}:{1}".format(ip_addr_p2p_server, port_subscribe))
    std::string temp = "tcp://"+ip_addr_p2p_server+":"+std::to_string(port_subscribe);
    db_client_socket.connect(temp);
    //db_client_socket.send_string("{0}:{1}".format(ip_addr, user_name))
    zmq::message_t msg (25);
    mmsg = ip_addr + ":"+user_name ; 
    memcpy ( (void *) msg.data(), &mmsg, 25);
    char* requestmsg = new char [25];
    memcpy (requestmsg, msg.data(), msg.size());
   db_client_socket.send(msg);

    sleep(1);
    db_client_socket.recv(&msg, 0);
    mmsg = std::string(static_cast<char*>(msg.data()), msg.size());
    if (mmsg == "ok")
        std::cout << "user registration to p2p server completed." << std::endl;
    else
        std::cout << "user registration to p2p server failed." << std::endl;

    std::cout << "starting message transfer procedure." << std::endl;

    //relay_client = zmq.Context()
    zmq::context_t relay_client (1);
    //p2p_rx = relay_client.socket(zmq.SUB)
    zmq::socket_t p2p_rx (relay_client, ZMQ_SUB);
    //p2p_rx.setsockopt(zmq.SUBSCRIBE, b'RELAY')
    p2p_rx.setsockopt(ZMQ_SUBSCRIBE, "RELAY");
    //p2p_rx.connect("tcp://{0}:{1}".format(ip_addr_p2p_server, port_chat_publisher))
    std::string temp2 = "tcp://"+ip_addr_p2p_server+":"+to_string(port_chat_publisher);
    p2p_rx.connect(temp2);
    //p2p_tx = relay_client.socket(zmq.PUSH)
    zmq::socket_t p2p_tx (relay_client, ZMQ_PUSH);
    //p2p_tx.connect("tcp://{0}:{1}".format(ip_addr_p2p_server, port_chat_collector))
    std::string temp3 = "tcp://"+ip_addr_p2p_server+":"+to_string(port_chat_collector);
    p2p_tx.connect(temp3);

    std::cout << "starting autonomous message transmit and receive scenario." << std::endl;

    zmq::pollitem_t items[2] = {
        { &p2p_rx, 0, ZMQ_POLLIN, 0 }
    };

    while (true){
        try{
        //if p2p_rx.poll(100) & zmq.POLLIN:
                //message = p2p_rx.recv_string()
                //std::cout << "p2p-recv::<<== " << message.split(":")[1] << ": " << message.split(":")[2] << std::endl;
        if (zmq_poll(items, 1, 100)>0 && ZMQ_POLLIN){
            zmq::message_t msg (25);
            p2p_rx.recv(&msg, 0);
            std::string message = std::string(static_cast<char*>(msg.data()), msg.size());
            std::vector<std::string> msg_list = split(message, ":");
            std::cout << "p2p-recv::<<== " << msg_list[1] << ": " << msg_list[2] << std::endl;
        }
            
        else{
                //rand = random.randint(1, 100)
                int ran = rand()%101;
                if (ran < 10){
                    sleep(3);
                    mmsg = "(" + user_name + "," + ip_addr + ":ON)";
                    memcpy ( (void *) msg.data(), &mmsg, 25);
                    char* requestmsg = new char [25];
                    memcpy (requestmsg, msg.data(), msg.size());
                    p2p_tx.send(msg);
                    std::cout << "p2p-send::==>>" <<  msg << std::endl;
                }
                else if (ran > 90){
                    sleep(3);
                    mmsg = "(" + user_name + "," + ip_addr + ":OFF)";
                    memcpy ( (void *) msg.data(), &mmsg, 25);
                    char* requestmsg = new char [25];
                    memcpy (requestmsg, msg.data(), msg.size());
                    p2p_rx.send(msg);
                    
                    std::cout << "p2p-send::==>>" <<  msg << std::endl;
                }
            }
        }
        catch (int err){
            break;
        }
        }

    std::cout << "closing p2p chatting program." << std::endl;

    db_client_context.close();
    p2p_rx.close();
    p2p_tx.close();
    //db_client_context.term();
    //relay_client.term();
    
    //global global_flag_shutdown
    //global_flag_shutdown = True
    //db_client_socket.close(linger=0)
    //p2p_rx.close(linger=0)
    //p2p_tx.close(linger=0)
    //db_client_context.term()
    //relay_client.term()


    }

}
