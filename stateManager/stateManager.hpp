#include <unordered_map>
#include <utility>
#include <thread>
#include <queue>
#include <mutex>
#include <zmqpp/zmqpp.hpp>
#include "basicState.hpp"
#include "queueBuffer.hpp"

namespace simpleStateMachine{
    using transitFunc = bool (*)(void);

    typedef struct transitions_entry_{
        BasicState * to_state=nullptr;
        unsigned int priority=0;
        transitFunc judge_condition=nullptr;

        bool operator()(const transitions_entry & left, const transitions_entry & right){
            return left.priority < right.priority;
        }

    } transitions_entry;

    typedef enum StateManagerStatus_{
        idle,
        loading,
        up
    } StateManagerStatus;

    class StateManager{
        private:
            std::unordered_map<BasicState*, std::priority_queue<transitions_entry, 
                std::vector<transitions_entry>, transitions_entry>> transisions_lists_;
            std::shared_ptr<std::thread> zmq_sub_thread_=nullptr;
            std::shared_ptr<std::thread> zmq_pub_thread_=nullptr;
            std::shared_ptr<std::thread> status_control_thread_=nullptr;
            BasicState* start_point_=nullptr;
            BasicState* exist_point_=nullptr;
            bool system_status_=false;
            StateManagerStatus zmq_status_=StateManagerStatus::idle;
            BasicState* current_state_=nullptr;
            std::shared_ptr<QueueBuffer<std::string>> zmq_sub_buffer_;
            std::shared_ptr<QueueBuffer<std::string>> zmq_pub_buffer_;
            unsigned int zmq_pub_msg_buffer_size_=10;
            unsigned int zmq_sub_msg_buffer_size_=10;
            void * zmq_sub_thread_worker();
            void * zmq_pub_thread_worker();
            void * status_control_worker();
            void add_sub_message(std::string msg);
            bool pop_pub_message(std::string &msg);
            std::mutex status_lock_;
            std::mutex sub_buffer_lock_;
            std::mutex pub_buffer_lock_;

            std::shared_ptr<zmqpp::context> zmq_context_=nullptr;
            std::shared_ptr<zmqpp::socket> zmq_sub_=nullptr;
            std::shared_ptr<zmqpp::socket> zmq_pub_=nullptr;

        public:
            StateManager(): zmq_status_(StateManagerStatus::idle){}
            ~StateManager();

            void start_up_manager(std::string file_name);
            void register_transistion_func(BasicState* from_state, BasicState* to_state, uint32_t priority, transitFunc transit_func);
            void unregister_transistion_func(BasicState* from_state, BasicState* to_state);
            bool pop_sub_message(std::string &msg);
            void add_pub_message(std::string msg);
            unsigned int get_manager_status(bool status);

            static StateManager * get_manager(){
                static StateManager state_manager;
                return &state_manager;
            }

    };
}