#include "interface.h"
#include<iostream>
#include<cassert>

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

bool operator==(Robot_outputs a,Robot_outputs b){
	for(unsigned i=0;i<Robot_outputs::PWMS;i++){
		if(a.pwm[i]!=b.pwm[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
		if(a.solenoid[i]!=b.solenoid[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Robot_outputs::RELAYS;i++){
		if(a.relay[i]!=b.relay[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		if(a.digital_io[i]!=b.digital_io[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		if(a.jaguar[i]!=b.jaguar[i]){
			return 0;
		}
	}
	return a.driver_station==b.driver_station;
}

bool operator!=(Robot_outputs a,Robot_outputs b){
	return !(a==b);
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
	o<<a.driver_station;
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

bool operator==(Joystick_data a,Joystick_data b){
	for(unsigned i=0;i<Joystick_data::AXES;i++){
		if(a.axis[i]!=b.axis[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Joystick_data::BUTTONS;i++){
		if(a.button[i]!=b.button[i]){
			return 0;
		}
	}
	return 1;
}

bool operator!=(Joystick_data a,Joystick_data b){
	return !(a==b);
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

bool operator==(Robot_mode a,Robot_mode b){
	return a.autonomous==b.autonomous && a.enabled==b.enabled;
}

bool operator!=(Robot_mode a,Robot_mode b){
	return !(a==b);
}

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

bool operator==(Robot_inputs a,Robot_inputs b){
	if(a.robot_mode!=b.robot_mode) return 0;
	if(a.now!=b.now) return 0;
	for(unsigned i=0;i<Robot_inputs::JOYSTICKS;i++){
		if(a.joystick[i]!=b.joystick[i]) return 0;
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		if(a.digital_io[i]!=b.digital_io[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Robot_inputs::ANALOG_INPUTS;i++){
		if(a.analog[i]!=b.analog[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		if(a.jaguar[i]!=b.jaguar[i]){
			return 0;
		}
	}
	return a.driver_station==b.driver_station;
}

bool operator!=(Robot_inputs a,Robot_inputs b){
	return !(a==b);
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
	o<<" jaguar:";
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		o<<a.jaguar[i];
	}
	o<<a.driver_station;
	return o<<")";
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
