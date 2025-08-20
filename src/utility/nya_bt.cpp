#include "utility/nya_bt.hpp"


namespace nya {
    #pragma GCC push_options
    #pragma GCC optimize("O0") 
    uintptr_t nya_backtrace(int index) {
        int counter = 0;
        uint64_t cur_frame;
        asm volatile("mov %0, fp" : "=r"(cur_frame));
        uint64_t prev_frame = 0;

        while (cur_frame != 0 and prev_frame != cur_frame) {
            prev_frame = cur_frame; 

            if (counter == index) {
                if (cur_frame % sizeof(uint64_t) == 0)
                    return ((uint64_t*)cur_frame)[1];
                else
                    return 0;
            }
            cur_frame = ((uint64_t*)cur_frame)[0];
            counter++;
        }
        return 0; 
    } 
    #pragma GCC pop_options 
}