#include "timer.h"
#include <chrono>

static std::chrono::time_point<std::chrono::steady_clock> s_start_time;
static std::chrono::time_point<std::chrono::steady_clock> s_end_time;
static bool s_timer_active = false;

void timer::start()
{
	s_start_time = std::chrono::steady_clock::now();
	s_timer_active = true;
}

double timer::get_elapsed_seconds()
{
	const std::chrono::time_point<std::chrono::steady_clock> elapsed_time = s_timer_active ? std::chrono::steady_clock::now() : s_end_time;
	const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time - s_start_time).count();
	return elapsed / 1000000000.0;

}

void timer::stop()
{
	s_end_time = std::chrono::steady_clock::now();
	s_timer_active = false;
}