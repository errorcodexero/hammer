#include "interface.h"
#include<iostream>

using namespace std;

std::ostream& operator<<(std::ostream& o,Digital_out a){
	switch(a){
		#define X(name) case name: return o<<""#name;
		X(DIO_INPUT)
		X(DIO_0)
		X(DIO_1)
		#undef X
		default: return o<<"?";
	}
}

void terse(ostream& o,Digital_out d){
	switch(d){
		case DIO_INPUT:
			o<<'i';
			break;
		case DIO_0:
			o<<'0';
			break;
		case DIO_1:
			o<<'1';
			break;
		default:
			o<<'?';
	}
}

std::ostream& operator<<(std::ostream& o,Relay_output a){
	switch(a){
		#define X(name) case name: return o<<""#name;
		X(RELAY_00)
		X(RELAY_01)
		X(RELAY_10)
		X(RELAY_11)
		#undef X
		default: return o<<"?";
	}
}

void terse(ostream& o,Relay_output a){
	switch(a){
		case RELAY_00:
			o<<'0';
			break;
		case RELAY_01:
			o<<'R';
			break;
		case RELAY_10:
			o<<'F';
			break;
		case RELAY_11:
			o<<"+";
			break;
		default:
			o<<"?";
	}
}

Robot_outputs::Robot_outputs(){
	for(unsigned i=0;i<PWMS;i++){
		pwm[i]=0;
	}
	for(unsigned i=0;i<SOLENOIDS;i++){
		solenoid[i]=0;
	}
	for(unsigned i=0;i<RELAYS;i++){
		relay[i]=RELAY_00;
	}
	for(unsigned i=0;i<DIGITAL_IOS;i++){
		digital_io[i]=DIO_INPUT;
	}
}

ostream& operator<<(ostream& o,Robot_outputs a){
	o<<"Robot_outputs(";
	o<<"pwm:";
	for(unsigned i=0;i<a.PWMS;i++){
		o<<(int)a.pwm[i]<<" ";
	}
	o<<"solenoid:";
	for(unsigned i=0;i<a.SOLENOIDS;i++){
		o<<a.solenoid[i];
	}
	o<<" relay:";
	for(unsigned i=0;i<a.RELAYS;i++){
		//o<<a.relay[i];
		terse(o,a.relay[i]);
	}
	o<<" dio:";
	for(unsigned i=0;i<a.DIGITAL_IOS;i++){
		//o<<a.digital_io[i];
		terse(o,a.digital_io[i]);
	}
	o<<" jaguar:";
	for(unsigned i=0;i<a.CAN_JAGUARS;i++){
		o<<a.jaguar[i];
	}
	return o<<")";
}

Joystick_data::Joystick_data(){
	for(unsigned i=0;i<AXES;i++){
		axis[i]=0;
	}
	for(unsigned i=0;i<BUTTONS;i++){
		button[i]=0;
	}
}

ostream& operator<<(ostream& o,Joystick_data a){
	o<<"Joystick_data(";
	o<<"axes:";
	for(unsigned i=0;i<Joystick_data::AXES;i++){
		o<<a.axis[i]<<" ";
	}
	o<<"buttons:";
	for(unsigned i=0;i<Joystick_data::BUTTONS;i++){
		o<<a.button[i]<<" ";
	}
	return o<<")";
}

Robot_mode::Robot_mode():autonomous(0),enabled(0){}

ostream& operator<<(ostream& o,Robot_mode m){
	/*switch(m){
		#define X(name) case name: return o<<""#name;
		X(ROBOT_MODE_AUTO)
		X(ROBOT_MODE_TELE)
		#undef X
		default: return o<<"ERROR";
	}*/
	o<<"Robot_mode(";
	o<<"auto="<<m.autonomous;
	o<<" en="<<m.enabled;
	return o<<")";
}

ostream& operator<<(ostream& o,Digital_in d){
	switch(d){
		#define X(name) case name: return o<<""#name;
		X(DI_OUTPUT)
		X(DI_0)
		X(DI_1)
		#undef X
		default: return o<<"error";
	}
}

void terse(ostream& o,Digital_in d){
	switch(d){
		case DI_OUTPUT:
			o<<'.';
			break;
		case DI_0:
			o<<'0';
			break;
		case DI_1:
			o<<'1';
			break;
		default:
			o<<'?';
	}
}

Robot_inputs::Robot_inputs():
	now(0)
{
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		digital_io[i]=DI_OUTPUT;
	}
	
	for(unsigned i=0;i<ANALOG_INPUTS;i++){
		//could make this be NAN instead
		analog[i]=0;
	}
}

ostream& operator<<(ostream& o,Robot_inputs a){
	o<<"Robot_inputs(\n";
	o<<"  mode="<<a.robot_mode<<"\n";
	o<<"  now="<<a.now<<"\n";
	//o<<"joysticks:";
	for(unsigned i=0;i<a.JOYSTICKS;i++){
		o<<"  "<<a.joystick[i]<<"\n";
	}
	o<<" dio:";
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		terse(o,a.digital_io[i]);
	}
	o<<" analog:";
	for(unsigned i=0;i<a.ANALOG_INPUTS;i++){
		o<<(i+1)<<" "<<a.analog[i]<<' ';
	}
	return o<<")";
}

Jaguar_output::Jaguar_output(){
	speed = 0;
	voltage = 0;
	controlSpeed = false;
}
ostream& operator<<(ostream& o,Jaguar_output a){
	o<<"Jaguar_output(";
	if(a.controlSpeed){
		o<<"speed ="<<a.speed<<")\n";
	} else{
		o<<"voltage ="<<a.voltage<<")\n";
	}
	return o;
}
Jaguar_output Jaguar_output::speedOut(double a){
	Jaguar_output j;
	j.controlSpeed = true;
	j.speed = a;
	return j;
}
Jaguar_output Jaguar_output::voltageOut(double a){
	Jaguar_output j;
	j.controlSpeed = false;
	j.voltage = a;
	return j;
}

#ifdef INTERFACE_TEST
int main(){
	cout<<Robot_outputs()<<"\n";
	cout<<Robot_inputs()<<"\n";
	cout<<Joystick_data()<<"\n";
	cout<<Jaguar_output()<<"\n";
	cout<<Jaguar_output::speedOut(10)<<"\n";
	cout<<Jaguar_output::voltageOut(1.0)<<"\n";
}
#endif
