#include <fstream>
#include <sstream>

#include "stateManager.hpp"
#include "json.hpp"
#include "simpleLogger.hpp"

namespace simpleStateMachine{

    void * StateManager::zmq_sub_thread_worker(){
        while (1) {
            bool rc;

            //  Process any waiting weather updates
            do {
                std::string update;
                if ((rc = zmq_sub_->receive(update, false)) == true) {
                    std::unique_lock<std::mutex> lock(sub_buffer_lock_);
                    zmq_sub_buffer_->push(update);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000000));
            } while(rc == true);
        }
    }

    void * StateManager::zmq_pub_thread_worker(){
        while (1) {
            bool rc;

            do {
                std::unique_lock<std::mutex> lock(pub_buffer_lock_);
                if(!zmq_pub_buffer_->empty()){
                    std::string data = zmq_pub_buffer_->pop();
                    if ((rc = zmq_pub_->send(data, false)) == true) {}
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000000));
            } while(rc == true);
        }
    }

    void StateManager::start_up_manager(std::string file_name){
        std::ifstream t("../config/service.json");
        std::stringstream buffer;
        buffer << t.rdbuf();

        std::string config_string;
        buffer >> config_string;

        nlohmann::json config_json(config_json);

        zmq_context_ = std::shared_ptr<zmqpp::context>(new zmqpp::context());

        if(config_json["zmq"]["sub"]["on"]==true){
            zmq_sub_ = std::shared_ptr<zmqpp::socket>(
                new zmqpp::socket(*zmq_context_, zmqpp::socket_type::sub)
            );
            std::string full_ip;
            std::string ip = config_json["zmq"]["sub"]["ip"];
            unsigned int port = config_json["zmq"]["sub"]["port"];
            full_ip = "tcp://"+ip+std::to_string(port);
            zmq_sub_->connect(full_ip);
            zmq_sub_->set(zmqpp::socket_option::subscribe, "", 0);
            std::string msg("zmq sub("+full_ip+") is up");

            // set up buffer
            unsigned int sub_buffer_size = config_json["zmq"]["sub"]["buffer_size"];
            zmq_sub_buffer_ = std::shared_ptr<QueueBuffer<std::string>>(
                new QueueBuffer<std::string>(sub_buffer_size)
            );

            // set up thread
            zmq_sub_thread_ = std::shared_ptr<std::thread>(new std::thread(&zmq_sub_thread_worker, this));

            SIMPLELOG(NORMAL, msg);
        }

        if(config_json["zmq"]["pub"]["on"]==true){
            zmq_pub_ = std::shared_ptr<zmqpp::socket>(
                new zmqpp::socket(*zmq_context_, zmqpp::socket_type::pub)
            );
            std::string full_ip;
            std::string ip = config_json["zmq"]["pub"]["ip"];
            unsigned int port = config_json["zmq"]["pub"]["port"];
            full_ip = "tcp://"+ip+std::to_string(port);
            zmq_pub_->bind(full_ip);
            std::string msg("zmq pub("+full_ip+") is up");

            // set up buffer
            unsigned int pub_buffer_size = config_json["zmq"]["pub"]["buffer_size"];
            zmq_pub_buffer_ = std::shared_ptr<QueueBuffer<std::string>>(
                new QueueBuffer<std::string>(pub_buffer_size)
            );

            // set up thread
            zmq_pub_thread_ = std::shared_ptr<std::thread>(new std::thread(&zmq_pub_thread_worker, this));

            SIMPLELOG(NORMAL, msg);
        }
    }

    StateManager::~StateManager(){
        // join threads
    }
}