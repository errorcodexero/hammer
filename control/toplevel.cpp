#include "toplevel.h"
#include<iostream>
#include<cassert>
#include<math.h>

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
		o<<" shoot:"<<g.shooter_wheels;
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

	void Estimator::update(Time time,Output out,Pump::Status pump_status, float orientation1,Shooter_wheels::Status wheels_in){
		collector_tilt.update(time,out.collector_tilt);
		injector.update(time,out.injector);
		injector_arms.update(time,out.injector_arms);
		ejector.update(time,out.ejector);
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
	
	/*ostream& operator<<(ostream& o,Control a){
		o<<"Toplevel::Control(";
		o<<a.shooter_wheels;
		return o<<")";
	}*/

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
		//X(SHOOT_LOW)
		#undef X
		assert(0);
	}
	
	Subgoals subgoals(Mode m,Drive_goal d,wheelcalib calib){
		Subgoals r;
		r.drive=d;
		switch(m){
			case DRIVE_WO_BALL:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_X;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::X);
				break;
			case DRIVE_W_BALL:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::X);
				break;
			case COLLECT:
				r.collector=ON;
				r.collector_tilt=Collector_tilt::GOAL_DOWN;
				r.injector_arms=Injector_arms::GOAL_OPEN;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::X);
				break;
			case SHOOT_HIGH_PREP:
			case SHOOT_HIGH:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::HIGH_GOAL);
				if(m==SHOOT_HIGH){
					r.injector=Injector::START;
				}
				break;
			case TRUSS_TOSS_PREP:
			case TRUSS_TOSS:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::TRUSS);
				if(m==TRUSS_TOSS) r.injector=Injector::START;
				break;
			case PASS_PREP:
			case PASS:
				r.collector_tilt=Collector_tilt::GOAL_UP;
				r.injector_arms=Injector_arms::GOAL_CLOSE;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::PASS);
				if(m==PASS) r.injector=Injector::START;
				break;
			case EJECT_PREP:
			case EJECT:
                r.collector_tilt=Collector_tilt::GOAL_UP;//was down, but with the current ejector geometry works better this way.
                r.injector_arms=Injector_arms::GOAL_OPEN;
                if(m==EJECT) r.ejector=Ejector::START;
                //Copied from a previous commit of the code, basically what it was before modification
				break;
				/*
				r.collector=REVERSE;
				r.collector_tilt=Collector_tilt::GOAL_DOWN;
				r.injector_arms=Injector_arms::GOAL_OPEN;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::X);
				if(m==EJECT) r.ejector=Ejector::START;
				break;
				*/
			case CATCH:
				r.collector_tilt=Collector_tilt::GOAL_DOWN;
				r.injector_arms=Injector_arms::GOAL_CLOSE;//not sure that this matters
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::STOP);//could also have a reverse mode here
				break;
			/*
			case SHOOT_LOW:
				r.collector_tilt=Collector_tilt::GOAL_UP;//was down, but with the current ejector geometry works better this way.
				r.injector_arms=Injector_arms::GOAL_OPEN;
				r.shooter_wheels=convert_goal(calib,Shooter_wheels::X);//could also have a reverse mode here
				if(m==SHOOT_LOW) r.ejector=Ejector::START;
				break;
			*/
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
		CATCH, //SHOOT_LOW
	};
	Toplevel::Status status;
	cout<<status<<"\n";
	//Toplevel::Control control;
	for(auto mode:MODES){
		cout<<mode<<":\n";
		auto g=subgoals(mode,Drive_goal(),wheelcalib());
		cout<<"\t"<<g<<"\n";
		cout<<"\t"<<control(status,g)<<"\n";
		cout<<"\t"<<ready(status,g)<<"\n";
	}
	Estimator est;
	cout<<est<<"\n";
	cout<<est.estimate()<<"\n";
	Pump::Status ps=Pump::FULL;
	est.update(0,Output(),ps,0,Shooter_wheels::Status());
	est.update(10,Output(),ps,0,Shooter_wheels::Status());
	cout<<est.estimate()<<"\n";
	/*
	if we choose one of the modes and use all the built-in controls then we should after some time get to a status where we're ready.  
	*/
}
#endif
