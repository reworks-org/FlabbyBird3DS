// Common.h

#ifndef _WRAPPER_COMMON_H_
#define _WRAPPER_COMMON_H_

#include <chrono>
#include <string>
#include <sstream>
#include <cstdint>
#include <ctime>
#include <cstdlib>

#define SCREEN_WIDTH_TOP 400
#define SCREEN_HEIGHT_TOP 240

#define SCREEN_WIDTH_BTM 320
#define SCREEN_HEIGHT_BTM 240

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

std::string to_string(int val)
{
	std::ostringstream ostr;
	ostr << val;
	return ostr.str();
}

uint64_t get_nano_time()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

inline uint64_t get_system_time()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void seed()
{
	srand((unsigned int)time(0));
}

int simple_random(int min, int max)
{
    return min + rand() % ((max + 1) - min);
}

#endif