#ifndef SHOOTER_WHEELS_H
#define SHOOTER_WHEELS_H

#include<iosfwd>

namespace Shooter_wheels{
	enum Goal{HIGH_GOAL,TRUSS,PASS,STOP,X};
	std::ostream& operator<<(std::ostream&,Goal);

	typedef int RPM;
	RPM target_speed_top(Goal);
	RPM target_speed_bottom(Goal);
	bool ready(Goal,RPM top_speed,RPM bottom_speed);
}

#endif
