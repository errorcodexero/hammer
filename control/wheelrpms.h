#ifndef WHEELRPMS_H
#define WHEELRPMS_H
#include <iosfwd>
#include "shooter_status.h"
//#include "../util/maybe.h"

struct wheelcalib{
	Shooter_wheels::Status highgoal;
	Shooter_wheels::Status lowgoal;
	Shooter_wheels::Status overtruss;
	Shooter_wheels::Status passing;
};

void configfile();
void adddefaultrpms();
void writeconfig(wheelcalib);

std::ostream& operator<<(std::ostream&,wheelcalib);

wheelcalib readconfig();
wheelcalib rpmsdefault();
wheelcalib nullvalues();
wheelcalib zerovalues();

#endif
