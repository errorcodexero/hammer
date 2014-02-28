#include "panel2014.h"
#include<stdlib.h>

using namespace std;

Mode_buttons::Mode_buttons():
	drive_wo_ball(0),
	drive_w_ball(0),
	collect(0),
	shoot_high(0),
	truss_toss(0),
	pass(0),
	eject(0),
	catch_mode(0)
{}

ostream& operator<<(ostream& o,Mode_buttons m){
	o<<"Mode_buttons( ";
	#define X(name) o<<""#name<<":"<<m.name<<" ";
	X(drive_wo_ball)
	X(drive_w_ball)
	X(collect)
	X(shoot_high)
	X(truss_toss)
	X(pass)
	X(eject)
	X(catch_mode)
	#undef X
	return o<<")";
}

Panel::Panel():
	fire(0),
	target(Fire_control::NO_TARGET),
	speed(0),
	learn(0)
{}

ostream& operator<<(ostream& o,Panel p){
	o<<"Panel( ";
	#define X(name) o<<""#name<<":"<<p.name<<" ";
	X(mode_buttons)
	X(fire)
	X(target)
	X(speed)
	X(learn)
	X(collector)
	X(collector_tilt)
	X(injector)
	X(injector_arms)
	X(ejector)
	#undef X
	return o<<")";
}

int demux27(double analog){
	assert(0);
}

Fire_control::Target interpret_target(bool a,bool b,bool c){
	int x=a+2*b+4*c;
	switch(x){
		case 0: return Fire_control::NO_TARGET;
		case 1: return Fire_control::HIGH;
		case 2: return Fire_control::TRUSS;
		case 3: return Fire_control::PASS;
		case 4: return Fire_control::EJECT;
		default:
			//may want to add some way of returning an error;
			return Fire_control::NO_TARGET;
	}
}

Maybe<Collector_mode> interpret_collector(double analog){
	//assuming that analog goes from 0 to 1.  This is likely not right, I think it may be actual voltages.
	int x=analog*4;
	if(analog==4) analog=3;
	switch(x){
		case 0: return Maybe<Collector_mode>();
		case 1: return Maybe<Collector_mode>(ON);
		case 2: return Maybe<Collector_mode>(OFF);
		case 3: return Maybe<Collector_mode>(REVERSE);
		default:
			//may want to add error detection here.
			return Maybe<Collector_mode>();
	}
}

pair<int,int> demux_3x3(double analog){
	int x=(int)analog*9;
	if(x==9) x=8;
	return make_pair(x%3,x/3);
}

Maybe<Collector_tilt::Output> interpret_collector_tilt(int x){
	switch(x){
		case 0: return Maybe<Collector_tilt::Output>();
		case 1: return Maybe<Collector_tilt::Output>(Collector_tilt::OUTPUT_UP);
		case 2: return Maybe<Collector_tilt::Output>(Collector_tilt::OUTPUT_DOWN);
		default: assert(0);
	}
}

Maybe<Injector::Output> interpret_injector(int x){
	switch(x){
		case 0: return Maybe<Injector::Output>();
		case 1: return Maybe<Injector::Output>(Injector::OUTPUT_DOWN);
		case 2: return Maybe<Injector::Output>(Injector::OUTPUT_UP);
		default: assert(0);
	}
}

Maybe<Injector_arms::Output> interpret_injector_arms(int x){
	switch(x){
		case 0: return Maybe<Injector_arms::Output>();
		case 1: return Maybe<Injector_arms::Output>(Injector_arms::OUTPUT_OPEN);
		case 2: return Maybe<Injector_arms::Output>(Injector_arms::OUTPUT_CLOSE);
		default: assert(0);
	}
}

Maybe<Ejector::Output> interpret_ejector(int x){
	switch(x){
		case 0: return Maybe<Ejector::Output>();
		case 1: return Maybe<Ejector::Output>(Ejector::OUTPUT_UP);
		case 2: return Maybe<Ejector::Output>(Ejector::OUTPUT_DOWN);
		default: assert(0);
	}
}

Panel interpret(Driver_station_input d){
	Panel panel;
	//all these assignments are just made up.  
	unsigned combined=0;
	for(unsigned i=0;i<4;i++){
		combined<<=1;
		combined|=d.digital[i];
	}
	panel.mode_buttons.drive_wo_ball=combined==1;
	panel.mode_buttons.drive_w_ball=combined==2;
	panel.mode_buttons.collect=combined==3;
	panel.mode_buttons.shoot_high=combined==4;
	panel.mode_buttons.truss_toss=combined==5;
	panel.mode_buttons.pass=combined==6;
	panel.mode_buttons.eject=combined==7;
	panel.mode_buttons.catch_mode=combined==8;

	panel.fire=combined==9;

	//5 options?
	panel.target=interpret_target(d.digital[4],d.digital[5],d.digital[6]);
	panel.speed=d.analog[0];
	panel.learn=combined==10;
	
	//4 options? maybe 3.
	panel.collector=interpret_collector(d.analog[1]);

	{
		pair<int,int> p=demux_3x3(d.analog[2]);
		//3 options
		panel.collector_tilt=interpret_collector_tilt(p.first);

		//3 options
		panel.injector=interpret_injector(p.second);
	}

	{
		pair<int,int> p=demux_3x3(d.analog[3]);
		//3 options
		panel.injector_arms=interpret_injector_arms(p.first);

		//3 options
		panel.ejector=interpret_ejector(p.second);
	}
	return panel;
}

#ifdef PANEL2014_TEST

//this should probably be moved to interface.cpp
Driver_station_input driver_station_input_rand(){
	Driver_station_input r;
	for(unsigned i=0;i<r.ANALOG_INPUTS;i++){
		r.analog[i]=(0.0+rand()%101)/100;
	}
	for(unsigned i=0;i<r.DIGITAL_INPUTS;i++){
		r.digital[i]=rand()%2;
	}
	return r;
}

int main(){
	Panel p;
	cout<<p<<"\n";
	cout<<interpret(Driver_station_input())<<"\n";
	for(unsigned i=0;i<50;i++){
		interpret(driver_station_input_rand());
	}
}
#endif