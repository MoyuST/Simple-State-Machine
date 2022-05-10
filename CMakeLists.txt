cmake_minimum_required(VERSION 3.10)

project(SIMPLE_STATE_MACHINE VERSION 1.0.0)

add_library(build_flags INTERFACE)
target_compile_definitions(build_flags INTERFACE cxx_std_11)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

file(GLOB utilities_files
    "${PROJECT_SOURCE_DIR}/utilities/*.*"
)

add_library(SIMPLESTATEMACHINE STATIC
    "${PROJECT_SOURCE_DIR}/basicState/basicState.hpp"
    "${PROJECT_SOURCE_DIR}/json/json.hpp"
    "${PROJECT_SOURCE_DIR}/simpleLogger/simpleLogger.hpp"
    "${PROJECT_SOURCE_DIR}/stateManager/stateManager.cpp"
    ${utilities_files}
)

target_include_directories(SIMPLESTATEMACHINE PRIVATE
    "${PROJECT_SOURCE_DIR}/basicState"
    "${PROJECT_SOURCE_DIR}/json"
    "${PROJECT_SOURCE_DIR}/simpleLogger"
    "${PROJECT_SOURCE_DIR}/stateManager"
    "${PROJECT_SOURCE_DIR}/utilities"
)


option(USE_ZMQPP "use zmqpp" ON)
option(BUILD_EXAMPLE "build example" ON)

target_link_libraries(SIMPLESTATEMACHINE pthread)

if(USE_ZMQPP)
    target_link_libraries(SIMPLESTATEMACHINE zmqpp zmq sodium pthread)
    target_compile_definitions(SIMPLESTATEMACHINE PUBLIC "USE_ZMQPP")
endif()

if(BUILD_EXAMPLE)
    if(USE_ZMQPP)
        set(EXAMPLE example_zmq)
        set(EXAMPLE_HELPER example_zmq_helper)
    else()
        set(EXAMPLE example_socket)
        set(EXAMPLE_HELPER example_socket_helper)
    endif()

    add_executable(${EXAMPLE} "${PROJECT_SOURCE_DIR}/example/${EXAMPLE}.cpp")
    target_link_libraries(${EXAMPLE} SIMPLESTATEMACHINE)
    target_include_directories(${EXAMPLE} PRIVATE
        "${PROJECT_SOURCE_DIR}/basicState"
        "${PROJECT_SOURCE_DIR}/json"
        "${PROJECT_SOURCE_DIR}/simpleLogger"
        "${PROJECT_SOURCE_DIR}/stateManager"
        "${PROJECT_SOURCE_DIR}/utilities"
        "${PROJECT_SOURCE_DIR}"
    )

    execute_process(
        COMMAND make clean
        COMMAND make ${EXAMPLE_HELPER}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/example
    )

endif()
