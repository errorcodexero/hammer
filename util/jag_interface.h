#ifndef JAG_INTERFACE_H
#define JAG_INTERFACE_H

#include<iosfwd>
#include "maybe.h"

struct Jaguar_output{
	double speed,voltage;
	float p;
	float i;
	float d;
	bool controlSpeed;
	//0 = voltage
	//1 = speed

	Jaguar_output();
	static Jaguar_output speedOut(double);
	static Jaguar_output voltageOut(double);
	static Maybe<Jaguar_output> parse(std::string const&);
};
bool operator==(Jaguar_output,Jaguar_output);
bool operator!=(Jaguar_output,Jaguar_output);
std::ostream& operator<<(std::ostream&,Jaguar_output);

struct Jaguar_input{
	double speed,current;

	Jaguar_input();

	static Maybe<Jaguar_input> parse(std::string const&);
};
bool operator==(Jaguar_input,Jaguar_input);
bool operator!=(Jaguar_input,Jaguar_input);
std::ostream& operator<<(std::ostream&,Jaguar_input);
bool approx_equal(Jaguar_input,Jaguar_input);

#endif
