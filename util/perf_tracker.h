#ifndef PERF_TRACKER_H
#define PERF_TRACKER_H

#include "interface.h"

class Perf_tracker{
	Time first,last;
	Time worst;
	int i;
	
	public:
	Perf_tracker();

	void update(Time now);
	void reset_worst();

	Time worst_case()const;
	Time average()const;
	
	friend std::ostream& operator<<(std::ostream&,Perf_tracker);
};

std::ostream& operator<<(std::ostream&,Perf_tracker);

#endif
