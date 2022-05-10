#ifndef SIMPLE_STATE_MACHINE_STATE_MANAGER
#define SIMPLE_STATE_MACHINE_STATE_MANAGER
#include <unordered_map>
#include <utility>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <signal.h>
#include "basicState.hpp"
#include "queueBuffer.hpp"
#include "heapVec.hpp"
#include "json.hpp"
#include "simpleLogger.hpp"

#ifdef USE_ZMQPP
#include <zmqpp/zmqpp.hpp>
#endif

#define STATEMANAGER simpleStateMachine::StateManager::get_manager()

namespace simpleStateMachine{
    using transitFunc = std::function<bool(void)>;

    typedef struct transitions_entry_{
        std::shared_ptr<BasicState> from_state=nullptr;
        std::shared_ptr<BasicState> to_state=nullptr;
        unsigned int priority=0;
        transitFunc judge_condition=nullptr;

        bool operator()(const std::shared_ptr<transitions_entry_> & left, 
            const std::shared_ptr<transitions_entry_> & right){

            return left->priority > right->priority;
        }

    } transitions_entry;

    typedef enum StateManagerStatus_{
        idle,
        loading,
        up,
        failed
    } StateManagerStatus;

    class StateManager{
        private:
            ////////// register info /////
            std::unordered_map<unsigned int, 
                HeapVec<std::shared_ptr<transitions_entry>, transitions_entry>> transitions_lists_;
            std::unordered_map<std::string, std::shared_ptr<transitions_entry>>
                state_transition_list_;
            std::string concate_state_id(std::shared_ptr<BasicState> state1, std::shared_ptr<BasicState> state2);
            void register_state(std::shared_ptr<BasicState> state);
            std::unordered_map<unsigned int, std::shared_ptr<BasicState>> id_to_state_;
            std::unordered_map<std::shared_ptr<BasicState>, unsigned int> state_to_id_;
            //////////////////////////////

            std::shared_ptr<BasicState> start_point_=nullptr;
            std::shared_ptr<BasicState> exist_point_=nullptr;
            std::shared_ptr<std::thread> status_control_thread_=nullptr;

#ifdef USE_ZMQPP
            std::shared_ptr<std::thread> zmq_sub_thread_=nullptr;
            std::shared_ptr<std::thread> zmq_pub_thread_=nullptr;
            std::shared_ptr<QueueBuffer<std::string>> zmq_sub_buffer_;
            std::shared_ptr<QueueBuffer<std::string>> zmq_pub_buffer_;
            unsigned int zmq_pub_msg_buffer_size_=10;
            unsigned int zmq_sub_msg_buffer_size_=10;
            void * zmq_sub_thread_worker();
            void * zmq_pub_thread_worker();
            nlohmann::json system_zmq_info_;
            std::shared_ptr<zmqpp::context> zmq_context_=nullptr;
            std::shared_ptr<zmqpp::socket> zmq_sub_=nullptr;
            std::shared_ptr<zmqpp::socket> zmq_pub_=nullptr;
            void push_sub_message(std::string msg);
            bool pop_pub_message(std::string &msg);
#endif

            inline unsigned int get_id(std::shared_ptr<BasicState> state) { 
                return state_to_id_.count(state)==0 ? 0 : state_to_id_[state];
            }
            StateManagerStatus system_status_=StateManagerStatus::idle;
            std::shared_ptr<BasicState> current_state_=nullptr;
            void * status_control_worker();
            std::mutex status_lock_;
            std::mutex sub_buffer_lock_;
            std::mutex pub_buffer_lock_;
            std::mutex register_info_lock_;

            static void stop_controller(int s){
                SIMPLELOG(NORMAL, "capture SIGINT, terminating program");
                delete get_manager();
                exit(0);
            }

        public:
            StateManager() = default;
            ~StateManager();

            void start_up_manager(std::string file_name);
            void register_transistion_func(std::shared_ptr<BasicState> from_state, std::shared_ptr<BasicState> to_state, uint32_t priority, transitFunc transit_func);
            void unregister_transistion_func(std::shared_ptr<BasicState> from_state, std::shared_ptr<BasicState> to_state);
#ifdef USE_ZMQPP
            bool pop_sub_message(std::string &msg);
            void push_pub_message(std::string msg);
#endif
            inline StateManagerStatus get_manager_status(){
                return system_status_;
            }

#ifdef USE_ZMQPP
            inline nlohmann::json system_zmq_info(){
                return system_zmq_info_;
            }
#endif

            static StateManager * & get_manager(){
                static StateManager* state_manager = new StateManager();
                return state_manager;
            }

            static void spin(){
                // save previous handler
                sighandler_t previous_handler = signal(SIGINT, stop_controller);
                while(1){

                }
                // recover previous handler
                signal(SIGINT, previous_handler);
            }

    };
}
#endif