#include <zmqpp/zmqpp.hpp>
#include <chrono>
#include <thread>
#include <stdio.h>

int main (int argc, char *argv[])
{
    //  Prepare our context and sockets
    zmqpp::context context;

    //  Connect to task ventilator
    zmqpp::socket_type type = zmqpp::socket_type::pull;
    zmqpp::socket receiver (context, type);
    receiver.connect("tcp://localhost:5557");
    printf("receiver established!\n");

    //  Connect to weather server
    type = zmqpp::socket_type::sub;
    zmqpp::socket subscriber(context, type);
    subscriber.connect("tcp://localhost:5556");
    subscriber.set(zmqpp::socket_option::subscribe, "10001 ", 6);
    printf("publisher established!\n");

    //  Process messages from both sockets
    //  We prioritize traffic from the task ventilator
    while (1) {
    	printf("enter waiting loop\n");
        //  Process any waiting tasks
        bool rc;
        do {
        	zmqpp::message task;
            if ((rc = receiver.receive(task, ZMQ_DONTWAIT)) == true) {
                //  process task
            }
        } while(rc == true);
        
        //  Process any waiting weather updates
        do {
            zmqpp::message update;
            if ((rc = subscriber.receive(update, ZMQ_DONTWAIT)) == true) {
                //  process weather update

            }
        } while(rc == true);
        
        //  No activity, so sleep for 1 msec
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}