#ifndef COUNTDOWN_TIMER_H
#define COUNTDOWN_TIMER_H

#include "interface.h"

class Countdown_timer{
	Time left,last_time;
	bool last_enable;
	
	public:
	Countdown_timer();
	void set(Time length);
	void update(Time now,bool enable);
	bool done()const;
	
	friend std::ostream& operator<<(std::ostream&,Countdown_timer);
};

std::ostream& operator<<(std::ostream&,Countdown_timer);

#endif
