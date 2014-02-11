#ifndef COUNTUP_TIMER_H
#define COUNTUP_TIMER_H

#include "interface.h"

class Countup_timer{
	Time start,latest;

	public:
	Countup_timer();
	void update(Time,bool reset);
	Time elapsed()const;
	void out(std::ostream&)const;
};

std::ostream& operator<<(std::ostream&,Countup_timer);

#endif
