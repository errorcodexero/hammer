#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include "collector.h"
#include "collector_tilt.h"
#include "injector.h"
#include "injector_arms.h"
#include "shooter_wheels.h"
#include "ejector.h"

namespace Toplevel{
	struct Output{
		Output();

		Collector_mode collector;
		Collector_tilt::Output collector_tilt;
		Injector::Output injector;
		Injector_arms::Output injector_arms;
		Ejector::Output ejector;
		Shooter_wheels::Output shooter_wheels;
	};
	std::ostream& operator<<(std::ostream&,Output);

	struct Subgoals{
		Subgoals();

		Collector_mode collector;
		Collector_tilt::Goal collector_tilt;
		Injector::Goal injector;
		Injector_arms::Goal injector_arms;
		Ejector::Goal ejector;
		Shooter_wheels::Goal shooter_wheels;
		//todo: add drivebase stuff here.
	};
	std::ostream& operator<<(std::ostream&,Subgoals);

	struct Status{
		Status();

		//collector has no state
		Collector_tilt::Status collector_tilt;
		Injector::Estimator::Location injector;
		Injector_arms::Status injector_arms;
		Ejector::Estimator::Location ejector;
		Shooter_wheels::Status shooter_wheels;
	};
	std::ostream& operator<<(std::ostream& o,Status);

	class Estimator{
		//no estimate for collector
		Collector_tilt::Estimator collector_tilt;
		Injector::Estimator injector;
		Injector_arms::Estimator injector_arms;
		Ejector::Estimator ejector;
		//no estimates for shooter wheels yet.

		public:
		void update(Time,Output);
		Status estimate()const;
		void out(std::ostream&)const;
	};
	std::ostream& operator<<(std::ostream& o,Estimator);

	Output control(Status,Subgoals);
	bool ready(Status,Subgoals);

	//all this mode stuff really belongs elsewhere
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
