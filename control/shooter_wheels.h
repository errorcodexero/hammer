#ifndef SHOOTER_WHEELS_H
#define SHOOTER_WHEELS_H

#include<iosfwd>
#include"shooter_status.h"
#include"wheelrpms.h"


namespace Shooter_wheels{
	enum Goal{HIGH_GOAL,TRUSS,PASS,STOP,X};
	std::ostream& operator<<(std::ostream&,Goal);
	
	RPM target_speed_top(Goal);
	RPM target_speed_bottom(Goal);

	typedef Status Output;
	Output control(Goal);

	bool ready(Goal,RPM top_speed,RPM bottom_speed);
	bool ready(Status,Goal);
	
	class Shooter{
		Shooter();
		wheelcalib calib;
		Output control(Goal g);
	};
	std::ostream& operator<<(std::ostream&,Shooter);
}

#endif
