#include "ejector.h"
#include<iostream>
#include<cassert>

using namespace std;

namespace Ejector{
	ostream& operator<<(ostream& o,Status s){
		switch(s){
			#define X(name) case name: return o<<""#name;
			X(IDLE)
			X(SHOOTING)
			X(RECOVERY)
			#undef X
			default: assert(0);
		}
	}

	ostream& operator<<(ostream& o,Output a){
		switch(a){
			#define X(name) case OUTPUT_##name: return o<<""#name;
			X(UP)
			X(DOWN)
			#undef X
			default: assert(0);
		}
	}

	ostream& operator<<(ostream& o,Goal g){
		switch(g){
			case START: return o<<"START";
			case WAIT: return o<<"WAIT";
			case X: return o<<"X";
			default: assert(0);
		}
	}

	Estimator::Estimator():location(GOING_DOWN){} //might want to change this to assume that it starts down.

	void Estimator::update(Time time,Output out){
		switch(location){
			case GOING_UP:
				if(out==OUTPUT_DOWN){
					location=GOING_DOWN;
					timer.update(time,1);
				}else{
					timer.update(time,0);
					static const Time RISE_TIME=2.5;
					if(timer.elapsed()>RISE_TIME){
						location=UP;
					}
				}
				break;
			case UP:
				if(out==OUTPUT_DOWN){
					location=GOING_DOWN;
					timer.update(time,1);
				}
				break;
			case GOING_DOWN:
				if(out==OUTPUT_DOWN){
					timer.update(time,0);
					static const Time LOWER_TIME=1.6;
					if(timer.elapsed()>LOWER_TIME){
						location=DOWN;
					}
				}else{
					location=GOING_UP;
					timer.update(time,1);
				}
				break;
			case DOWN:
				if(out==OUTPUT_UP){
					location=GOING_UP;
					timer.update(time,1);
				}
				break;
			default:
				assert(0);
		}
	}

	Status location_to_status(Estimator::Location location){
		switch(location){
			case Estimator::GOING_UP:
			case Estimator::UP:
				return SHOOTING;
			case Estimator::GOING_DOWN:
				return RECOVERY;
			case Estimator::DOWN:
				return IDLE;
			default: assert(0);
		}
	}

	Estimator::Location Estimator::estimate()const{ return location; }

	//I'm not sure that this function should be part of the class.
	Status Estimator::status()const{
		return location_to_status(location);
	}

	void Estimator::out(ostream& o)const{
		o<<"Estimator("<<location<<" "<<timer<<")";
	}

	ostream& operator<<(ostream& o,Estimator::Location a){
		switch(a){
			#define X(name) case Estimator::name: return o<<""#name;
			X(GOING_UP)
			X(UP)
			X(GOING_DOWN)
			X(DOWN)
			#undef X
			default: assert(0);
		}
	}

	bool operator!=(Estimator,Estimator){
		assert(0);
	}

	ostream& operator<<(ostream& o,Estimator est){
		est.out(o);
		return o;
	}

	Output control(Estimator::Location location,Goal goal){
		switch(location){
			case Estimator::DOWN: return (goal==START)?OUTPUT_UP:OUTPUT_DOWN;
			case Estimator::GOING_UP: return OUTPUT_UP;
			case Estimator::UP:
			case Estimator::GOING_DOWN:
				return OUTPUT_DOWN;
			default: assert(0);
		}
	}

	bool ready(Estimator::Location location,Goal goal){
		return ready(location_to_status(location),goal);
	}

	bool ready(Status status,Goal goal){
		return goal==X || status==IDLE;
	}
}

#ifdef EJECTOR_TEST
#include<vector>

int main(){
	using namespace Ejector;
	Estimator e;
	cout<<e<<"\n";
	assert(e.status()==RECOVERY);
	e.update(0,OUTPUT_UP);
	assert(e.status()==SHOOTING);
	static const vector<Estimator::Location> LOCATIONS{Estimator::GOING_UP,Estimator::UP,Estimator::GOING_DOWN,Estimator::DOWN};
	static const vector<Status> STATUS_LIST{IDLE,SHOOTING,RECOVERY};
	static const vector<Goal> GOALS{START,WAIT,X};
	cout<<"Control outputs:\n";
	for(auto location:LOCATIONS){
		for(auto goal:GOALS){
			cout<<location<<"\t"<<goal<<"\t"<<control(location,goal)<<"\n";
		}
	}
	cout<<"Ready states:\n";
	for(auto status:STATUS_LIST){
		for(auto goal:GOALS){
			cout<<status<<"\t"<<goal<<"\t"<<ready(status,goal)<<"\n";
		}
	}
}
#endif
