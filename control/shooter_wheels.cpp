#include "shooter_wheels.h"
#include<iostream>
#include<cassert>
#include<stdlib.h>
#include"wheelrpms.h"

using namespace std;

namespace Shooter_wheels{
	ostream& operator<<(ostream& o,High_level_goal g){
		switch(g){
			#define X1(name) case name: return o<<""#name;
			X1(HIGH_GOAL)
			X1(TRUSS)
			X1(PASS)
			X1(STOP)
			X1(X)
			#undef X1
			default: assert(0);
		}
	}

	ostream& operator<<(ostream& o,Output a){
		o<<"Shooter_wheel::Output(";
		for(unsigned i=0;i<2;i++){
			o<<"t"<<i<<":"<<a.top[i];
		}
		for(unsigned i=0;i<2;i++){
			o<<"b"<<i<<":"<<a.bottom[i];
		}
		return o<<")";
	}
	
	Control::Control(){
		calib = readconfig();
		if(calib.highgoal.top == 0){
			calib = rpmsdefault();
		}
	}
	
	RPM target_speed_top(High_level_goal g,wheelcalib c){
		//this stuff eventually should come out of a config file.
		switch(g){
			case TRUSS:
				return c.overtruss.top; //Previously 1200
			case HIGH_GOAL:
				return c.highgoal.top; //Previously 1200
			case PASS:
				return c.highgoal.top; //Previously 2200
			case STOP:
			case X:
				return 0;
			default: assert(0);
		}
	}

	RPM Control::target_speed_top(High_level_goal g)const{
		return Shooter_wheels::target_speed_top(g,calib);
	}
	
	RPM target_speed_bottom(High_level_goal g, wheelcalib c){
		switch(g){
			case TRUSS:
				return c.overtruss.bottom; //Previously 1200
			case HIGH_GOAL:
				return c.highgoal.bottom; //Previously 3000
			case PASS:
				return c.lowgoal.bottom; //Previously 2200
			case STOP:
			case X:
				return 0;
			default: assert(0);
		}
	}
	
	RPM Control::target_speed_bottom(High_level_goal g)const{
		return Shooter_wheels::target_speed_bottom(g,calib);
	}

	Jaguar_output open_loop(RPM status,RPM goal){
		static const double FREE_SPEED=5000;
		if(goal>FREE_SPEED) goal=FREE_SPEED;
		if(status>.8*goal) return Jaguar_output::voltageOut(goal/FREE_SPEED);
		return Jaguar_output::voltageOut(goal>1000);
	}
	
	//pair<Jag_output,Jag_output> ctl(Status 
	
	Output control(Status status,Goal goal){
		Output r;
		//Jaguar_output top=Jaguar_output::voltageOut(.25),bottom=Jaguar_output::voltageOut(.6);
		bool all_open_loop=0;
		r.top[Output::FEEDBACK]=all_open_loop?open_loop(status.top,goal.top):Jaguar_output::speedOut(goal.top);
		r.top[Output::OPEN_LOOP]=open_loop(status.top,goal.top);
		r.bottom[Output::FEEDBACK]=all_open_loop?open_loop(status.bottom,goal.bottom):Jaguar_output::speedOut(goal.bottom);
		r.bottom[Output::OPEN_LOOP]=open_loop(status.bottom,goal.bottom);
		return r;
	}
	/*
	Output control(Goal g){
		Output r;
		r.top=target_speed_top(g);
		r.bottom=target_speed_bottom(g);
		return r;
	}
	*/
	bool Control::ready(High_level_goal g,RPM top_speed,RPM bottom_speed)const{
		RPM error_top=top_speed-target_speed_top(g);
		RPM error_bot=bottom_speed-target_speed_bottom(g);
		RPM worst_error=max(abs(error_top),abs(error_bot));
		switch(g){
			case HIGH_GOAL: return worst_error<300;
			case TRUSS:
			case PASS:
			case STOP:
				return worst_error<600;
			case X:
				return 1;
			default:
				return 0;
		}
	}

	bool Control::ready(Status status,High_level_goal goal)const{
		return ready(goal,status.top,status.bottom);
	}
	
	ostream& operator<<(ostream& o,Control c){
		return o<<"Shooter_wheels::Control("<<c.calib<<")";
	}
	
	bool ready(Status,Goal){
		//this is wrong
		return 1;//assert(0);
	}
	
	Goal convert_goal(wheelcalib c,High_level_goal g){
		switch(g){
			case TRUSS:
				return c.overtruss; //Previously 1200
			case HIGH_GOAL:
				return c.highgoal; //Previously 3000
			case PASS:
				return c.lowgoal; //Previously 2200
			case STOP:
			case X:
				return Goal();
			default: assert(0);
		}
	}
}

#ifdef SHOOTER_WHEELS_TEST
#include<vector>

int main(){
	using namespace Shooter_wheels;

	static const vector<High_level_goal> GOALS{HIGH_GOAL,TRUSS,PASS,STOP,X};
	for(auto goal:GOALS){
		Control control;
		assert(control.ready(goal,target_speed_top(goal,rpmsdefault()),target_speed_bottom(goal,rpmsdefault())));
	}
}
#endif
