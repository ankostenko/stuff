#pragma once

#include <chrono>

using namespace std::chrono;

namespace my
{
    class Timer
    {
    private:
        high_resolution_clock::time_point start_time;
        
    public:
        Timer()
        {
            start_time = high_resolution_clock::now();
        }
        
        ~Timer()
        {
            high_resolution_clock::time_point end_time = high_resolution_clock::now();
            nanoseconds time_dif = duration_cast<nanoseconds>(end_time - start_time);
            std::cout << time_dif.count() << "nano" << '\n';
            }
            
        static high_resolution_clock::time_point now() {
            return  high_resolution_clock::now();
        }
            
    };
   
}
