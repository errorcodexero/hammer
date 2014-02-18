#include "shooter_wheels.h"
#include<iostream>
#include<cassert>
#include<stdlib.h>

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

	Status::Status():top(0),bottom(0){}

	Status::Status(RPM a,RPM b):top(a),bottom(b){}

	ostream& operator<<(ostream& o,Status s){
		return o<<"Shooter_wheels::Status("<<s.top<<","<<s.bottom<<")";
	}

	RPM target_speed_top(Goal g){
		//this stuff eventually should come out of a config file.
		switch(g){
			case TRUSS:
				return 1200;
			case HIGH_GOAL:
				return 1200;
			case PASS:
				return 2200;
			case STOP:
			case X:
				return 0;
			default: assert(0);
		}
	}

	RPM target_speed_bottom(Goal g){
		switch(g){
			case TRUSS:
				return 1200;
			case HIGH_GOAL:
				return 3000;
			case PASS:
				return 2200;
			case STOP:
			case X:
				return 0;
			default: assert(0);
		}
	}

	Output control(Goal g){
		Output r;
		r.top=target_speed_top(g);
		r.bottom=target_speed_bottom(g);
		return r;
	}

	bool ready(Goal g,RPM top_speed,RPM bottom_speed){
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

	bool ready(Status status,Goal goal){
		return ready(goal,status.top,status.bottom);
	}
}

#ifdef SHOOTER_WHEELS_TEST
#include<vector>

int main(){
	using namespace Shooter_wheels;

	static const vector<Goal> GOALS{HIGH_GOAL,TRUSS,PASS,STOP,X};
	for(auto goal:GOALS){
		assert(ready(goal,target_speed_top(goal),target_speed_bottom(goal)));
	}
}
#endif
