#ifndef SHOOTER_WHEELS_H
#define SHOOTER_WHEELS_H

#include<iosfwd>
#include"shooter_status.h"
#include"wheelrpms.h"
#include "../util/jag_interface.h"

namespace Shooter_wheels{
	enum High_level_goal{HIGH_GOAL,TRUSS,PASS,STOP,X};
	std::ostream& operator<<(std::ostream&,High_level_goal);
	
	struct Output{
		static const int FEEDBACK=0,OPEN_LOOP=1;
		Jaguar_output top[2],bottom[2];
	};
	std::ostream& operator<<(std::ostream& o,Output);
	
	typedef Status Goal;
	Output control(Goal);

	struct Control{
		wheelcalib calib;
		
		Control();
		
		RPM target_speed_top(High_level_goal)const;
		RPM target_speed_bottom(High_level_goal)const;

		bool ready(High_level_goal,RPM top_speed,RPM bottom_speed)const;
		bool ready(Status,High_level_goal)const;
	};
	std::ostream& operator<<(std::ostream&,Control);
	Goal convert_goal(wheelcalib,High_level_goal);
	Output control(Status,Goal);
	bool ready(Status,Goal);
}

#endif
