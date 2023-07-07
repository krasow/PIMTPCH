#ifndef TIMER_H 
#define TIMER_H

#include <string>


/*  class calculates average time
 *  calc_time::start()		: begins timer
 *  calc_time::stop() 		: stops timer and updates average time
 *  calc_time::set(double)	: manually set new iteration without calling start() and stop()
 *  calc_time::time()		: retrieve average time
 *  calc_time::print()		: print average time for test
 */
class calc_time {
private:
	struct timeval startTime;
	struct timeval stopTime;
	double total_time = 0.0;
	double average_time = 0.0;
	uint32_t iterations = 0;
	void update();

public:
	void start();
	void stop();
	void set(double time);
	void print(std::string test, uint64_t output);
	void print(std::string test);
	double time();
};

#endif