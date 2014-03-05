#include "jag_interface.h"
#include<iostream>
#include<cmath>

using namespace std;

Jaguar_output::Jaguar_output(){
	speed = 0;
	voltage = 0;
	controlSpeed = false;
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

bool operator==(Jaguar_output a,Jaguar_output b){
	return a.speed==b.speed && a.voltage==b.voltage && a.controlSpeed==b.controlSpeed;
}

bool operator!=(Jaguar_output a,Jaguar_output b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Jaguar_output a){
	o<<"Jaguar_output(";
	if(a.controlSpeed){
		o<<"speed="<<a.speed<<")";
	} else{
		o<<"voltage="<<a.voltage<<")";
	}
	return o;
}

Jaguar_input::Jaguar_input():speed(0),current(0){}

bool operator==(Jaguar_input a,Jaguar_input b){
	return a.speed==b.speed && a.current==b.current;
}

bool operator!=(Jaguar_input a,Jaguar_input b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Jaguar_input a){
	o<<"Jaguar_input(";
	o<<"speed:"<<a.speed;
	o<<" current:"<<a.current;
	return o<<")";
}

bool approx_equal(Jaguar_input a,Jaguar_input b){
	return a.current==b.current && fabs(a.speed-b.speed)<140;//this threshold is totally arbitrary.
}

#ifdef JAG_INTERFACE_TEST
int main(){
	Jaguar_input a;
	cout<<a<<"\n";
	Jaguar_output b;
	cout<<b<<"\n";
}
#endif
