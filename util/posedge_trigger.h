#ifndef POSEDGE_TRIGGER_H
#define POSEDGE_TRIGGER_H

#include<iosfwd>

struct Posedge_trigger{
	bool last;
	//Last stores the last value that was given

	Posedge_trigger();
	//Lets you call things of posedgetrigger as if it were a function
	bool operator()(bool);
	//If previous value was 0, and new value is one, return one
};

std::ostream& operator<<(std::ostream& o,Posedge_trigger);

#endif
