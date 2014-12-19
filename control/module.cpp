#include "module.h"
#include<math.h>

using namespace std;

ostream& operator<<(ostream& o,Encoder_input a){
	return o<<"Enc("<<a.a<<a.b<<")";
}

//Module_state::Module_state():angle(0),{0, 0}{}

ostream& operator<<(ostream& o,Module_state const& a){
	o<<"Module_state(";
	o<<a.angle<<","<<a.last;
	return o<<")";
}

pair<Module_state,Portion> rotation_power(Module_state state,Angle target,Encoder_input enc){
	state.last=enc;
	Angle to_go=normalize_angle(state.angle-target);
	Portion power;
	static const Angle ERROR_THRESHOLD=.2;//radians
	if(to_go>=0){
		power=(fabs(to_go)>ERROR_THRESHOLD)?1:0;
	}else{
		power=(fabs(to_go)>ERROR_THRESHOLD)?-1:0;
	}
	return make_pair(state,power);
}
