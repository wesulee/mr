#pragma once

#include <chrono>
#include <iostream>


class Console {
public:
	static std::ostream& begin(void);
	static void start(void);
private:
	static std::chrono::steady_clock::time_point startTime;
	static std::ostream& os;
};
