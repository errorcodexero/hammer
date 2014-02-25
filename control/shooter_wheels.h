#ifndef SHOOTER_WHEELS_H
#define SHOOTER_WHEELS_H

#include<iosfwd>
#include"shooter_status.h"
#include"wheelrpms.h"


namespace Shooter_wheels{
	enum Goal{HIGH_GOAL,TRUSS,PASS,STOP,X};
	std::ostream& operator<<(std::ostream&,Goal);
	
	typedef Status Output;
	Output control(Goal);

	struct Control{
		wheelcalib calib;
		
		Control();
		
		RPM target_speed_top(Goal)const;
		RPM target_speed_bottom(Goal)const;

		bool ready(Goal,RPM top_speed,RPM bottom_speed)const;
		bool ready(Status,Goal)const;
		
		Output control(Goal g)const;
	};
	std::ostream& operator<<(std::ostream&,Control);
}

#endif
