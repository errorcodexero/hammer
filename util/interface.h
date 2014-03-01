#ifndef INTERFACE_H
#define INTERFACE_H

#include<iosfwd>
#include<string>

typedef double Time;
typedef unsigned char Pwm_output;
typedef bool Solenoid_output;

struct Jaguar_output{
	double speed,voltage;
	bool controlSpeed;
	//0 = voltage
	//1 = speed

	Jaguar_output();
	static Jaguar_output speedOut(double);
	static Jaguar_output voltageOut(double);
};
bool operator==(Jaguar_output,Jaguar_output);
bool operator!=(Jaguar_output,Jaguar_output);
std::ostream& operator<<(std::ostream&,Jaguar_output);

struct Jaguar_input{
	double speed,current;

	Jaguar_input();
};
bool operator==(Jaguar_input,Jaguar_input);
bool operator!=(Jaguar_input,Jaguar_input);
std::ostream& operator<<(std::ostream&,Jaguar_input);

typedef enum{DIO_INPUT,DIO_1,DIO_0} Digital_out;
std::ostream& operator<<(std::ostream&,Digital_out);

typedef enum{RELAY_00,RELAY_01,RELAY_10,RELAY_11} Relay_output;
std::ostream& operator<<(std::ostream&,Relay_output);

struct Driver_station_output{
	static const unsigned DIGITAL_OUTPUTS=8;
	bool digital[DIGITAL_OUTPUTS];

	static const unsigned LCD_HEIGHT=6;
	std::string lcd[LCD_HEIGHT];

	Driver_station_output();
};
bool operator==(Driver_station_output,Driver_station_output);
bool operator!=(Driver_station_output,Driver_station_output);
std::ostream& operator<<(std::ostream&,Driver_station_output);

struct Robot_outputs{
	static const unsigned PWMS=10;//Number of ports on the digital sidecar
	Pwm_output pwm[PWMS];
	
	static const unsigned SOLENOIDS=8;
	Solenoid_output solenoid[SOLENOIDS];
	
	static const unsigned RELAYS=8;
	Relay_output relay[RELAYS];
	
	static const unsigned DIGITAL_IOS=4;//there are really 14
	Digital_out digital_io[DIGITAL_IOS];
	
	static const unsigned CAN_JAGUARS=4;
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
	double axis[6];
	
	static const unsigned BUTTONS=12;//limitation of FRC coms
	bool button[BUTTONS];
	
	Joystick_data();
};
bool operator==(Joystick_data,Joystick_data);
bool operator!=(Joystick_data,Joystick_data);
std::ostream& operator<<(std::ostream&,Joystick_data);

struct Driver_station_input{
	//In the traditional mode of the Cybress board there are only 4 available, but there are 8 in the 'enhanced' mode
	static const unsigned ANALOG_INPUTS=8;
	double analog[ANALOG_INPUTS];

	static const unsigned DIGITAL_INPUTS=8;
	bool digital[DIGITAL_INPUTS];

	Driver_station_input();
};
bool operator==(Driver_station_input,Driver_station_input);
bool operator!=(Driver_station_input,Driver_station_input);
std::ostream& operator<<(std::ostream&,Driver_station_input);

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

	Robot_inputs();
};
bool operator==(Robot_inputs,Robot_inputs);
bool operator!=(Robot_inputs,Robot_inputs);
std::ostream& operator<<(std::ostream& o,Robot_inputs);

#endif
