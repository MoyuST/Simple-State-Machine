#include "simpleStateMachine.hpp"
#include <iostream>

#define SIMPLELOGEXAMPLE(STATUS, MSG)                                                          \
    do{                                                                                 \
    auto cur_time = std::chrono::system_clock::now();                                   \
    std::time_t cur_time_t = std::chrono::system_clock::to_time_t(cur_time);            \
        std::cout << "[example_zmq][" << strtok(std::ctime(&cur_time_t), "\n") << "][" \
        << #STATUS << "] " << MSG << std::endl;                                         \
    } while(false)

int main(int argc, char const *argv[])
{
    auto state1 = std::shared_ptr<simpleStateMachine::BasicState>(new simpleStateMachine::BasicState("state1", true, false, [](){
        SIMPLELOGEXAMPLE(NORMAL, "state1");
    }));

    auto state2 = std::shared_ptr<simpleStateMachine::BasicState>(new simpleStateMachine::BasicState("state2", false, false, [](){
        SIMPLELOGEXAMPLE(NORMAL,"state2");
    }));

    auto state3 = std::shared_ptr<simpleStateMachine::BasicState>(new simpleStateMachine::BasicState("state3", false, false, [](){
        SIMPLELOGEXAMPLE(NORMAL,"state3");
    }));


    STATEMANAGER->register_transistion_func(state1, state2, 1, []()->bool{
        if(STATEMANAGER->system_zmq_info().contains("enter")){
            return STATEMANAGER->system_zmq_info()["enter"].get<std::string>() == "1->2";
        }
        return false;
    });

    STATEMANAGER->register_transistion_func(state2, state3, 1, []()->bool{
        if(STATEMANAGER->system_zmq_info().contains("enter")){
            return STATEMANAGER->system_zmq_info()["enter"].get<std::string>() == "2->3";
        }
        return false;
    });

    STATEMANAGER->register_transistion_func(state3, state1, 1, []()->bool{
        if(STATEMANAGER->system_zmq_info().contains("enter")){
            return STATEMANAGER->system_zmq_info()["enter"].get<std::string>() == "3->1";
        }
        return false;
    });

    STATEMANAGER->start_up_manager("../config/service.json");

    STATEMANAGER->spin();

    return 0;
}
