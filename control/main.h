#ifndef MAIN_H
#define MAIN_H

#include "octocanum.h"
#include "force_interface.h"
#include "bunny_launcher.h"
#include "gyro_tracker.h"
#include "../util/interface.h"
#include "../util/posedge_toggle.h"
#include "../util/perf_tracker.h"
#include "../util/countdown_timer.h"
#include "../util/bound_tracker.h"

struct Bunnybot_goal{
	Octocanum_goal drive;
	bool launch_bunny_now;
	bool poop_bunny_now;
	
	Bunnybot_goal();
};

std::ostream& operator<<(std::ostream&,Bunnybot_goal);

struct Bunnybot_output{
	Octocanum_output drive;
	bool launch_bunny;
	bool poop_bunny;

	Bunnybot_output();
};

std::ostream& operator<<(std::ostream&,Bunnybot_output);

struct Bunnybot{
	typedef enum{
		TELEOP,
		AUTO_DRIVE1,
		AUTO_SHOOT,
		AUTO_DRIVE2,
		AUTO_DONE
	} Mode;
	Mode mode;
	Countdown_timer auto_timer;
	Posedge_toggle ball_collecter;
	Posedge_toggle goal_traction_mode;
	Octocanum_state octocanum;
	Bunny_launcher bunny_launcher;
	Bunny_launcher bunny_pooper;

	Bunnybot();

	Bunnybot_goal autonomous(Robot_inputs);
	Bunnybot_goal teleop(Robot_inputs);
	Bunnybot_goal goal(Robot_inputs);
	Bunnybot_output operator()(Robot_inputs);
};

std::ostream& operator<<(std::ostream&,Bunnybot::Mode);
std::ostream& operator<<(std::ostream&,Bunnybot);

struct Main{
	Force_interface force;
	Perf_tracker perf;
	Gyro_tracker gyro;
	Bunnybot bunnybot;
	Posedge_toggle ball_collecter;
	
	Robot_outputs operator()(Robot_inputs);
};

std::ostream& operator<<(std::ostream&,Main);

#endif
