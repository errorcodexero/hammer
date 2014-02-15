#include "fire_control.h"
#include<iostream>
#include<cassert>

using namespace std;
namespace Fire_control{
	ostream& operator<<(ostream& o,Target t){
		#define X(name) if(t==name) return o<<""#name;
		X(NONE)
		X(HIGH)
		X(TRUSS)
		X(PASS)
		X(EJECT)
		#undef X
		assert(0);
	}

	Target target(Control_status::Control_status){
		assert(0);
	}

	ostream& operator<<(ostream& o,Goal g){
		#define X(name) if(g==name) return o<<""#name;
		X(PREP)
		X(FIRE)
		X(FIRE_WHEN_READY)
		X(DRIVE)
		X(OTHER)
		#undef X
		assert(0);
	}

	Goal goal(Control_status::Control_status){
		assert(0);
	}

	Control_status::Control_status generate_status(Target target,Goal goal){
		switch(target){
			case NONE:
			case HIGH:
			case TRUSS:
			case EJECT:
			default: assert(0);
		}
	}

	Goal next_goal(Goal g,bool ready,bool fired,bool prep_button,bool fire_button,bool fire_when_ready_button){
		if(fire_button) return FIRE;
		switch(g){
			case PREP:
				return fire_when_ready_button?FIRE_WHEN_READY:PREP;
			case FIRE:
				return fired?DRIVE:FIRE;
			case FIRE_WHEN_READY:
				if(ready) return FIRE;
				if(fire_when_ready_button) return FIRE_WHEN_READY;
				return PREP;
			case DRIVE://why are we in here?
			default: assert(0);
		}
	}

	Control_status::Control_status next(Control_status::Control_status cs,bool ready,bool fired,bool prep_button,bool fire_button,bool fire_when_ready_button){
		Target t=target(cs);
		Goal g=goal(cs);
		cout<<t<<" "<<g<<"\n";
		assert(0);
	}
}

#ifdef FIRE_CONTROL_TEST
#include<vector>

int main(){
	using namespace Fire_control;
	static const vector<Fire_control::Goal> GOALS{PREP,FIRE,FIRE_WHEN_READY,DRIVE,OTHER};
	cout<<GOALS<<"\n";
	static const vector<Target> TARGETS{NONE,HIGH,TRUSS,PASS,EJECT};
	cout<<TARGETS<<"\n";
	for(auto goal:GOALS){
		for(auto target:TARGETS){
			//cout<<goal<<" "<<target<<generate_status(
		}
	}
}
#endif
