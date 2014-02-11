#include "injector.h"
#include<iostream>
#include<cassert>

using namespace std;

namespace Injector{
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

	//I'm not sure that this function should be part of the class.
	Status Estimator::status()const{
		switch(location){
			case GOING_UP:
			case UP:
				return SHOOTING;
			case GOING_DOWN:
				return RECOVERY;
			case DOWN:
				return IDLE;
			default: assert(0);
		}
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

	ostream& operator<<(ostream& o,Estimator est){
		est.out(o);
		return o;
	}

	Output control(Estimator::Location loc,Goal g){
		switch(loc){
			case Estimator::Location::GOING_UP:
				return OUTPUT_UP;
			case Estimator::Location::UP:
			case Estimator::Location::GOING_DOWN:
				return OUTPUT_DOWN;
			case Estimator::Location::DOWN:
				return (g==START)?OUTPUT_UP:OUTPUT_DOWN;
			default: assert(0);
		}
	}
}

#ifdef INJECTOR_TEST
#include<vector>

int main(){
	using namespace Injector;
	Estimator e;
	cout<<e<<"\n";
	assert(e.status()==RECOVERY);
	e.update(0,OUTPUT_UP);
	assert(e.status()==SHOOTING);
	static const vector<Estimator::Location> LOCATIONS{Estimator::GOING_UP,Estimator::UP,Estimator::GOING_DOWN,Estimator::DOWN};
	static const vector<Goal> GOALS{START,WAIT};
	for(auto loc:LOCATIONS){
		for(auto goal:GOALS){
			cout<<loc<<"\t"<<goal<<"\t"<<control(loc,goal)<<"\n";
		}
	}
}
#endif
