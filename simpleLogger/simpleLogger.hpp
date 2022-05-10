#ifndef SIMPLE_STATE_MACHINE_SIMPLE_LOGGER
#define SIMPLE_STATE_MACHINE_SIMPLE_LOGGER
#include <iostream>
#include <ctime> 
#include <chrono>

#define SIMPLELOG(STATUS, MSG)                                                          \
    do{                                                                                 \
    auto cur_time = std::chrono::system_clock::now();                                   \
    std::time_t cur_time_t = std::chrono::system_clock::to_time_t(cur_time);            \
        std::cout << "[stateManager][" << strtok(std::ctime(&cur_time_t), "\n") << "][" \
        << #STATUS << "] " << MSG << std::endl;                                         \
    } while(false)
#endif