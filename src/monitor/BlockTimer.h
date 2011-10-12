#ifndef BLOCKTIMER_H_
#define BLOCKTIMER_H_

#include <sys/time.h>

namespace monitor
{

class BlockTimer
{
public:
	static unsigned long get_tick_count()
	{
		timeval ts;
		gettimeofday(&ts,0);
		return ((unsigned long)(ts.tv_sec * 1000 + (ts.tv_usec / 1000)));
	}

	BlockTimer(const std::string& description) : description_(description), start_time_(get_tick_count())
	{
	}

	~BlockTimer()
	{
		std::clog << description_ << ": " << (get_tick_count()-start_time_) << "\n";
	}

private:
	BlockTimer(const BlockTimer& bt)
	{
	}

	std::string description_;
	unsigned long start_time_;
};

}

#endif /* BLOCKTIMER_H_ */
