#ifndef HIGH_LEVEL_H
#define HIGH_LEVEL_H

#include <iosfwd>
#include "common.h"

struct Module_target{
	Angle angle;
	Portion power;
};
std::ostream& operator<<(std::ostream&,Module_target);

#define MODULES 3

struct Drivebase_target{
	Module_target mod[MODULES];
};
std::ostream& operator<<(std::ostream&,Drivebase_target const&);

struct Joy_in{
	Portion x,y,theta;
};
std::ostream& operator<<(std::ostream&,Joy_in);

Drivebase_target high_level(Joy_in);


struct Testcase{
	Joy_in in;
	Drivebase_target out;
};

#endif
