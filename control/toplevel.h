#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include "collector.h"
#include "collector_tilt.h"
#include "injector.h"
#include "injector_arms.h"
#include "shooter_wheels.h"

namespace Toplevel{
	struct Subgoals{
		Subgoals();

		Collector_mode collector;
		Collector_tilt::Goal collector_tilt;
		Injector::Goal injector;
		Injector_arms::Goal injector_arms;
		Injector::Goal ejector;
		Shooter_wheels::Goal shooter_wheels;
		//todo: add drivebase stuff here.
	};
	std::ostream& operator<<(std::ostream&,Subgoals);

	struct Status{
		Status();

		//collector has no state
		Collector_tilt::Status collector_tilt;
		Injector::Status injector;
		Injector_arms::Status injector_arms;
		Injector::Status ejector;
		Shooter_wheels::Status shooter_wheels;
	};
	std::ostream& operator<<(std::ostream& o,Status);

	enum Mode{
		DRIVE_WO_BALL,DRIVE_W_BALL,
		COLLECT,
		SHOOT_HIGH_PREP,SHOOT_HIGH,
		TRUSS_TOSS_PREP,TRUSS_TOSS,
		PASS_PREP,PASS,
		EJECT_PREP,EJECT,
		CATCH
	};
	std::ostream& operator<<(std::ostream& o,Mode);

	Subgoals subgoals(Mode);
}

#endif
