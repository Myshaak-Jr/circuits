#include "util.h"
#include <chrono>
#include <ctime>
#include <format>
#include <string>


std::string make_timestamp() {
	using namespace std::chrono;

	auto now = system_clock::now();
	auto t = system_clock::to_time_t(now);

	std::tm tm{};
	localtime_s(&tm, &t);

	return std::format("{:04}-{:02}-{:02}-{:02}-{:02}-{:02}",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec
	);
}