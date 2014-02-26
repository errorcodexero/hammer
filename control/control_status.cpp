#include "control_status.h"
#include<iostream>

using namespace std;

namespace Control_status{
	ostream& operator<<(ostream& o,Control_status c){
		#define X(name) if(c==name) return o<<""#name;
		X(AUTO_SPIN_UP)
		X(AUTO_FIRE)
		X(AUTO_TO_COLLECT)
		X(AUTO_COLLECT)
		X(AUTO_SPIN_UP2)
		X(AUTO_FIRE2)
		X(DRIVE_W_BALL)
		X(DRIVE_WO_BALL)
		X(COLLECT)
		X(SHOOT_HIGH_PREP)
		X(SHOOT_HIGH)
		X(SHOOT_HIGH_WHEN_READY)
		X(TRUSS_TOSS_PREP)
		X(TRUSS_TOSS)
		X(TRUSS_TOSS_WHEN_READY)
		X(PASS_PREP)
		X(PASS)
		X(PASS_WHEN_READY)
		X(EJECT_PREP)
		X(EJECT)
		X(EJECT_WHEN_READY)
		X(CATCH)
		#undef X
		assert(0);
	}

	vector<Control_status> all(){
		vector<Control_status> r;
		#define X(name) r.push_back(name);
		X(AUTO_SPIN_UP)
		X(AUTO_FIRE)
		X(AUTO_TO_COLLECT)
		X(AUTO_COLLECT)
		X(AUTO_SPIN_UP2)
		X(AUTO_FIRE2)
		X(DRIVE_W_BALL)
		X(DRIVE_WO_BALL)
		X(COLLECT)
		X(SHOOT_HIGH_PREP)
		X(SHOOT_HIGH)
		X(SHOOT_HIGH_WHEN_READY)
		X(TRUSS_TOSS_PREP)
		X(TRUSS_TOSS)
		X(TRUSS_TOSS_WHEN_READY)
		X(PASS_PREP)
		X(PASS)
		X(PASS_WHEN_READY)
		X(EJECT_PREP)
		X(EJECT)
		X(EJECT_WHEN_READY)
		X(CATCH)
		#undef X
		return r;
	}

	bool autonomous(Control_status s){
		return s==AUTO_SPIN_UP || s==AUTO_FIRE || s==AUTO_TO_COLLECT || s==AUTO_COLLECT || s==AUTO_SPIN_UP2 || s==AUTO_FIRE;
	}
	bool teleop(Control_status s){
		return !autonomous(s);
	}
}

#ifdef CONTROL_STATUS_TEST
int main(){
	for(auto a:Control_status::all()){
		cout<<a<<" "<<autonomous(a)<<"\n";
	}
}
#endif
