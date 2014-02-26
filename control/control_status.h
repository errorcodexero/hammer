#ifndef CONTROL_STATUS_H
#define CONTROL_STATUS_H
#include <iosfwd>
#include <assert.h>
#include<vector>

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
	std::vector<Control_status> all();
	bool autonomous(Control_status);
	bool teleop(Control_status);
}
#endif
