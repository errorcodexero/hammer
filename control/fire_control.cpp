#include "fire_control.h"
#include<iostream>
#include<cassert>

using namespace std;

	enum Target{NONE,HIGH,TRUSS,PASS,EJECT};

	ostream& operator<<(ostream&,Target){
		assert(0);
	}

	Target target(Control_status::Control_status){
		assert(0);
	}

	enum Goal{PREP,FIRE,FIRE_WHEN_READY,DRIVE};

	ostream& operator<<(ostream&,Goal g){
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

#ifdef FIRE_CONTROL_TEST
#include<vector>

int main(){
	using namespace Fire_control;
	static const vector<Fire_control::Goal> GOALS{};
	cout<<GOALS<<"\n";
}
#endif