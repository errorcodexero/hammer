#include "shooter_wheels.h"
#include<iostream>
#include<cassert>
#include<stdlib.h>
#include"wheelrpms.h"

using namespace std;

namespace Shooter_wheels{
	ostream& operator<<(ostream& o,Goal g){
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
	
	Control::Control(){
		calib = readconfig();
		if(calib.highgoal.top == 0){
			calib = rpmsdefault();
		}
	}
	
	RPM target_speed_top(Goal g,wheelcalib c){
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

	RPM Control::target_speed_top(Goal g)const{
		return Shooter_wheels::target_speed_top(g,calib);
	}
	
	RPM target_speed_bottom(Goal g, wheelcalib c){
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
	
	RPM Control::target_speed_bottom(Goal g)const{
		return Shooter_wheels::target_speed_bottom(g,calib);
	}
	
	Output Control::control(Goal g)const{
		Output r;
		r.top=target_speed_top(g);
		r.bottom=target_speed_bottom(g);
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
	bool Control::ready(Goal g,RPM top_speed,RPM bottom_speed)const{
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

	bool Control::ready(Status status,Goal goal)const{
		return ready(goal,status.top,status.bottom);
	}
	
	ostream& operator<<(ostream& o,Control c){
		return o<<"Shooter_wheels::Control("<<c.calib<<")";
	}
}

#ifdef SHOOTER_WHEELS_TEST
#include<vector>

int main(){
	using namespace Shooter_wheels;

	static const vector<Goal> GOALS{HIGH_GOAL,TRUSS,PASS,STOP,X};
	for(auto goal:GOALS){
		Control control;
		assert(control.ready(goal,target_speed_top(goal,rpmsdefault()),target_speed_bottom(goal,rpmsdefault())));
	}
}
#endif
