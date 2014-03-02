#include "driver_station_interface.h"
#include<iostream>

using namespace std;

Driver_station_input::Driver_station_input(){
	for(unsigned i=0;i<ANALOG_INPUTS;i++) analog[i]=0;
	for(unsigned i=0;i<DIGITAL_INPUTS;i++) digital[i]=0;
}

bool operator==(Driver_station_input a,Driver_station_input b){
	for(unsigned i=0;i<Driver_station_input::ANALOG_INPUTS;i++){
		if(a.analog[i]!=b.analog[i]){
			return 0;
		}
	}
	for(unsigned i=0;i<Driver_station_input::DIGITAL_INPUTS;i++){
		if(a.digital[i]!=b.digital[i]){
			return 0;
		}
	}
	return 1;
}

bool operator!=(Driver_station_input a,Driver_station_input b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Driver_station_input a){
	o<<"Driver_station_input(";
	o<<"analog:";
	for(unsigned i=0;i<Driver_station_input::ANALOG_INPUTS;i++){
		o<<a.analog[i]<<" ";
	}
	o<<"digital:";
	for(unsigned i=0;i<Driver_station_input::DIGITAL_INPUTS;i++){
		o<<a.digital[i];
	}
	return o<<")";
}

bool operator==(Driver_station_output::Lcd a,Driver_station_output::Lcd b){
	for(unsigned i=0;i<Driver_station_output::Lcd::HEIGHT;i++){
		if(a.line[i]!=b.line[i]){
			return 0;
		}
	}
	return 1;
}

Driver_station_output::Driver_station_output(){
	for(unsigned i=0;i<DIGITAL_OUTPUTS;i++){
		digital[i]=0;
	}
}

bool operator==(Driver_station_output a,Driver_station_output b){
	for(unsigned i=0;i<Driver_station_output::DIGITAL_OUTPUTS;i++){
		if(a.digital[i]!=b.digital[i]){
			return 0;
		}
	}
	return a.lcd==b.lcd;
}

bool operator!=(Driver_station_output a,Driver_station_output b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Driver_station_output::Lcd a){
	o<<"lcd(";
	for(unsigned i=0;i<Driver_station_output::Lcd::HEIGHT;i++){
		o<<a.line[i];
		if(i+1!=Driver_station_output::Lcd::HEIGHT){
			o<<"/";
		}
	}
	return o<<")";
}

ostream& operator<<(ostream& o,Driver_station_output a){
	o<<"Driver_station_output(";
	o<<"digital:";
	for(unsigned i=0;i<Driver_station_output::DIGITAL_OUTPUTS;i++){
		o<<a.digital[i];
	}
	o<<a.lcd;
	return o<<")";
}

#ifdef DRIVER_STATION_INTERFACE_TEST
int main(){
	Driver_station_input a;
	Driver_station_output b;
}
#endif