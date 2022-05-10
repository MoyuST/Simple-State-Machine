// Client side C/C++ program to demonstrate Socket
// programming
// adapted from https://www.geeksforgeeks.org/socket-programming-cc/
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "simpleStateMachine.hpp"

#define PORT 8080

#define SIMPLELOGEXAMPLE(STATUS, MSG)                                                   \
    do{                                                                                 \
    auto cur_time = std::chrono::system_clock::now();                                   \
    std::time_t cur_time_t = std::chrono::system_clock::to_time_t(cur_time);            \
        std::cout << "[example_socket][" << strtok(std::ctime(&cur_time_t), "\n") << "][" \
        << #STATUS << "] " << MSG << std::endl;                                         \
    } while(false)

int main(int argc, char const* argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		SIMPLELOGEXAMPLE(ERROR, "Socket creation error");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		SIMPLELOGEXAMPLE(ERROR, 
			"Invalid address/ Address not supported");
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr))
		< 0) {
		SIMPLELOGEXAMPLE(ERROR, "Connection Failed");
		return -1;
	}

    auto state1 = std::shared_ptr<simpleStateMachine::BasicState>(new simpleStateMachine::BasicState("state1", true, false, [](){
        SIMPLELOGEXAMPLE(NORMAL, "state1 looping");
    }));

    auto state2 = std::shared_ptr<simpleStateMachine::BasicState>(new simpleStateMachine::BasicState("state2", false, false, [](){
        SIMPLELOGEXAMPLE(NORMAL, "state2 looping");
    }));

    auto state3 = std::shared_ptr<simpleStateMachine::BasicState>(new simpleStateMachine::BasicState("state3", false, false, [](){
        SIMPLELOGEXAMPLE(NORMAL, "state3 looping");
    }));


    STATEMANAGER->register_transistion_func(state1, state2, 1, [&]()->bool{
        std::string request = "request from state1";
        send(sock, request.c_str(), strlen(request.c_str()), 0);
        SIMPLELOGEXAMPLE(NORMAL, "state1_request");
        valread = read(sock, buffer, 1024);
        std::string buffer_str(buffer);

        if(buffer_str=="1->2"){
            return true;
        }
        else{
            return false;
        }
    });

    STATEMANAGER->register_transistion_func(state2, state3, 1, [&]()->bool{
        std::string request = "request from state2";
        send(sock, request.c_str(), strlen(request.c_str()), 0);
        SIMPLELOGEXAMPLE(NORMAL, "state2_request");
        valread = read(sock, buffer, 1024);
        std::string buffer_str(buffer);

        if(buffer_str=="2->3"){
            return true;
        }
        else{
            return false;
        }
    });

    STATEMANAGER->register_transistion_func(state3, state1, 1, [&]()->bool{
        std::string request = "request from state3";
        send(sock, request.c_str(), strlen(request.c_str()), 0);
        SIMPLELOGEXAMPLE(NORMAL, "state3_request");
        valread = read(sock, buffer, 1024);
        std::string buffer_str(buffer);

        if(buffer_str=="3->1"){
            return true;
        }
        else{
            return false;
        }
    });

    STATEMANAGER->start_up_manager("../config/service.json");

    STATEMANAGER->spin();

    return 0;
}
