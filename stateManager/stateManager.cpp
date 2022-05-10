#include <fstream>
#include <sstream>
#include <iostream>
#include "stateManager.hpp"

namespace simpleStateMachine{

    void * StateManager::zmq_sub_thread_worker(){
        while (system_status_ == StateManagerStatus::up) {
            bool rc;

            //  Process any waiting weather updates
            do {
                std::string update;
                if ((rc = zmq_sub_->receive(update, false)) == true) {
                    std::unique_lock<std::mutex> lock(sub_buffer_lock_);
                    zmq_sub_buffer_->push(update);
                    system_zmq_info_ = nlohmann::json::parse(update);
                    SIMPLELOG(NORMAL, system_zmq_info_.dump());
                    // std::cout << system_zmq_info_.dump() << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } while(rc == true && system_status_ == StateManagerStatus::up);
        }
        return nullptr;
    }

    void * StateManager::zmq_pub_thread_worker(){
        while (system_status_ == StateManagerStatus::up) {
            bool rc;

            do {
                std::unique_lock<std::mutex> lock(pub_buffer_lock_);
                if(!zmq_pub_buffer_->empty()){
                    std::string data = zmq_pub_buffer_->pop();
                    if ((rc = zmq_pub_->send(data, false)) == true) {}
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } while(rc == true && system_status_ == StateManagerStatus::up);
        }
        return nullptr;
    }

    void * StateManager::status_control_worker(){
        auto transition_check_func = [&](std::shared_ptr<transitions_entry> t)->bool{
            if(t->judge_condition()==true){
                std::string msg = "transition from state("+current_state_->get_descip()+") to state("+t->to_state->get_descip()+")";
                SIMPLELOG(NORMAL, msg);
                current_state_ = t->to_state;
                return true;
            }
            else{
                return false;
            }
        };

        while(system_status_ == StateManagerStatus::up){
            // check whether exist point is reached
            if(current_state_==exist_point_){
                system_status_ = StateManagerStatus::idle;
                break;
            }

            // check transition
            transitions_lists_[get_id(current_state_)].for_each(transition_check_func);

            current_state_->loop();

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return nullptr;
    }

    void StateManager::start_up_manager(std::string file_name){
        system_status_ = StateManagerStatus::loading;
        std::ifstream t(file_name);
        std::stringstream buffer;
        buffer << t.rdbuf();

        std::string config_string(buffer.str());

        nlohmann::json config_json = nlohmann::json::parse(config_string);

        zmq_context_ = std::shared_ptr<zmqpp::context>(new zmqpp::context());

        if(config_json["zmq"]["sub"]["on"].get<bool>()==true){
            zmq_sub_ = std::shared_ptr<zmqpp::socket>(
                new zmqpp::socket(*zmq_context_, zmqpp::socket_type::sub)
            );
            std::string full_ip;
            std::string ip = config_json["zmq"]["sub"]["ip"].get<std::string>();
            unsigned int port = config_json["zmq"]["sub"]["port"].get<unsigned int>();
            full_ip = "tcp://"+ip+":"+std::to_string(port);
            zmq_sub_->connect(full_ip);
            zmq_sub_->set(zmqpp::socket_option::subscribe, "", 0);
            zmq_sub_->set(zmqpp::socket_option::receive_timeout, 500);
            std::string msg("zmq sub("+full_ip+") ready");

            // set up buffer
            unsigned int sub_buffer_size = config_json["zmq"]["sub"]["buffer_size"].get<unsigned int>();
            zmq_sub_buffer_ = std::shared_ptr<QueueBuffer<std::string>>(
                new QueueBuffer<std::string>(sub_buffer_size)
            );

            SIMPLELOG(NORMAL, msg);
        }

        if(config_json["zmq"]["pub"]["on"].get<bool>()==true){
            zmq_pub_ = std::shared_ptr<zmqpp::socket>(
                new zmqpp::socket(*zmq_context_, zmqpp::socket_type::pub)
            );
            std::string full_ip;
            std::string ip = config_json["zmq"]["pub"]["ip"].get<std::string>();
            unsigned int port = config_json["zmq"]["pub"]["port"].get<unsigned int>();
            full_ip = "tcp://"+ip+":"+std::to_string(port);
            zmq_pub_->bind(full_ip);
            std::string msg("zmq pub("+full_ip+") ready");

            // set up buffer
            unsigned int pub_buffer_size = config_json["zmq"]["pub"]["buffer_size"].get<unsigned int>();
            zmq_pub_buffer_ = std::shared_ptr<QueueBuffer<std::string>>(
                new QueueBuffer<std::string>(pub_buffer_size)
            );

            SIMPLELOG(NORMAL, msg);
        }

        current_state_ = start_point_;
        system_status_ = StateManagerStatus::up;
        zmq_sub_thread_ = std::shared_ptr<std::thread>(new std::thread(&StateManager::zmq_sub_thread_worker, this));
        zmq_pub_thread_ = std::shared_ptr<std::thread>(new std::thread(&StateManager::zmq_pub_thread_worker, this));
        status_control_thread_ = std::shared_ptr<std::thread>(new std::thread(&StateManager::status_control_worker, this));
        SIMPLELOG(NORMAL, std::string("stateManger working"));

    }

    bool StateManager::pop_sub_message(std::string &msg){
        std::unique_lock<std::mutex> lock(sub_buffer_lock_);
        msg = zmq_sub_buffer_->pop();
        return msg == "";
    }

    void StateManager::push_sub_message(std::string msg){
        std::unique_lock<std::mutex> lock(sub_buffer_lock_);
        zmq_sub_buffer_->push(msg);
    }

    bool StateManager::pop_pub_message(std::string &msg){
        std::unique_lock<std::mutex> lock(pub_buffer_lock_);
        msg = zmq_pub_buffer_->pop();
        return msg == "";
    }

    void StateManager::push_pub_message(std::string msg){
        std::unique_lock<std::mutex> lock(pub_buffer_lock_);
        zmq_pub_buffer_->push(msg);
    }

    std::string StateManager::concate_state_id(std::shared_ptr<BasicState> state1, std::shared_ptr<BasicState> state2){
        if(state_to_id_.count(state1)==0 || state_to_id_.count(state2)==0){
            return "";
        }

        return std::to_string(state_to_id_[state1])+"|"+std::to_string(state_to_id_[state2]);
    }

    void StateManager::register_state(std::shared_ptr<BasicState> state){
        static unsigned int state_cnt=1;
        id_to_state_[state_cnt] = state;
        state_to_id_[state] = state_cnt;
        transitions_lists_[state_cnt] = {};
        state_cnt++;
        std::string msg = "state("+state->get_descip()+") resgitered";
        SIMPLELOG(NORMAL, msg);

        if(start_point_==nullptr){
            start_point_ = state;
            std::string msg = "state("+state->get_descip()+") is set to start state by default";
            SIMPLELOG(NORMAL, msg);
        }
        else{
            if(state->is_start_point()==true){
                start_point_ = state;
                std::string msg = "state("+state->get_descip()+") is set to start state";
                SIMPLELOG(NORMAL, msg);
            }
        }

        if(state->is_exist_point()==true){
            exist_point_ = state;
            std::string msg = "state("+state->get_descip()+") is set to exist state";
            SIMPLELOG(NORMAL, msg);
        }

    }

    void StateManager::register_transistion_func(
        std::shared_ptr<BasicState> from_state, std::shared_ptr<BasicState> to_state, uint32_t priority, transitFunc transit_func){
        
        std::unique_lock<std::mutex> lock(register_info_lock_);
        // update information for neighbors of from_state
        if(state_to_id_.count(from_state)==0){
            register_state(from_state);
        }

        if(state_to_id_.count(to_state)==0){
            register_state(to_state);
        }

        // update infomation for transitions pair
        std::string ref = concate_state_id(from_state, to_state);

        // invalid transition
        if(ref==""){
            std::string msg = "state("+from_state->get_descip()+") or state("+
                to_state->get_descip()+") is invalid";
            SIMPLELOG(NORMAL, msg);
            return;
        }

        if(state_transition_list_.count(ref)==0){
            auto entry = std::shared_ptr<transitions_entry>(new transitions_entry());
            entry->priority = priority;
            entry->judge_condition = transit_func;
            entry->from_state = from_state;
            entry->to_state = to_state;
            state_transition_list_[ref] = entry;
            transitions_lists_[get_id(from_state)].push(entry);
            std::string msg = "transition between state("+from_state->get_descip()+") and state("+
                to_state->get_descip()+") established";
            SIMPLELOG(NORMAL, msg);
        }
        else{
            state_transition_list_[ref]->priority = priority;
            state_transition_list_[ref]->judge_condition = transit_func;
            transitions_lists_[get_id(from_state)].update();
            std::string msg = "transition between state("+from_state->get_descip()+") and state("+
                to_state->get_descip()+") updated";
            SIMPLELOG(NORMAL, msg);
        }
    }

    void StateManager::unregister_transistion_func(std::shared_ptr<BasicState> from_state, std::shared_ptr<BasicState> to_state){
        std::unique_lock<std::mutex> lock(register_info_lock_);

        std::string ref = concate_state_id(from_state, to_state);

        // invalid transition
        if(ref==""){
            SIMPLELOG(ERROR, std::string("unregister transition failed! transition states invalid"));
            return;
        }

        if(state_transition_list_.count(ref)!=0){
            state_transition_list_.erase(ref);
            std::string msg = "transition between state("+from_state->get_descip()+"state("+
                to_state->get_descip()+") removed";
            SIMPLELOG(NORMAL, msg);
        }
        else{
            std::string msg = "transition between state("+from_state->get_descip()+"state("+
                to_state->get_descip()+") no exist, unregister no needed";
            SIMPLELOG(NORMAL, msg);
        }
    }

    StateManager::~StateManager(){
        system_status_ = StateManagerStatus::idle;

        status_control_thread_->join();
        zmq_sub_thread_->join();
        zmq_pub_thread_->join();

        std::string msg = "stateManager down";
        SIMPLELOG(NORMAL, msg);
    }
}