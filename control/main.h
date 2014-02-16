#ifndef MAIN_H
#define MAIN_H

#include "force_interface.h"
#include "gyro_tracker.h"
#include "../util/interface.h"
#include "../util/posedge_toggle.h"
#include "../util/perf_tracker.h"
#include "../util/countdown_timer.h"
#include "../util/bound_tracker.h"
#include "control_status.h"

struct Main{
	Force_interface force;
	Perf_tracker perf;
	Gyro_tracker gyro;

	Control_status::Control_status control_status;
	Countup_timer since_switch;

	Posedge_toggle ball_collecter;
	Posedge_trigger print_button;

	bool isPressed;
	bool fieldRelative;
	
	Main();
	Robot_outputs operator()(Robot_inputs);
};

std::ostream& operator<<(std::ostream&,Main);

#endif
