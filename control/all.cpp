#include "all.h"
#include "common.h"

using namespace std;

template<typename A,typename B>
ostream& operator<<(ostream& o,pair<A,B> const& p){
	return o<<"("<<p.first<<","<<p.second<<")";
}

ostream& operator<<(ostream& o,Drivebase_state const& a){
	o<<"Drivebase_state(";
	for(int i=0;i<MODULES;i++){
		o<<a.mod[i];
	}
	return o<<")";
}

ostream& operator<<(ostream& o,Drivebase_output const& a){
	o<<"Drivebase_output(";
	for(int i=0;i<MODULES;i++){
		o<<a.drive_module[i]<<endl;
	}
	for(int i=0;i<MODULES;i++){
		o<<a.turn_power[1]<<endl;
	}
	return o<<")";
}

pair<Drivebase_state,Drivebase_output> next(Drivebase_state state,Drivebase_input in){
	Drivebase_target h=high_level(in.joysticks);
	Drivebase_output out;
	for(unsigned i=0;i<MODULES;i++){
		pair<Module_state,Portion> p=rotation_power(state.mod[i],h.mod[i].angle,in.encoders[i]);
		state.mod[i]=p.first;
		out.turn_power[i]=p.second;
		out.drive_module[i]=h.mod[i].power;
	}
	return make_pair(state,out);
}

/*int main(){
	Drivebase_state a;
	cout<<next(a,Drivebase_input{});
}*/
