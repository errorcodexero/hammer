#ifndef SHOOTER_WHEELS_H
#define SHOOTER_WHEELS_H

#include<iosfwd>
#include<utility>
#include"shooter_status.h"
#include"wheelrpms.h"
#include "../util/jag_interface.h"
#include "../input/panel2014.h"

namespace Shooter_wheels{
	enum High_level_goal{HIGH_GOAL,TRUSS,PASS,STOP,X};
	std::ostream& operator<<(std::ostream&,High_level_goal);
	
	struct Output{
		static const int FEEDBACK=0,OPEN_LOOP=1;
		Jaguar_output top[2],bottom[2];
	};
	std::ostream& operator<<(std::ostream& o,Output);
	
	typedef std::pair<High_level_goal,Status> Goal;
	Output control(Goal);

	struct Calibration_manager{
		wheelcalib calib;
		Posedge_trigger learn;
		
		Calibration_manager();
		
		//adjust wheel in 0-3.3
		wheelcalib update(bool learn,double adjust_wheel,Calibration_target);
	};
	std::ostream& operator<<(std::ostream&,Calibration_manager);
	
	Goal convert_goal(wheelcalib,High_level_goal);
	Output control(Status,Goal);
	bool ready(Status,Goal);

	RPM free_speed();
}

#endif
