//
// Suicidal Snail
//
// Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>
// adapted from zmq tutorial
#include <zmqpp/zmqpp.hpp>
#include <sstream>
#include <iostream>
#include <vector>

#define SIMPLELOG(STATUS, MSG)                                                          \
    do{                                                                                 \
    auto cur_time = std::chrono::system_clock::now();                                   \
    std::time_t cur_time_t = std::chrono::system_clock::to_time_t(cur_time);            \
        std::cout << "[example_zmq_helper][" << strtok(std::ctime(&cur_time_t), "\n") << "][" \
        << #STATUS << "] " << MSG << std::endl;                                         \
    } while(false)

int receive_cnt = 0, publish_cnt =0;

static void *
publisher (void *args) {
    static std::vector<std::string> command_list{
        "{\"enter\":\"1->2\"}",
        "{\"enter\":\"2->3\"}",
        "{\"enter\":\"3->1\"}"
    };

    zmqpp::context context;

    // Prepare publisher
    zmqpp::socket_type type = zmqpp::socket_type::pub;
    zmqpp::socket_t publisher(context, type);
    publisher.bind("tcp://*:7777");

    int cnt = 0;

    while (1) {
        // ss.clear();
        bool rc;

        //  Process any waiting weather updates
        do {
            cnt = (cnt+1)%3;
            std::string update = command_list[cnt];
            if ((rc = publisher.send(update, false)) == true) {
                SIMPLELOG(NORMAL, ("sending "+update));
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        } while(rc == true);

    }
    return 0;
}

int main (void)
{
    pthread_t server_thread;
    pthread_create (&server_thread, NULL, publisher, NULL);
    pthread_join (server_thread, NULL);

    return 0;
}