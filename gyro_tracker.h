#ifndef GYRO_TRACKER_H
#define GYRO_TRACKER_H

#include "interface.h"

class Integrator{
	Time last;
	
	public:
	double total;
	
	Integrator();
	void update(Time,double value);
	
	friend std::ostream& operator<<(std::ostream&,Integrator);
};

std::ostream& operator<<(std::ostream&,Integrator);

typedef double Degree;

class Gyro_tracker{
	double cal_accumulated;
	unsigned cal_samples;
	Time cal_start;
	Volt center;
	Integrator integrator;
	
	public:
	Gyro_tracker();
	void update(Time now,Volt);
	Degree angle()const;
	
	friend std::ostream& operator<<(std::ostream&,Gyro_tracker);
};

std::ostream& operator<<(std::ostream&,Gyro_tracker);

#endif
