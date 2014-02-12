#include "toplevel.h"
#include<iostream>
#include<cassert>

using namespace std;

namespace Toplevel{
	Subgoals::Subgoals():
		collector(OFF),
		collector_tilt(Collector_tilt::GOAL_UP),
		injector(Injector::WAIT),
		injector_arms(Injector_arms::GOAL_X),
		ejector(Injector::WAIT),
		shooter_wheels(Shooter_wheels::X)
	{}

	ostream& operator<<(ostream& o,Subgoals g){
		o<<"Toplevel::Subgoals(";
		o<<"collect:"<<g.collector;
		o<<" colct_tlt:"<<g.collector_tilt;
		o<<" inject:"<<g.injector;
		o<<" inj arm:"<<g.injector_arms;
		o<<" eject:"<<g.ejector;
		o<<" shoot:"<<g.shooter_wheels;
		return o<<")";
	}

	Status::Status():
		collector_tilt(Collector_tilt::STATUS_LOWERING),
		injector(Injector::RECOVERY),
		injector_arms(Injector_arms::STATUS_CLOSING),
		ejector(Injector::RECOVERY)
	{}

	ostream& operator<<(ostream& o,Status s){
		o<<"Status(";
		o<<s.collector_tilt;
		o<<s.injector;
		o<<s.injector_arms;
		o<<s.shooter_wheels;
		return o<<")";
	}

	ostream& operator<<(ostream& o,Mode m){
		#define X(name) if(m==name) return o<<""#name;
		X(DRIVE_WO_BALL)
		X(DRIVE_W_BALL)
		X(COLLECT)
		X(SHOOT_HIGH)
		X(TRUSS_TOSS)
		X(PASS)
		X(EJECT)
		X(CATCH)
		#undef X
		assert(0);
	}

	Subgoals subgoals(Mode m){
		Subgoals r;
		switch(m){
			case DRIVE_WO_BALL:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_X;
				r.shooter_wheels=Shooter_wheels::X;
				break;
			case DRIVE_W_BALL:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=Shooter_wheels::X;
				break;
			case COLLECT:
				r.collector_tilt=Collector_tilt::GOAL_DOWN;
				r.injector_arms=Injector_arms::GOAL_OPEN;
				r.shooter_wheels=Shooter_wheels::X;
				break;
			case SHOOT_HIGH_PREP:
			case SHOOT_HIGH:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=Shooter_wheels::HIGH_GOAL;
				if(m==SHOOT_HIGH){
					r.injector=Injector::START;
				}
				break;
			case TRUSS_TOSS_PREP:
			case TRUSS_TOSS:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=Shooter_wheels::TRUSS;
				if(m==TRUSS_TOSS) r.injector=Injector::START;
				break;
			case PASS_PREP:
			case PASS:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=Shooter_wheels::PASS;
				if(m==PASS) r.injector=Injector::START;
				break;
			case EJECT_PREP:
			case EJECT:
				r.collector_tilt=Collector_tilt::GOAL_DOWN;
				r.injector_arms=Injector_arms::GOAL_OPEN;
				if(m==EJECT) r.ejector=Injector::START;
				break;
			case CATCH:
				r.collector_tilt=Collector_tilt::GOAL_DOWN;
				r.injector_arms=Injector_arms::GOAL_CLOSE;//not sure that this matters
				r.shooter_wheels=Shooter_wheels::STOP;//could also have a reverse mode here
				break;
			default:assert(0);
		}
		return r;
	}
}

#ifdef TOPLEVEL_TEST
#include<vector>

int main(){
	Toplevel::Subgoals g;
	cout<<g<<"\n";
	using namespace Toplevel;
	static const vector<Mode> MODES{DRIVE_WO_BALL,DRIVE_W_BALL,COLLECT,SHOOT_HIGH,PASS,EJECT,CATCH};
	for(auto mode:MODES){
		cout<<mode<<":";
		cout<<subgoals(mode)<<"\n";
	}
}
#endif
