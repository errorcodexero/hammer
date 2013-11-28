#ifndef POSEDGE_TRIGGER_H
#define POSEDGE_TRIGGER_H

#include<iosfwd>

struct Posedge_trigger{
	bool last;

	Posedge_trigger();
	bool operator()(bool);
};

std::ostream& operator<<(std::ostream& o,Posedge_trigger);

#endif
