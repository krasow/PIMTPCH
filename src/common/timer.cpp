#include <sys/time.h>

#include <common/tpch.h>
#include <common/timer.h>

void calc_time::start() {
	gettimeofday(&startTime, NULL);
}

void calc_time::stop() {
	gettimeofday(&stopTime, NULL);
	this->update();
}

void calc_time::set(double time) {
	this->iterations++;
	this->total_time += time;
	this->average_time = this->total_time / this->iterations;
}

void calc_time::print(std::string test, uint64_t output) {
	printf("%s with %d iterations\n", test.c_str(), this->iterations);
	if (output != 0)  printf("test result: %lu\n", output);
	printf("avg time: %f ms\n\n", this->average_time);
}

void calc_time::print(std::string test) {
	this->print(test, 0);
}

void calc_time::update() {
	this->iterations++;
	this->total_time += ((stopTime.tv_sec - startTime.tv_sec) * 1000000.0 +
		stopTime.tv_usec - startTime.tv_usec) / 1000;

	this->average_time = this->total_time / this->iterations;
}

double calc_time::time() {
	return this->average_time;
}