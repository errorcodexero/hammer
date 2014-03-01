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
#include "../util/countup_timer.h"
#include "toplevel.h"

struct Main{
	Force_interface force;
	Perf_tracker perf;
	Gyro_tracker gyro;
	Toplevel::Estimator est;

	Control_status::Control_status control_status;
	Countup_timer since_switch;

	Posedge_toggle ball_collecter;
	Posedge_trigger print_button;

	Posedge_toggle field_relative;
	
	Main();
	Robot_outputs operator()(Robot_inputs);
};

bool operator==(Main,Main);
bool operator!=(Main,Main);
std::ostream& operator<<(std::ostream&,Main);

#endif
