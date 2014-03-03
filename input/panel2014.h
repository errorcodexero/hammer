#ifndef PANEL2014_H
#define PANEL2014_H

#include "../control/fire_control.h"
#include "../util/maybe.h"
#include "../control/collector.h"
#include "../control/collector_tilt.h"
#include "../control/injector.h"
#include "../control/injector_arms.h"
#include "../control/ejector.h"

//these will probably be the things that light up.
struct Mode_buttons{
	bool drive_wo_ball,drive_w_ball,collect,shoot_high,truss_toss,pass,eject,catch_mode;

	Mode_buttons();
};
std::ostream& operator<<(std::ostream&,Mode_buttons);

//maybe this should go in wheelcalib.h or something.
struct Calibration_target{
	Fire_control::Target target;
	bool top;
	
	Calibration_target();
};
std::ostream& operator<<(std::ostream&,Calibration_target);

struct Panel{
	//presets
	Mode_buttons mode_buttons;
	bool fire,pass_now;

	//shooting override section
	Calibration_target target;
	double speed;	
	bool learn;

	//standard override section
	/*
	Maybe<Collector_mode> collector;
	Maybe<Collector_tilt::Output> collector_tilt;
	Maybe<Injector::Output> injector;
	Maybe<Injector_arms::Output> injector_arms;
	Maybe<Ejector::Output> ejector;
	*/
	bool force_wheels_off;
	
	Panel();
};
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Driver_station_input);

#endif
