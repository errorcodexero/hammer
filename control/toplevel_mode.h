#ifndef TOPLEVEL_MODE_H
#define TOPLEVEL_MODE_H

#include "toplevel.h"
#include "control_status.h"

namespace Toplevel{
	//all this mode stuff really belongs elsewhere
	enum Mode{
		DRIVE_WO_BALL,DRIVE_W_BALL,
		COLLECT,
		SHOOT_HIGH_PREP,SHOOT_HIGH,
		TRUSS_TOSS_PREP,TRUSS_TOSS,
		PASS_PREP,PASS,
		EJECT_PREP,EJECT,
		CATCH, //SHOOT_LOW
		COLLECT_SPIN_UP,
	};
	std::ostream& operator<<(std::ostream& o,Mode);

	Subgoals subgoals(Mode,Drive_goal,wheelcalib);
	Toplevel::Mode to_mode(Control_status::Control_status);
}

#endif
