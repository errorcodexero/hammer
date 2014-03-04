#ifndef WHEEL_SIM_H
#define WHEEL_SIM_H

#include "../util/interface.h"
#include "shooter_status.h"
#include "shooter_wheels.h"

class Wheel_sim{
	Time last;
	Shooter_wheels::RPM est;

	public:
	Wheel_sim();
	void update(Time,double power);
	Shooter_wheels::RPM estimate()const;

	friend std::ostream& operator<<(std::ostream&,Wheel_sim);
};
std::ostream& operator<<(std::ostream&,Wheel_sim);

class Shooter_sim{
	Wheel_sim top,bottom;

	public:
	void update(Time,Shooter_wheels::Output);
	Shooter_wheels::Status estimate()const;

	friend std::ostream& operator<<(std::ostream&,Shooter_sim);
};
std::ostream& operator<<(std::ostream& o,Shooter_sim);

#endif
