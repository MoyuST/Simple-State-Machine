# Simple state machine

During my first formal internship, I had read tons of codes of the company. Among all these codes, the most impressive one was the state machine. After I left my company, I always wanna build a simple state machine library of my own which could be used when needed.

## design patterns

### components

1. states
    <!-- the build blocks of the program -->
    - basic state
        *BasicState* is the basic of the state, all state involved in the system must inherit it and complete all necessary functions.
    - inside one state
        the *void inside_loop_func(void)* is the function the system would consistently running once it in the state.
    - between states
        - the *register_transistion_func(BasicState\* from_state, BasicState\* to_state, uint32_t priority, bool (\*transit()))* is the function connecting two states specifying the priority. the priority would be useful when one state could be transferred to multiple other state. in this case, the priority would determine which state the from_state could be changed to.
        - the *unregister_transistion_func(BasicState\* from_state, BasicState\* to_state)* would then remove the transition between two states.
        - at most one *transition_func* is allowed from exatcly same one state to another state. if multiple *transition_func* is set, the one set lastest would remain
    - state manager
        to make sure the system running correctly, we need to have a manager to trace all the status of the system

2. inputs
    <!-- how system detect the input to change its state -->
    when making decision to whether to change from current state to another one, we need some necessary to make desicion. to make that much easier, pub/sub pattern will be used. that is, we will register our program as a subscriber with the aid of zmq, and all this program do is simply parse the message captured. if this feature is not necessary needed by you, you can disable it by modify the configure file.

3. monitor/outputs
    <!--  how others outside the system could find the state of the current system -->

4. configure
    <!--  how the program could start up with required settings -->
    read json file to change the settings of the program

## usage

the basic usage could be found in the `example/example.cpp`, which initiate a simple state machine. to control the state machine in the example, you may need to start a zmq server like `example\example_helper.cpp`. `example` would be automatically generated after building the project using cmake. to build `example_helper`, simple type `make example_helper` in the example folder.

![sample](Assets/imgs/sample.png)


## current process

### implementation
- [x] desgin
- [x] functions
  - [x] basic functions (where zmq is mandatory)
  - [ ] disable zmq using `config.json`

### build & install
- [x] build
- [ ] install

## special thanks

thanks to 
  - [zmq](https://zeromq.org/) for powerful sub/pub function
  - [nlohmann/json](https://github.com/nlohmann/json) for handy json function