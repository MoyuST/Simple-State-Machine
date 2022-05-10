cmake_minimum_required(VERSION 3.10)

project(SIMPLE_STATE_MACHINE VERSION 1.0.0)

add_library(build_flags INTERFACE)
target_compile_definitions(build_flags INTERFACE cxx_std_11)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

file(GLOB utilities_files
    "${PROJECT_SOURCE_DIR}/utilities/*.*"
)

add_library(simple_state_machine STATIC
    "${PROJECT_SOURCE_DIR}/basicState/basicState.hpp"
    "${PROJECT_SOURCE_DIR}/json/json.hpp"
    "${PROJECT_SOURCE_DIR}/simpleLogger/simpleLogger.hpp"
    "${PROJECT_SOURCE_DIR}/stateManager/stateManager.cpp"
    ${utilities_files}
)

target_include_directories(simple_state_machine PRIVATE
    "${PROJECT_SOURCE_DIR}/basicState"
    "${PROJECT_SOURCE_DIR}/json"
    "${PROJECT_SOURCE_DIR}/simpleLogger"
    "${PROJECT_SOURCE_DIR}/stateManager"
    "${PROJECT_SOURCE_DIR}/utilities"
)

target_link_libraries(simple_state_machine zmqpp zmq sodium pthread)

add_executable(example "${PROJECT_SOURCE_DIR}/example/example.cpp")

target_link_libraries(example simple_state_machine)

target_include_directories(example PRIVATE
    "${PROJECT_SOURCE_DIR}/basicState"
    "${PROJECT_SOURCE_DIR}/json"
    "${PROJECT_SOURCE_DIR}/simpleLogger"
    "${PROJECT_SOURCE_DIR}/stateManager"
    "${PROJECT_SOURCE_DIR}/utilities"
    "${PROJECT_SOURCE_DIR}"
)