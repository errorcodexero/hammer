#include "gyro_tracker.h"
#include<iostream>

using namespace std;

Integrator::Integrator():last(-1),total(0){}

void Integrator::update(Time t,double value){
	if(last!=-1){
		double elapsed=t-last;
		total+=elapsed*value;
	}
	last=t;
}

ostream& operator<<(ostream& o,Integrator a){
	return o<<"Integrator("<<a.last<<" "<<a.total<<")";
}

Gyro_tracker::Gyro_tracker():cal_accumulated(0),cal_samples(0),cal_start(-1),center(0){
}

void Gyro_tracker::update(Time now,Volt v){
	bool cal_done=cal_start!=-1 && now-cal_start>1;
	if(cal_done){
		//this value is totally bogus and needs to be calibrated or copied carefully from Gyro.cpp.
		double x=(v-center)*18000;
		
		integrator.update(now,x);
	}else{
		if(cal_start==-1) cal_start=now;
		cal_accumulated+=v;
		cal_samples++;
		center=cal_accumulated/cal_samples;
	}
}

//it might make more sense to do the unit convertion in here instead of in update.
Degree Gyro_tracker::angle()const{ return integrator.total; }

ostream& operator<<(ostream& o,Gyro_tracker a){
	return o<<"Gyro_tracker(cal_a:"<<a.cal_accumulated<<" cal_s:"<<a.cal_samples<<" cal_t:"<<a.cal_start<<" ctr:"<<a.center<<" int:"<<a.integrator<<")";
}

#ifdef GYRO_TRACKER_TEST
int main(){
	Integrator a;
	a.update(0,100);
	for(unsigned i=1;i<=10;i++){
		a.update(i,1);
	}
	cout<<a<<"\n";
	
	Gyro_tracker g;
	double expected_center=1.5;
	unsigned i;
	for(i=0;i<20;i++){
		g.update(i,expected_center);
	}
	cout<<g<<"\n";
	g.update(i,0);
	cout<<g<<"\n";
}
#endif
