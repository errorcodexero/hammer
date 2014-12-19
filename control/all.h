#ifndef ALL_H
#define ALL_H

#include "high_level.h"
#include "module.h"

struct Drivebase_state{
	Module_state mod[MODULES];
};
std::ostream& operator<<(std::ostream&,Drivebase_state const&);

struct Drivebase_input{
	Encoder_input encoders[MODULES];
	Joy_in joysticks;
};
std::ostream& operator<<(std::ostream&,Drivebase_input const&);

struct Drivebase_output{
	Portion drive_module[MODULES];
	Portion turn_power[MODULES];
};
std::ostream& operator<<(std::ostream&,Drivebase_output const&);

std::pair<Drivebase_state,Drivebase_output> next(Drivebase_state,Drivebase_input);

#endif
