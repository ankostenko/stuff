#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>

using namespace std::chrono;

namespace my
{
	namespace timer
	{
		high_resolution_clock::time_point last_time;

		void startTimer()
		{				
			last_time = high_resolution_clock::now();
		}

		uint64_t passed_time()
		{
			high_resolution_clock::time_point now = high_resolution_clock::now();
			uint64_t time_dif = (uint64_t)(duration_cast<nanoseconds>(now - last_time).count());
			last_time = now;
			return time_dif;
		}
	}

	uint64_t now()
	{
		uint64_t time = clock();
		return time;
	}
}