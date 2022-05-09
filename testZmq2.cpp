//
// Suicidal Snail
//
// Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>
#include <zmqpp/zmqpp.hpp>
#include <sstream>
#include <iostream>

int receive_cnt = 0, publish_cnt =0;

// ---------------------------------------------------------------------
// This is our subscriber
// It connects to the publisher and subscribes to everything. It
// sleeps for a short time between messages to simulate doing too
// much work. If a message is more than 1 second late, it croaks.

#define MAX_ALLOWED_DELAY 1000 // msecs

static void *
subscriber (void *args) {
    zmqpp::context context;

    // Subscribe to everything
    zmqpp::socket_type type = zmqpp::socket_type::sub;
    zmqpp::socket subscriber(context, type);
    subscriber.connect("tcp://localhost:5556");
    subscriber.set(zmqpp::socket_option::subscribe, "", 0);

    // std::stringstream ss;
    // Get and process messages
    while (1) {
        // ss.clear();
        bool rc;

        //  Process any waiting weather updates
        do {
            std::string update;
            if ((rc = subscriber.receive(update, false)) == true) {
                //  process weather update
                // ss.str(update);
                std::cout << "receiver: " << receive_cnt << std::endl;
                receive_cnt++;
            }
        } while(rc == true);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000000));

    }
    return (NULL);
}


// ---------------------------------------------------------------------
// This is our server task
// It publishes a time-stamped message to its pub socket every 1ms.

static void *
publisher (void *args) {
    zmqpp::context context;

    // Prepare publisher
    zmqpp::socket_type type = zmqpp::socket_type::pub;
    zmqpp::socket_t publisher(context, type);
    publisher.bind("tcp://*:5556");

    while (1) {
        // ss.clear();
        bool rc;

        //  Process any waiting weather updates
        do {
            std::string update("test msg");
            if ((rc = publisher.send(update, false)) == true) {
                //  process weather update
                // ss.str(update);
                publish_cnt++;
                std::cout << "publisher: " << publish_cnt << std::endl;
            }
        } while(rc == true);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000000));

    }
    return 0;
}


// This main thread simply starts a client, and a server, and then
// waits for the client to croak.
//
int main (void)
{
    pthread_t server_thread;
    pthread_create (&server_thread, NULL, publisher, NULL);

    pthread_t client_thread;
    pthread_create (&client_thread, NULL, subscriber, NULL);
    pthread_join (client_thread, NULL);

    std::cout << "pub " << publish_cnt << " vs sub" << receive_cnt << std::endl;

    return 0;
}