#include "monitor.h"
#include "gyro_tracker.h"
#include "toplevel.h"
#include "main.h"
#include "../util/interface.h"

using namespace std;

void print_diff(ostream& o,unsigned char a,unsigned char b){
	print_diff(o,(int)a,(int)b);
}

void print_diff(ostream& o,Gyro_tracker const& a,Gyro_tracker const& b){ print_diff_approx(o,a,b); }

void print_diff(ostream& o,Toplevel::Status const& a,Toplevel::Status const& b){
	#define X(name) print_diff(o,""#name ": ",a.name,b.name);
	X(collector_tilt)
	X(injector)
	X(injector_arms)
	X(ejector)
	print_diff_approx(o,a.shooter_wheels,b.shooter_wheels);
	X(pump)
	//X(orientation)
	//print_diff_approx(o,a.orientation,b.orientation);
	#undef X
}

void print_diff(ostream& o,Toplevel::Estimator const& a,Toplevel::Estimator const& b){
	print_diff(o,a.estimate(),b.estimate());
}

void print_diff(ostream& o,Main const& a,Main const& b){
	#define X(name) print_diff(o,""#name ": ",a.name,b.name);
	#define Y(name) print_diff(o,a.name,b.name);
	X(force)
	Y(gyro);
	Y(est)
	X(control_status)
	X(ball_collecter)
	X(field_relative)
	#undef Y
	#undef X
}

void print_diff(ostream& o,Driver_station_output const& a,Driver_station_output const& b){
	#define X(name) print_diff(o,a.name,b.name);
	//X(lcd) skipping this for now since it really clutters the prinout right now since it shows shooter wheel speeds
	for(unsigned i=0;i<Driver_station_output::DIGITAL_OUTPUTS;i++){
		if(a.digital[i]!=b.digital[i]){
			o<<"Driver_station_output::digital["<<i<<"]:"<<a.digital[i]<<"->"<<b.digital[i]<<"\n";
		}
	}
	#undef X
}

void print_diff(ostream& o,Robot_outputs const& a,Robot_outputs const& b){
	for(unsigned i=0;i<Robot_outputs::PWMS;i++){
		if(a.pwm[i]!=b.pwm[i]){
			o<<"pwm"<<i<<" "<<(int)a.pwm[i]<<"->"<<(int)b.pwm[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
		if(a.solenoid[i]!=b.solenoid[i]){
			o<<"solenoid"<<i<<" "<<a.solenoid[i]<<"->"<<b.solenoid[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::RELAYS;i++){
		if(a.relay[i]!=b.relay[i]){
			o<<"relay"<<i<<" "<<a.relay[i]<<"->"<<b.relay[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		if(a.digital_io[i]!=b.digital_io[i]){
			o<<"digital_io"<<i<<" "<<a.digital_io[i]<<"->"<<b.digital_io[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		if(a.jaguar[i]!=b.jaguar[i]){
			o<<"jaguar"<<i<<" "<<a.jaguar[i]<<"->"<<b.jaguar[i]<<"\n";
		}
	}
	print_diff(o,a.driver_station,b.driver_station);
}

void print_diff(ostream& o,string const& s,Jaguar_input const& a,Jaguar_input const& b){
	if(!approx_equal(a,b)){
		o<<s<<a<<"->"<<b<<"\n";
	}
}

void print_diff(ostream& o,Robot_inputs const& a,Robot_inputs const& b){
	#define X(name) print_diff(o,""#name ": ",a.name,b.name);
	X(robot_mode)
	//X(now)
	for(unsigned i=0;i<Robot_inputs::JOYSTICKS;i++){
		X(joystick[i])
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		X(digital_io[i])
	}
	for(unsigned i=0;i<Robot_inputs::ANALOG_INPUTS;i++){
		X(analog[i])
	}
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		X(jaguar[i])
	}
	X(driver_station)
	#undef X
}

#ifdef MONITOR_TEST
int main(){}
#endif