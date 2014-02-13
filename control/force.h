//Code to override values for outputs
#ifndef FORCE_H
#define FORCE_H

#include "../util/interface.h"

struct Force{
	Robot_outputs values;
	//Each bool sets whether or not the value is overridden by Force
	bool pwm[Robot_outputs::PWMS];
	bool solenoid[Robot_outputs::SOLENOIDS];
	bool relay[Robot_outputs::RELAYS];
	bool digital_io[Robot_outputs::DIGITAL_IOS];
	
	Force();
	
	Robot_outputs operator()(Robot_outputs)const;
};

std::ostream& operator<<(std::ostream&,Force);

#endif
