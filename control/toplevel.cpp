#include "toplevel.h"
#include<iostream>
#include<cassert>

using namespace std;

namespace Toplevel{
	Output::Output():
		collector(OFF),
		collector_tilt(Collector_tilt::OUTPUT_UP),
		injector(Injector::OUTPUT_DOWN),
		injector_arms(Injector_arms::OUTPUT_CLOSE),
		ejector(Ejector::OUTPUT_DOWN)
	{}

	template<typename T>
	void out(ostream& o,T g){
		o<<"collect:"<<g.collector;
		o<<" colct_tlt:"<<g.collector_tilt;
		o<<" inject:"<<g.injector;
		o<<" inj arm:"<<g.injector_arms;
		o<<" eject:"<<g.ejector;
		o<<" shoot:"<<g.shooter_wheels;
	}

	ostream& operator<<(ostream& o,Output a){
		o<<"Output(";
		out(o,a);
		return o<<")";
	}

	Subgoals::Subgoals():
		collector(OFF),
		collector_tilt(Collector_tilt::GOAL_UP),
		injector(Injector::WAIT),
		injector_arms(Injector_arms::GOAL_X),
		ejector(Ejector::WAIT),
		shooter_wheels(Shooter_wheels::X)
	{}

	ostream& operator<<(ostream& o,Subgoals g){
		o<<"Toplevel::Subgoals(";
		out(o,g);
		return o<<")";
	}

	Status::Status():
		collector_tilt(Collector_tilt::STATUS_LOWERING),
		injector(Injector::RECOVERY),
		injector_arms(Injector_arms::STATUS_CLOSING),
		ejector(Ejector::RECOVERY)
	{}

	ostream& operator<<(ostream& o,Status s){
		o<<"Status(";
		o<<"colct_tlt:"<<s.collector_tilt;
		o<<" inject:"<<s.injector;
		o<<" inj arm:"<<s.injector_arms;
		o<<" eject:"<<s.ejector;
		o<<" shoot:"<<s.shooter_wheels;
		return o<<")";
	}

	void Estimator::update(Time time,Output out){
		collector_tilt.update(time,out.collector_tilt);
		injector.update(time,out.injector);
		injector_arms.update(time,out.injector_arms);
		ejector.update(time,out.ejector);
	}

	Status Estimator::estimate()const{
		Status r;
		r.collector_tilt=collector_tilt.estimate();
		r.injector=injector.status();
		r.injector_arms=injector_arms.estimate();
		r.ejector=ejector.status();
		return r;
	}

	void Estimator::out(ostream& o)const{
		o<<"Estimator(";
		o<<"colct_tlt:"<<collector_tilt;
		o<<" inject:"<<injector;
		o<<" inj arm:"<<injector_arms;
		o<<" eject:"<<ejector;
		//o<<" shoot:"<<e.shoot;
		o<<")";
	}

	ostream& operator<<(ostream& o,Estimator e){
		e.out(o);
		return o;
	}

	Output control(Status status,Subgoals g){
		Output r;
		r.collector=g.collector;
		r.collector_tilt=control(g.collector_tilt);
		r.injector=control(status.injector,g.injector);
		r.injector_arms=control(status.injector_arms,g.injector_arms);
		r.ejector=control(status.ejector,g.ejector);
		r.shooter_wheels=control(g.shooter_wheels);
		return r;
	}

	bool ready(Status status,Subgoals g){
		return ready(status.collector_tilt,g.collector_tilt) && ready(status.injector,g.injector) && ready(status.injector_arms,g.injector_arms) && ready(status.ejector,g.ejector) && ready(status.shooter_wheels,g.shooter_wheels);
	}

	ostream& operator<<(ostream& o,Mode m){
		#define X(name) if(m==name) return o<<""#name;
		X(DRIVE_WO_BALL)
		X(DRIVE_W_BALL)
		X(COLLECT)
		X(SHOOT_HIGH_PREP)
		X(SHOOT_HIGH)
		X(TRUSS_TOSS_PREP)
		X(TRUSS_TOSS)
		X(PASS_PREP)
		X(PASS)
		X(EJECT_PREP)
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
				if(m==EJECT) r.ejector=Ejector::START;
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
	static const vector<Mode> MODES{
		DRIVE_WO_BALL,DRIVE_W_BALL,
		COLLECT,
		SHOOT_HIGH_PREP,SHOOT_HIGH,
		TRUSS_TOSS_PREP,TRUSS_TOSS,
		PASS_PREP,PASS,
		EJECT_PREP,EJECT,
		CATCH
	};
	Toplevel::Status status;
	cout<<status<<"\n";
	for(auto mode:MODES){
		cout<<mode<<":\n";
		auto g=subgoals(mode);
		cout<<"\t"<<g<<"\n";
		cout<<"\t"<<control(status,g)<<"\n";
		cout<<"\t"<<ready(status,g)<<"\n";
	}
	Estimator est;
	cout<<est<<"\n";
	cout<<est.estimate()<<"\n";
	est.update(0,Output());
	est.update(10,Output());
	cout<<est.estimate()<<"\n";
	/*
	if we choose one of the modes and use all the built-in controls then we should after some time get to a status where we're ready.  
	*/
}
#endif
