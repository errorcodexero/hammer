#include "collector_tilt.h"
#include<iostream>
#include<cassert>

//this is very similar to the injector arms.

using namespace std;

namespace Collector_tilt{
	ostream& operator<<(ostream& o,Goal g){
		switch(g){
			#define X(name) case GOAL_##name: return o<<""#name;
			X(UP)
			X(DOWN)
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

	ostream& operator<<(ostream& o,Status s){
		switch(s){
			#define X(name) case STATUS_##name: return o<<""#name;
			X(UP)
			X(DOWN)
			X(RAISING)
			X(LOWERING)
			#undef X
			default: assert(0);
		}
	}

	Estimator::Estimator():est(STATUS_UP){}

	void Estimator::update(Time time,Output output){
		switch(est){
			case STATUS_UP:
				if(output!=OUTPUT_UP){
					est=STATUS_LOWERING;
					timer.update(time,1);
				}
				break;
			case STATUS_DOWN:
				if(output!=OUTPUT_DOWN){
					est=STATUS_RAISING;
					timer.update(time,1);
				}
				break;
			case STATUS_RAISING:
				if(output==OUTPUT_UP){
					timer.update(time,0);
					static const Time RISE_TIME=2.5;//total guess
					if(timer.elapsed()>RISE_TIME){
						est=STATUS_UP;
					}
				}else{
					timer.update(time,0);
					est=STATUS_LOWERING;
				}
				break;
			case STATUS_LOWERING:
				if(output==OUTPUT_DOWN){
					timer.update(time,0);
					static const Time LOWER_TIME=1.5;//total guess
					if(timer.elapsed()>LOWER_TIME){
						est=STATUS_DOWN;
					}
				}else{
					timer.update(time,0);
					est=STATUS_RAISING;
				}
				break;
			default: assert(0);
		}
	}

	Status Estimator::estimate()const{ return est; }

	void Estimator::out(ostream& o)const{
		o<<"Estimator("<<est<<","<<timer<<")";
	}

	ostream& operator<<(ostream& o,Estimator e){
		e.out(o);
		return o;
	}

	Output control(Goal g){
		switch(g){
			case GOAL_UP: return OUTPUT_UP;
			case GOAL_DOWN: return OUTPUT_DOWN;
			default: assert(0);
		}
	}
}

#ifdef COLLECTOR_TILT_TEST
#include<vector>
int main(){
	using namespace Collector_tilt;
	Estimator e;
	assert(e.estimate()==STATUS_UP);
	cout<<e<<"\n";
	e.update(.4,OUTPUT_DOWN);
	assert(e.estimate()==STATUS_LOWERING);
	e.update(10,OUTPUT_DOWN);
	assert(e.estimate()==STATUS_DOWN);
	//if wanted to do this well would try all the other transitions as well.

	static const vector<Goal> GOALS{GOAL_UP,GOAL_DOWN};
	for(auto goal:GOALS){
		cout<<goal<<":"<<control(goal)<<"\n";
	}
}
#endif
