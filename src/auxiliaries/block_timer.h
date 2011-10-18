#ifndef BLOCK_TIMER_H_
#define BLOCK_TIMER_H_

#include <sys/time.h>
#include <string>

namespace auxiliaries
{

class BlockTimer
{
public:
	BlockTimer(const std::string& description) : description_(description), start_time_(get_tick_count())
	{
	}

	~BlockTimer()
	{
		std::clog << description_ << ": " << (get_tick_count()-start_time_) << "\n";
	}

	static unsigned long get_tick_count()
	{
		timeval ts;
		gettimeofday(&ts,0);
		return ((unsigned long)(ts.tv_sec * 1000 + (ts.tv_usec / 1000)));
	}

private:
	BlockTimer(const BlockTimer& bt)
	{
	}

	std::string description_;
	unsigned long start_time_;
};

}

#endif /* BLOCK_TIMER_H_ */
