#ifndef WHEELRPMS_H
#define WHEELRPMS_H
#include <iosfwd>
#include "shooter_wheels.h"
struct wheelcalib{
	Shooter_wheels::Status highgoal;
	Shooter_wheels::Status lowgoal;
	Shooter_wheels::Status overtruss;
	Shooter_wheels::Status passing;
};
std::ostream& operator<<(std::ostream&,wheelcalib);
void configfile();
void adddefaultrpms();
void writeconfig(wheelcalib);
wheelcalib readconfig();	
wheelcalib rpmsdefault();
#endif