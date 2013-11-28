#include "force.h"
#include<iostream>

using namespace std;

Force::Force(){
	for(unsigned i=0;i<Robot_outputs::PWMS;i++){
		pwm[i]=0;
	}
	for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
		solenoid[i]=0;
	}
	for(unsigned i=0;i<Robot_outputs::RELAYS;i++){
		relay[i]=0;
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		digital_io[i]=0;
	}
}

Robot_outputs Force::operator()(Robot_outputs r)const{
	for(unsigned i=0;i<Robot_outputs::PWMS;i++){
		if(pwm[i]){
			r.pwm[i]=values.pwm[i];
		}
	}
	for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
		if(solenoid[i]) r.solenoid[i]=values.solenoid[i];
	}
	for(unsigned i=0;i<Robot_outputs::RELAYS;i++){
		if(relay[i]) r.relay[i]=values.relay[i];
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		if(digital_io[i]) r.digital_io[i]=values.digital_io[i];
	}
	return r;
}

ostream& operator<<(ostream& o,Force f){
	o<<"Force( ";
	for(unsigned i=0;i<Robot_outputs::PWMS;i++){
		if(f.pwm[i]){
			o<<"pwm"<<i<<"="<<(int)f.values.pwm[i]<<" ";
		}
	}
	for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
		if(f.solenoid[i]){
			o<<"solenoid"<<i<<"="<<f.values.solenoid[i]<<" ";
		}
	}
	for(unsigned i=0;i<Robot_outputs::RELAYS;i++){
		if(f.relay[i]){
			o<<"relay"<<i<<"="<<f.values.relay[i]<<" ";
		}
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		if(f.digital_io[i]){
			o<<"dio"<<i<<"="<<f.values.digital_io[i]<<" ";
		}
	}
	return o<<")";
}
