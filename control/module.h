#ifndef MODULE_H
#define MODULE_H

#include "common.h"

struct Encoder_input{
	bool a,b;
};
std::ostream& operator<<(std::ostream&,Encoder_input);

struct Module_state{
	Angle angle;
	Encoder_input last;

	Module_state();
};
std::ostream& operator<<(std::ostream&,Module_state const&);

std::pair<Module_state,Portion> rotation_power(Module_state,Angle target,Encoder_input);

#endif
