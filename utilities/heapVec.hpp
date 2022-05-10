#ifndef SIMPLE_STATE_MACHINE_HEAPVEC
#define SIMPLE_STATE_MACHINE_HEAPVEC

#include <vector>

namespace simpleStateMachine{
    template<class T, class C>
    class HeapVec{
        private:
            std::vector<T> container_;
        
        public:
            void remove(T t){
                for(int i=0;i<container_.size();i++){
                    if(t==container_[i]){
                        container_.erase(i);
                        break;
                    }
                }
                update();
            }

            void update(){
                std::make_heap(container_.begin(), container_.end(), C{});
            }

            void push(T t){
                container_.push_back(t);
                std::push_heap(container_.begin(), container_.end(), C{});
            }

            T pop(){
                if(container_.empty()){
                    return T{};
                }
                else{
                    std::pop_heap(container_.begin(), container_.end(), C{});
                    T rt = container_.back();
                    container_.pop_back();
                    return rt;
                }
            }

            void for_each(std::function< bool(T) > loop_end_condi){
                bool loop_end_flag = false;
                int pop_posi=container_.size()-1;

                int i=0;
                for(;(i<container_.size())&&(loop_end_flag == false);i++){
                    std::pop_heap(container_.begin(), container_.end()-i, C{});
                    loop_end_flag = loop_end_condi(container_[pop_posi]);
                }

                for(;i>=0;i--){
                    std::push_heap(container_.begin(), container_.end()-i, C{});
                }
            }
    };
}
#endif