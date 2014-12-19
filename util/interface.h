#ifndef INTERFACE_H
#define INTERFACE_H

#include<iosfwd>
#include<bitset>
#include "jag_interface.h"
#include "driver_station_interface.h"

typedef double Time;
typedef unsigned char Pwm_output;
typedef bool Solenoid_output;

typedef enum{DIO_INPUT,DIO_1,DIO_0} Digital_out;
std::ostream& operator<<(std::ostream&,Digital_out);

typedef enum{RELAY_00,RELAY_01,RELAY_10,RELAY_11} Relay_output;
std::ostream& operator<<(std::ostream&,Relay_output);

struct Robot_outputs{
	static const unsigned PWMS=10;//Number of ports on the digital sidecar
	Pwm_output pwm[PWMS];
	
	static const unsigned SOLENOIDS=8;
	Solenoid_output solenoid[SOLENOIDS];
	
	static const unsigned RELAYS=8;
	Relay_output relay[RELAYS];
	
	static const unsigned DIGITAL_IOS=14;//there are really 14
	Digital_out digital_io[DIGITAL_IOS];
	
	static const unsigned CAN_JAGUARS=0;
	Jaguar_output jaguar[CAN_JAGUARS];
	
	//could add in some setup for the analog inputs
	
	Driver_station_output driver_station;

	Robot_outputs();
};

bool operator==(Robot_outputs,Robot_outputs);
bool operator!=(Robot_outputs,Robot_outputs);
std::ostream& operator<<(std::ostream& o,Robot_outputs);

struct Joystick_data{
	static const unsigned AXES=6;//limitation of FRC coms
	double axis[AXES];
	
	static const unsigned BUTTONS=12;//limitation of FRC coms
	std::bitset<BUTTONS> button;
	
	Joystick_data();

	static Maybe<Joystick_data> parse(std::string const&);
};
bool operator==(Joystick_data,Joystick_data);
bool operator!=(Joystick_data,Joystick_data);
std::ostream& operator<<(std::ostream&,Joystick_data);

//We may need to add support for other modes at some point.
struct Robot_mode{
	bool autonomous;
	bool enabled;
	
	Robot_mode();
};
bool operator==(Robot_mode,Robot_mode);
bool operator!=(Robot_mode,Robot_mode);
std::ostream& operator<<(std::ostream&,Robot_mode);

typedef enum{DI_OUTPUT,DI_0,DI_1} Digital_in;
std::ostream& operator<<(std::ostream&,Digital_in);

typedef float Volt;
typedef double Rad; //radians, clockwise

struct Robot_inputs{
	Robot_mode robot_mode;
	Time now;//time since boot.

	static const unsigned JOYSTICKS=4; //limitation of FRC coms
	Joystick_data joystick[JOYSTICKS];

	Digital_in digital_io[Robot_outputs::DIGITAL_IOS];
	
	static const unsigned ANALOG_INPUTS=8;
	Volt analog[ANALOG_INPUTS];

	Jaguar_input jaguar[Robot_outputs::CAN_JAGUARS];
	Driver_station_input driver_station;
	Rad orientation;

	Robot_inputs();
};
bool operator==(Robot_inputs,Robot_inputs);
bool operator!=(Robot_inputs,Robot_inputs);
std::ostream& operator<<(std::ostream& o,Robot_inputs);

#endif
