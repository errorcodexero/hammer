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

namespace Control_status{
enum Control_status{
	AUTO_SPIN_UP,
	AUTO_FIRE,
	AUTO_TO_COLLECT,
	AUTO_COLLECT,
	AUTO_SPIN_UP2,
	AUTO_FIRE2,

	//many of these are the same as things in "Mode"
	DRIVE_W_BALL,DRIVE_WO_BALL,
	COLLECT,
	SHOOT_HIGH_PREP,SHOOT_HIGH,SHOOT_HIGH_WHEN_READY,
	TRUSS_TOSS_PREP,TRUSS_TOSS,TRUSS_TOSS_WHEN_READY,
	PASS_PREP,PASS,PASS_WHEN_READY,
	EJECT_PREP,EJECT,EJECT_WHEN_READY,
	CATCH	
};
std::ostream& operator<<(std::ostream&,Control_status);
}



struct Main{
	Force_interface force;
	Perf_tracker perf;
	Gyro_tracker gyro;
	//Bunnybot bunnybot;
	Control_status::Control_status control_status;
	Posedge_toggle ball_collecter;

	Main();	
	Robot_outputs operator()(Robot_inputs);
};

std::ostream& operator<<(std::ostream&,Main);

#endif
