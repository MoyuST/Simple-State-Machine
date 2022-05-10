// Server side C/C++ program to demonstrate Socket
// programming
// adapted from https://www.geeksforgeeks.org/socket-programming-cc/
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <ctime> 
#include <chrono>

#define SIMPLELOG(STATUS, MSG)                                                          \
    do{                                                                                 \
    auto cur_time = std::chrono::system_clock::now();                                   \
    std::time_t cur_time_t = std::chrono::system_clock::to_time_t(cur_time);            \
        std::cout << "[example_socket_helper][" << strtok(std::ctime(&cur_time_t), "\n") << "][" \
        << #STATUS << "] " << MSG << std::endl;                                         \
    } while(false)

#define PORT 8080
int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
		== 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	
	if ((new_socket = 
		accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen))< 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

    while(1){

        valread = read(new_socket, buffer, 1024);
        auto buffer_str = std::string(buffer);
        
        std::string reply;

        if(buffer_str=="request from state1"){
            reply = "1->2";
        }
        else if(buffer_str=="request from state2"){
            reply = "2->3";
        }
        else if(buffer_str=="request from state3"){
            reply = "3->1";
        }

        send(new_socket, reply.c_str(), strlen(reply.c_str()), 0);
        std::string msg = "sending "+reply;
		SIMPLELOG(NORMAL, msg);
    }
    
	return 0;
}
