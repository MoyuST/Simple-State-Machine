#ifndef SIMPLE_STATE_MACHINE_BASIC_STATE
#define SIMPLE_STATE_MACHINE_BASIC_STATE
#include <functional>

namespace simpleStateMachine{
    using loopFunc = std::function<void(void)>;
    class BasicState{
        private:
            // the looping function inside
            std::string descrip_;
            loopFunc loop_func_=nullptr;
            bool is_start_point_=false;
            bool is_exist_point_=false;
        
        public:
            // BasicState();
            BasicState(std::string descrip): descrip_(descrip) {}
            BasicState(std::string descrip, bool is_start_point, bool is_exist_point) : 
                descrip_(descrip), is_start_point_(is_start_point), is_exist_point_(is_exist_point) {
            }
            BasicState(std::string descrip, bool is_start_point, bool is_exist_point, loopFunc loop_func) :
                descrip_(descrip), is_start_point_(is_start_point), is_exist_point_(is_exist_point), loop_func_(loop_func) {
            }

            inline std::string get_descip(){
                return descrip_;
            }

            void set_inside_looping(loopFunc loop_func){ this->loop_func_ = loop_func;}
            void set_is_start_point(bool option){ this->is_start_point_ = option;}
            void set_is_exsit_point(bool option){ this->is_exist_point_ = option;}

            inline bool is_start_point(){return is_start_point_;}
            inline bool is_exist_point(){return is_exist_point_;}
            inline void loop(){
                if(loop_func_!=nullptr){
                    loop_func_();
                }
            }
    };
}
#endif