#include "shooter_wheels.h"
#include<iostream>
#include<cassert>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include"wheelrpms.h"

using namespace std;

namespace Shooter_wheels{
	ostream& operator<<(ostream& o,High_level_goal g){
		switch(g){
			#define X1(name) case name: return o<<""#name;
			X1(HIGH_GOAL)
			X1(TRUSS)
			X1(PASS)
			X1(STOP)
			X1(X)
			#undef X1
			default: assert(0);
		}
	}

	ostream& operator<<(ostream& o,Output a){
		o<<"Shooter_wheel::Output(";
		for(unsigned i=0;i<2;i++){
			o<<"t"<<i<<":"<<a.top[i];
		}
		for(unsigned i=0;i<2;i++){
			o<<"b"<<i<<":"<<a.bottom[i];
		}
		return o<<")";
	}
	
	Calibration_manager::Calibration_manager(){
		calib = readconfig();
		//this could be done in a better way!
		if(calib.highgoal.top == 0){
			calib = rpmsdefault();
		}
	}

	RPM& find_rpm(Shooter_wheels::Status &status,bool top){
		return top?status.top:status.bottom;
	}
	
	RPM& find_rpm(wheelcalib &in,Calibration_target t){
		switch(t.target){
			case Fire_control::HIGH: return find_rpm(in.highgoal,t.top);
			case Fire_control::TRUSS: return find_rpm(in.overtruss,t.top);
			case Fire_control::PASS: return find_rpm(in.passing,t.top);
			case Fire_control::NO_TARGET:
			case Fire_control::EJECT:
				return in.lowgoal.bottom;
			default: assert(0);
		}
	}
	
	wheelcalib Calibration_manager::update(bool learn_button,double adjust_wheel,Calibration_target t){
		wheelcalib r=calib;
		double adjustment=((adjust_wheel/3.3)-.5)*2*100;
		//cout<<"adjustment:"<<adjustment<<"\n";
		find_rpm(r,t)+=adjustment;
		if(learn(learn_button)){
			//write changes to the file
			calib=r;
			writeconfig(r);
		}
		return r;
	}
	
	RPM target_speed_top(High_level_goal g,wheelcalib c){
		//this stuff eventually should come out of a config file.
		switch(g){
			case TRUSS:
				return c.overtruss.top; //Previously 1200
			case HIGH_GOAL:
				return c.highgoal.top; //Previously 1200
			case PASS:
				return c.highgoal.top; //Previously 2200
			case STOP:
			case X:
				return 0;
			default: assert(0);
		}
	}

	RPM target_speed_bottom(High_level_goal g, wheelcalib c){
		switch(g){
			case TRUSS:
				return c.overtruss.bottom; //Previously 1200
			case HIGH_GOAL:
				return c.highgoal.bottom; //Previously 3000
			case PASS:
				return c.lowgoal.bottom; //Previously 2200
			case STOP:
			case X:
				return 0;
			default: assert(0);
		}
	}
	
	RPM free_speed(){
		return 5984;  //Calculated top speed on 100% output
	}

	Jaguar_output open_loop(RPM status,RPM goal){
		if(goal>free_speed()) goal=free_speed();
		if(status>.8*goal&&goal>1000) return Jaguar_output::voltageOut((goal/62.5+4.199)/100);
		return Jaguar_output::voltageOut(goal>1000);
	} 
	
	Output control(Status status,Goal goal){
		Output r;
		//Jaguar_output top=Jaguar_output::voltageOut(.25),bottom=Jaguar_output::voltageOut(.6);
		bool all_open_loop=0;
		r.top[Output::FEEDBACK]=all_open_loop?open_loop(status.top,goal.second.top):Jaguar_output::speedOut(goal.second.top);
		r.top[Output::OPEN_LOOP]=open_loop(status.top,goal.second.top);
		r.bottom[Output::FEEDBACK]=all_open_loop?open_loop(status.bottom,goal.second.bottom):Jaguar_output::speedOut(goal.second.bottom);
		r.bottom[Output::OPEN_LOOP]=open_loop(status.bottom,goal.second.bottom);
		return r;
	}

	ostream& operator<<(ostream& o,Calibration_manager c){
		return o<<"Shooter_wheels::Calibration_manager("<<c.calib<<")";
	}
	
	bool ready(Status status,Goal goal){
		if(goal.first==Shooter_wheels::STOP || goal.first==Shooter_wheels::X) return 1;
		//this could be refined.
		return fabs(status.top-goal.second.top)<100 && fabs(status.bottom-goal.second.bottom)<100;
	}
	
	Goal convert_goal(wheelcalib c,High_level_goal g){
		switch(g){
			case TRUSS:
				return make_pair(g,c.overtruss); //Previously 1200
			case HIGH_GOAL:
				return make_pair(g,c.highgoal); //Previously 3000
			case PASS:
				return make_pair(g,c.lowgoal); //Previously 2200
			case STOP:
			case X:
				return make_pair(g,Shooter_wheels::Status());
			default: assert(0);
		}
	}
}

#ifdef SHOOTER_WHEELS_TEST
#include<vector>

int main(){
	using namespace Shooter_wheels;

	static const vector<High_level_goal> GOALS{HIGH_GOAL,TRUSS,PASS,STOP,X};
	for(auto goal:GOALS){
		//Control control;
		//assert(control.ready(goal,target_speed_top(goal,rpmsdefault()),target_speed_bottom(goal,rpmsdefault())));
		cout<<goal<<"\n";
	}
	{
		auto r=unlink("wheelrpms.txt");
		assert(r==0);
	}
	Calibration_manager c;
	cout<<c<<"\n";
	for(auto a:Calibration_target::all()){
		cout<<"-----------------------\n";
		for(double d:vector<double>{0,1.5,3.3}){
			cout<<a<<"\n";
			c.update(0,d,a);
			cout<<c.update(1,d,a)-rpmsdefault()<<"\n";
		}
	}
	
	wheelcalib a;
	wheelcalib b;
	cout<<(a-b)<<"\n";
}
#endif
