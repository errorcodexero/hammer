//Author: Logan Traffas and Andrew Kassab.
#include "scooper_control.h"
#include <iostream>
#include <vector>
#include <assert.h>

using namespace std;

ostream& operator<<(ostream & o, Scooper_control b){//How to print this type
	if(b==Scooper_control::UP){
		o<<"UP";
	}
	else if(b==Scooper_control::STOP){
		o<<"STOP";
	}
	else if(b==Scooper_control::DOWN){
		o<<"DOWN";
	}
	return o;
}

ostream& operator<<(ostream & o, Sensor_states b){//How to print this type
	if(b==Sensor_states::UP){
		o<<"UP";
	}
	else if(b==Sensor_states::DOWN){
		o<<"DOWN";
	}
	else if(b==Sensor_states::INBETWEEN){
		o<<"INBETWEEN";
	}
	else if(b==Sensor_states::ERROR){
		o<<"ERROR";
	}
	return o;
}

template<typename T>
ostream& operator<<(ostream & o, vector<T> v){//How to print the vector
	for(unsigned int i=0; i<v.size(); i++){
		o<<v[i]<<" ";
	}
	return o;
}

Scooper_control motor_control(Scooper_control scooper_control, Sensor_states sensor_states){//Determines what to do with the motor.
	if(scooper_control==Scooper_control::STOP){
		return Scooper_control::STOP;
	}
	if(scooper_control==Scooper_control::UP && sensor_states!=Sensor_states::UP && sensor_states!=Sensor_states::ERROR){
		return Scooper_control::UP;
	}
	if(scooper_control==Scooper_control::DOWN && sensor_states!=Sensor_states::DOWN && sensor_states!=Sensor_states::ERROR){
		return Scooper_control::DOWN;
	}
	if(scooper_control==Scooper_control::UP && sensor_states==Sensor_states::UP){
		return Scooper_control::STOP;
	}
	if(scooper_control==Scooper_control::DOWN && sensor_states==Sensor_states::DOWN){
		return Scooper_control::STOP;
	}
	if(sensor_states==Sensor_states::DOWN){
		return Scooper_control::STOP;
	}
	if(sensor_states==Sensor_states::UP){
		return Scooper_control::STOP;
	}
	if(sensor_states==Sensor_states::ERROR){
		return Scooper_control::STOP;
	}	
	assert(0);
}
#ifdef SCOOPER_CONTROL_TEST
int main(){
	Scooper_control motor_state=Scooper_control::STOP;//Declares all possible values for testing
	vector<Scooper_control> v_scooper_control;
	vector<Sensor_states> v_sensor_states;
	v_scooper_control.push_back(Scooper_control::UP);
	v_scooper_control.push_back(Scooper_control::DOWN);
	v_scooper_control.push_back(Scooper_control::STOP);
	v_sensor_states.push_back(Sensor_states::UP);
	v_sensor_states.push_back(Sensor_states::DOWN);
	v_sensor_states.push_back(Sensor_states::INBETWEEN);
	v_sensor_states.push_back(Sensor_states::ERROR);
	for(unsigned int i=0; i<v_scooper_control.size(); i++){
		Scooper_control i_v_scooper_control=v_scooper_control[i];
		for(unsigned int i=0; i<v_sensor_states.size(); i++){
			Sensor_states i_v_sensor_states=v_sensor_states[i];
			motor_state=motor_control(i_v_scooper_control, i_v_sensor_states);
			cout<<i_v_scooper_control<<" ";
			cout<<i_v_sensor_states<<" ";
			cout<<motor_state<<" ";
			cout<<endl;
		}
	}
	return 0;
}
#endif