#include "panel2014.h"

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
	learn(0),
	injector(Injector::X),
	injector_arms(Injector_arms::GOAL_X),
	ejector(Ejector::X)
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

#ifdef PANEL2014_TEST
int main(){
	Panel p;
	cout<<p<<"\n";
}
#endif
