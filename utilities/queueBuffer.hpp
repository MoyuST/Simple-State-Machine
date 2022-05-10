#ifndef SIMPLE_STATE_MACHINE_QUEUEBUFFER
#define SIMPLE_STATE_MACHINE_QUEUEBUFFER

#include <queue>

namespace simpleStateMachine{
    template<class T>
    class QueueBuffer {
        private:
            std::queue<T> inside_queue_;
            unsigned int max_size_;
        
        public:
            QueueBuffer() = delete;
            QueueBuffer(unsigned int max_size):
                max_size_(max_size) {}
            void push(T data){
                if(inside_queue_.size()>=max_size_){
                    inside_queue_.pop();
                }
                inside_queue_.push(data);
            }
            T pop(){
                if(inside_queue_.empty()){
                    return T{};
                }
                else{
                    T rt = inside_queue_.front();
                    inside_queue_.pop();
                    return rt;
                }
            }
            bool empty(){
                return inside_queue_.empty();
            }
            size_t size(){
                return inside_queue_.size();
            }

    };
}
#endif