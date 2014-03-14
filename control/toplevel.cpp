#include "toplevel.h"
#include<iostream>
#include<cassert>
#include<math.h>
#include "../util/util.h"

using namespace std;

namespace Toplevel{
	Output::Output():
		collector(OFF),
		collector_tilt(Collector_tilt::OUTPUT_UP),
		injector(Injector::OUTPUT_DOWN),
		injector_arms(Injector_arms::OUTPUT_CLOSE),
		ejector(Ejector::OUTPUT_DOWN),
		pump(Pump::ON)
	{}

	ostream& operator<<(ostream& o,Output g){
		o<<"Output(";
		o<<"collect:"<<g.collector;
		o<<" colct_tlt:"<<g.collector_tilt;
		o<<" inject:"<<g.injector;
		o<<" inj arm:"<<g.injector_arms;
		o<<" eject:"<<g.ejector;
		o<<" shoot:"<<g.shooter_wheels;
		o<<" pump:"<<g.pump;
		o<<" drive:"<<g.drive;
		return o<<")";
	}

	Subgoals::Subgoals():
		collector(OFF),
		collector_tilt(Collector_tilt::GOAL_UP),
		injector(Injector::WAIT),
		injector_arms(Injector_arms::GOAL_X),
		ejector(Ejector::WAIT)
		//shooter_wheels(Shooter_wheels:)
	{}

	ostream& operator<<(ostream& o,Subgoals g){
		o<<"Toplevel::Subgoals(";
		o<<"collect:"<<g.collector;
		o<<" colct_tlt:"<<g.collector_tilt;
		o<<" inject:"<<g.injector;
		o<<" inj arm:"<<g.injector_arms;
		o<<" eject:"<<g.ejector;
		o<<" shoot:";
		o<<g.shooter_wheels.first;
		o<<g.shooter_wheels.second;
		//o<<g.shooter_wheels; not sure why this line doesn't work.
		o<<" drive:"<<g.drive;
		return o<<")";
	}

	Status::Status():
		collector_tilt(Collector_tilt::STATUS_LOWERING),
		injector(Injector::Estimator::GOING_DOWN),
		injector_arms(Injector_arms::STATUS_CLOSING),
		ejector(Ejector::Estimator::GOING_DOWN),
		pump(Pump::NOT_FULL)
	{}

	bool operator==(Status a,Status b){
		#define X(name) if(a.name!=b.name) return 0;
		X(collector_tilt)
		X(injector)
		X(injector_arms)
		X(ejector)
		X(shooter_wheels)
		X(pump)
		X(orientation)
		#undef X
		return 1;
	}

	bool operator!=(Status a,Status b){
		return !(a==b);
	}

	ostream& operator<<(ostream& o,Status s){
		o<<"Status(";
		o<<"colct_tlt:"<<s.collector_tilt;
		o<<" inject:"<<s.injector;
		o<<" inj arm:"<<s.injector_arms;
		o<<" eject:"<<s.ejector;
		o<<" shoot:"<<s.shooter_wheels;
		o<<" pump:"<<s.pump;
		o<<" orientation:"<<s.orientation;
		return o<<")";
	}

	Estimator::Estimator():pump(Pump::NOT_FULL), orientation(0){}

	void Estimator::update(Time time,bool enabled,Output out,Pump::Status pump_status, float orientation1,Shooter_wheels::Status wheels_in){
		collector_tilt.update(time,enabled?out.collector_tilt:Collector_tilt::OUTPUT_NEITHER);
		injector.update(time,enabled?out.injector:Injector::OUTPUT_VENT);
		injector_arms.update(time,enabled?out.injector_arms:Injector_arms::OUTPUT_OPEN);
		ejector.update(time,enabled?out.ejector:Ejector::OUTPUT_DOWN);
		shooter_wheels=wheels_in;
		pump=pump_status;
		orientation = orientation1;
	}

	Status Estimator::estimate()const{
		Status r;
		r.collector_tilt=collector_tilt.estimate();
		r.injector=injector.estimate();
		r.injector_arms=injector_arms.estimate();
		r.ejector=ejector.estimate();
		r.pump=pump;
		r.shooter_wheels = shooter_wheels;
		return r;
	}

	void Estimator::out(ostream& o)const{
		o<<"Estimator(";
		o<<"colct_tlt:"<<collector_tilt;
		o<<" inject:"<<injector;
		o<<" inj arm:"<<injector_arms;
		o<<" eject:"<<ejector;
		o<<" shooter_wheels:"<<shooter_wheels;
		o<<" pump:"<<pump;
		o<<")";
	}

	bool operator==(Estimator a,Estimator b){
		#define X(name) if(a.name!=b.name) return 0;
		X(collector_tilt)
		X(injector)
		X(injector_arms)
		X(ejector)
		X(shooter_wheels)
		X(pump)
		X(orientation)
		#undef X
		return 1;
	}

	bool operator!=(Estimator a,Estimator b){
		return !(a==b);
	}

	ostream& operator<<(ostream& o,Estimator e){
		e.out(o);
		return o;
	}

	bool approx_equal(Estimator a,Estimator b){
		return a.estimate()==b.estimate();
	}

	Output control(Status status,Subgoals g){
		Output r;
		r.collector=g.collector;
		r.collector_tilt=Collector_tilt::control(g.collector_tilt);
		r.injector=Injector::control(status.injector,g.injector);
		r.injector_arms=Injector_arms::control(status.injector_arms,g.injector_arms);
		r.ejector=Ejector::control(status.ejector,g.ejector);
		r.shooter_wheels=control(status.shooter_wheels,g.shooter_wheels);
		r.pump=Pump::control(status.pump);
		r.drive=::control(g.drive, status.orientation);
		return r;
	}

	bool ready(Status status,Subgoals g){
		return Collector_tilt::ready(status.collector_tilt,g.collector_tilt) && 
			Injector::ready(status.injector,g.injector) && 
			Injector_arms::ready(status.injector_arms,g.injector_arms) && 
			Ejector::ready(status.ejector,g.ejector) && 
			ready(status.shooter_wheels,g.shooter_wheels);
	}
	
	vector<string> not_ready(Status status,Subgoals g){
		vector<string> r;
		#define X(name) if(!ready(status.name,g.name)) r|=as_string(""#name);
		X(collector_tilt)
		X(injector)
		X(injector_arms)
		X(ejector)
		X(shooter_wheels)
		#undef X
		return r;
	}
	
	/*ostream& operator<<(ostream& o,Control a){
		o<<"Toplevel::Control(";
		o<<a.shooter_wheels;
		return o<<")";
	}*/

}

#ifdef TOPLEVEL_TEST
int main(){
	using namespace Toplevel;
	Toplevel::Subgoals g;
	cout<<g<<"\n";
	Toplevel::Status status;
	cout<<status<<"\n";
	//Toplevel::Control control;
	Estimator est;
	cout<<est<<"\n";
	cout<<est.estimate()<<"\n";
	Pump::Status ps=Pump::FULL;
	est.update(0,1,Output(),ps,0,Shooter_wheels::Status());
	est.update(10,0,Output(),ps,0,Shooter_wheels::Status());
	cout<<est.estimate()<<"\n";
	/*
	if we choose one of the modes and use all the built-in controls then we should after some time get to a status where we're ready.  
	*/
}
#endif
