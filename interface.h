#ifndef INTERFACE_H
#define INTERFACE_H

#include<iosfwd>

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
	
	static const unsigned DIGITAL_IOS=14;
	Digital_out digital_io[DIGITAL_IOS];
	
	//could add in some setup for the analog inputs
	
	Robot_outputs();
};

std::ostream& operator<<(std::ostream& o,Robot_outputs);

struct Joystick_data{
	static const unsigned AXES=6;//limitation of FRC coms
	double axis[6];
	
	static const unsigned BUTTONS=12;//limitation of FRC coms
	bool button[BUTTONS];
	
	Joystick_data();
};

std::ostream& operator<<(std::ostream&,Joystick_data);

//We may need to add support for other modes at some point.
struct Robot_mode{
	bool autonomous;
	bool enabled;
	
	Robot_mode();
};

std::ostream& operator<<(std::ostream&,Robot_mode);

typedef double Time;

static const unsigned MAX_JOYSTICKS=4; //limitation of FRC coms

typedef enum{DI_OUTPUT,DI_0,DI_1} Digital_in;
std::ostream& operator<<(std::ostream&,Digital_in);

typedef float Volt;

struct Robot_inputs{
	Robot_mode robot_mode;
	Time now;//time since boot.
	Joystick_data joystick[MAX_JOYSTICKS];
	Digital_in digital_io[Robot_outputs::DIGITAL_IOS];
	
	static const unsigned ANALOG_INPUTS=8;
	Volt analog[ANALOG_INPUTS];
	
	Robot_inputs();
};

std::ostream& operator<<(std::ostream& o,Robot_inputs);

#endif
