#ifndef MAIN_H
#define MAIN_H

#include "interface.h"
#include "octocanum.h"
#include "force_interface.h"
#include "posedge_toggle.h"
#include "perf_tracker.h"
#include "bunny_launcher.h"
#include "countdown_timer.h"
#include "gyro_tracker.h"
#include "bound_tracker.h"

struct Bunnybot_goal{
	Octocanum_goal drive;
	bool launch_bunny_now;
	bool poop_bunny_now;
	
	Bunnybot_goal();
};

std::ostream& operator<<(std::ostream&,Bunnybot_goal);

struct Main{
	typedef enum{
		TELEOP,
		AUTO_DRIVE1,
		AUTO_SHOOT,
		AUTO_DRIVE2,
		AUTO_DONE
	} Mode;
	Mode mode;
	Countdown_timer auto_timer;
	
	Posedge_toggle goal_traction_mode;
	Octocanum_state octocanum;
	Bunny_launcher bunny_launcher;
	Bunny_launcher bunny_pooper;
	Force_interface force;
	Perf_tracker perf;
	Gyro_tracker gyro;
	//Bound_tracker bound[Joystick_data::AXES];
	
	Main();
	
	Bunnybot_goal autonomous(Robot_inputs);
	Bunnybot_goal teleop(Robot_inputs);
	Bunnybot_goal goal(Robot_inputs);
	Robot_outputs operator()(Robot_inputs);
};

std::ostream& operator<<(std::ostream&,Main);

#endif
