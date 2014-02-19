#ifndef HOLONOMIC_H
#define HOLONOMIC_H

#include "../util/point.h"
#include "gyro_tracker.h"

struct Drive_goal {
	Pt direction;
	bool field_relative;
	Drive_goal();
};
std::ostream& operator<<(std::ostream&, Drive_goal);

struct Drive_motors {
	double a,b,c;
//Outputs to motors; 
//a = frontLeft, b = frontRight, c = back;
//+value = clockwise
};
std::ostream& operator<<(std::ostream&, Drive_motors);

Drive_motors holonomic_mix(Pt);
Drive_motors holonomic_mix(double x,double y,double theta, double ortientation, bool fieldRelative);
Pt rotate_vector (double x, double y, double theta, double angle);

#endif
