#pragma once

#include <chrono>
#include <iostream>


class Console {
public:
	static std::ostream& begin(void);	// begin new line
	static void startTimer(void);
	static void flush(void);
	static std::ostream& get(void);
private:
	static std::chrono::steady_clock::time_point startTime;
	static std::ostream& os;
};


inline
void Console::flush() {
	os.flush();
}


inline
std::ostream& Console::get() {
	return os;
}
