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
	for(unsigned i=0;i<Driver_station_output::LCD_HEIGHT;i++){
		if(a.lcd[i]!=b.lcd[i]){
			return 0;
		}
	}
	return 1;
}

bool operator!=(Driver_station_output a,Driver_station_output b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Driver_station_output a){
	o<<"Driver_station_output(";
	o<<"digital:";
	for(unsigned i=0;i<Driver_station_output::DIGITAL_OUTPUTS;i++){
		o<<a.digital[i];
	}
	o<<" lcd:";
	for(unsigned i=0;i<Driver_station_output::LCD_HEIGHT;i++){
		o<<a.lcd[i];
		if(i+1!=Driver_station_output::LCD_HEIGHT){
			o<<"/";
		}
	}
	return o<<")";
}

#ifdef DRIVER_STATION_INTERFACE_TEST
int main(){
	Driver_station_input a;
	Driver_station_output b;
}
#endif