#include "main.h"
#include<iostream>
#include<sstream>
#include "octocanum.h"
#include "../util/util.h"
#include <math.h>

using namespace std;

double max3(double a,double b,double c){
	return max(max(a,b),c);
}

struct Drive_motors {
	double a,b,c;
//Outputs to motors; 
//a = frontLeft, b = frontRight, c = back;
//+value = clockwise
};

Drive_motors func_inner(double x, double y, double theta){	
	Drive_motors r;
	r.a=-double(1)/3* theta- double(1)/3* x -(double(1)/sqrt(3))*y;
	r.b=-double(1)/3* theta- double(1)/3* x +(double(1)/sqrt(3))*y;
	r.c=(-(double(1)/3)* theta) + ((double(2)/3)* x);
	return r;
}

Drive_motors func(double x,double y,double theta){
	//This function exists in order to pull the full power out of the drivetrain.  It makes some of the areas of the x/y/theta space have funny edges/non-smooth areas, but I think this is an acceptable tradeoff.
	Drive_motors r=func_inner(x,y,theta);
	const double s=sqrt(3);
	r.a*=s;
	r.b*=s;
	r.c*=s;
	const double m=max3(fabs(r.a),fabs(r.b),fabs(r.c));
	if(m>1){
		r.a/=m;
		r.b/=m;
		r.c/=m;
	}
	return r;
}

ostream& operator<<(ostream& o, Drive_motors d){
	return o<<"dm("<<d.a<<","<<d.b<<","<<d.c<<")";
}

unsigned interpret_10_turn_pot(Volt v){
	/*measured values, measured on the Fall 2013 mecanum base:
	0.005
	0.56
	1.116
	1.66
	2.22
	2.77
	3.33
	3.89
	4.44
	5.01
	limits are halfway between each value.
	*/
	Volt limits[]={
		0.2825,
		0.838,
		1.388,
		1.94,
		2.495,
		3.05,
		3.61,
		4.165,
		4.725,
		5.01
	};
	for(unsigned i=0;i<10;i++){
		if(v<limits[i]) return i;
	}
	return 9;
}

Bunnybot_goal::Bunnybot_goal():launch_bunny_now(0),poop_bunny_now(0){}

ostream& operator<<(ostream& o,Bunnybot_goal a){
	return o<<"Bunnybot_goal";
}

Bunnybot_output::Bunnybot_output():launch_bunny(0),poop_bunny(0){}

Bunnybot::Bunnybot():mode(TELEOP){}

Bunnybot_goal Bunnybot::autonomous(Robot_inputs in){
	//The lack of "break" is on purpose.
	switch(mode){
		case TELEOP:
			auto_timer.set(2);
			mode=AUTO_DRIVE1;
		case AUTO_DRIVE1:
			if(!auto_timer.done()){
				Bunnybot_goal r;
				r.drive=Traction_mode_goal(.1,0);
				return r;
			}
			mode=AUTO_SHOOT;
		case AUTO_SHOOT:
			if(bunny_launcher.state()!=Bunny_launcher::LAUNCHED){
				Bunnybot_goal r;
				r.launch_bunny_now=1;
				r.poop_bunny_now=1;
				return r;
			}
			auto_timer.set(2);
			mode=AUTO_DRIVE2;
		case AUTO_DRIVE2:
			if(!auto_timer.done()){
				Bunnybot_goal r;
				r.drive=Traction_mode_goal(.1,0);
				return r;
			}
			mode=AUTO_DONE;
		default: //should never get here.  Perhaps should add an assert or something.
		case AUTO_DONE:
			return Bunnybot_goal();
	}
}

Bunnybot_goal Bunnybot::teleop(Robot_inputs in){
	mode=TELEOP;
		
	Joystick_data main_joystick=in.joystick[0];
	bool traction_mode_button=main_joystick.button[3];//chosen arbitrarily.
	double x=main_joystick.axis[0];
	double y=-main_joystick.axis[1];
	double theta=-main_joystick.axis[2];
	/*for(unsigned i=0;i<Joystick_data::AXES;i++){
		bound[i].update(main_joystick.axis[i]);
	}*/
	
	goal_traction_mode.update(traction_mode_button);

	Bunnybot_goal r;
	if (main_joystick.button [7]){
		r.poop_bunny_now = 1;
	}
	
	if(goal_traction_mode.get()){
		r.drive=Traction_mode_goal(y,theta);
	}else{
		r.drive=Mecanum_goal(x,y,theta);
	}
	return r;
}

Bunnybot_goal Bunnybot::goal(Robot_inputs in){
	if(in.robot_mode.autonomous){
		return autonomous(in);
	}else{
		return teleop(in);
	}
}

Bunnybot_output Bunnybot::operator()(Robot_inputs in){
	auto_timer.update(in.now,in.robot_mode.enabled);
	Bunnybot_goal todo=goal(in);
	bunny_launcher.update(in.now,!in.robot_mode.enabled,todo.launch_bunny_now,0);
	bunny_pooper.update(in.now,!in.robot_mode.enabled,todo.poop_bunny_now,0);
	//pair<Octocanum_state,Octocanum_output> p=run(octocanum,todo.drive,in.now);
	//octocanum=p.first;
	Bunnybot_output r;
	//r.drive=p.second;
	r.launch_bunny=bunny_launcher.output();
	r.poop_bunny=bunny_pooper.output();
	return r;
}

ostream& operator<<(ostream& o,Bunnybot::Mode a){
	switch(a){
		#define X(name) case Bunnybot::name: return o<<""#name;
		X(TELEOP)
		X(AUTO_DRIVE1)
		X(AUTO_SHOOT)
		X(AUTO_DRIVE2)
		X(AUTO_DONE)
		#undef X
		default: return o<<"error";
	}
}

ostream& operator<<(ostream& o,Bunnybot a){
	o<<"Bunnybot(";
	o<<a.mode;
	o<<a.auto_timer;
	o<<"traction_mode="<<a.goal_traction_mode;
	o<<a.octocanum;
	o<<a.bunny_launcher;
	o<<a.bunny_pooper;
	return o<<")";
}

Robot_outputs Main::operator()(Robot_inputs in){
	gyro.update(in.now,in.analog[0]);
	perf.update(in.now);

	Joystick_data main_joystick=in.joystick[0];
	force.update(
		main_joystick.button[0],
		main_joystick.button[4],
		main_joystick.button[5],
		main_joystick.button[6],
		main_joystick.button[1],
		main_joystick.button[2]
	);
	Bunnybot_output b=bunnybot(in);

	Robot_outputs r;
	/*r.pwm[0]=pwm_convert(b.drive.wheels.lf);
	r.pwm[1]=pwm_convert(b.drive.wheels.lr);
	r.pwm[2]=pwm_convert(-b.drive.wheels.rf);
	r.pwm[3]=pwm_convert(-b.drive.wheels.rr);*/
	ball_collecter.update(main_joystick.button[5]);
	r.solenoid[1]=b.drive.traction_mode;
	r.solenoid[0]=b.launch_bunny;
	r.solenoid[2]=b.poop_bunny;
	double throttle = (1 - in.joystick[0].axis[5]) / 2; 
	//Since the throttle axis ranges from -1 to 1, need to make all values positive   
	//Also, (1 - Throttle) ranges from 0 to 2, so need to divide the values in half to range form 0 to 1
	//This means that by default the robot will run at 0.5x max speed when throttle is not pressed
	//This is how the robot has driven for the last few seasons
	Drive_motors d=func ( in.joystick[0].axis[0] * throttle, -in.joystick[0].axis[1] * throttle, in.joystick[0].axis[3] * throttle);
	r.pwm[0]=pwm_convert(d.a);
	r.pwm[1]=pwm_convert(d.b);
	r.pwm[2]=pwm_convert(d.c);
	
	//todo: double check that this is right.
	r.relay[0]=(in.digital_io[0]==DI_1)?RELAY_00:RELAY_10;
	r.pwm[3]=(ball_collecter.get());
	//r.pwm[3] = main_joystick.button[5]?1:(main_joystick.button[6?-1:0]);
	
	r=force(r);
	
	static int i=0;
	if(i==0){
		stringstream ss;
		ss<<in<<"\r\n"<<*this<<"\r\n";
		cerr<<ss.str();//putting this all together at once in hope that it'll show up at closer to the same time.  
	}
	i=(i+1)%100;
	
	string filename="demo.tmp";
	string data="lien1\nline2";
	/*if(i==10){
		int r=write_file(filename,data);
		if(r){
			cerr<<"Error while writing file:"<<r<<"\r\n";
		}
	}*/
	
	/*if(i==12){
		string out;
		int r=read_file(filename,out);
		if(r) cerr<<"Error during read:"<<r<<"\r";
		cerr<<"Got from file:"<<out<<"\r\n";
		cerr<<"Eqal?"<<(out==data)<<"\n";
	}*/
	
	/*if(i==20){
		string line;
		cerr<<"Getting line:";
		getline(cin,line);
		cerr<<"Got line:"<<line<<"\n";
	}*/
	
	return r;
}

ostream& operator<<(ostream& o,Main m){
	o<<"Main(";
	//o<<m.auto_timer<<" ";
	//o<<"traction_mode="<<m.goal_traction_mode<<" ";
	//o<<m.octocanum<<" ";
	o<<m.force;
	o<<m.perf;
	//o<<m.bunny_launcher;
	//o<<m.bunny_pooper;
	o<<m.gyro;
/*	for(unsigned i=0;i<Joystick_data::AXES;i++){
		o<<m.bound[i];
	}*/
	o<<m.bunnybot;
	return o<<")";
}

float newdistance(float x1, float y1, float x2, float y2, float x3);

float converttodistance(float value){
	if(value>.25&&value<=.35){
		return newdistance(.25,3,.35,2,value);
	}
	if(value>.35&&value<=.66){
		return newdistance(.35,2,.66,1,value);
	}
	if(value>.66&&value<=.79){
		return newdistance(.66,1,.79,.80,value);
	}
	if(value>.79&&value<=1.01){
		return newdistance(.79,.80,1.01,.60,value);
	}
	if(value>1.01&&value<=1.51){
		return newdistance(1.01,.60,1.51,.40,value);
	}
	if(value>1.51){
		return newdistance(1.51,.40,2.51,.20,value);
	}

	
	if(value<.26){
		return 3;
	}
	else if(value>=.26&&value<.51){
		return 2;
	}
	else if(value>=.51&&value<.73){
		return 1;
	}
	else if(value>=.73&&value<.81){
		return .8;
	}
	else if(value>=.81&&value<1.21){
		return .6;
	}
	else if(value>=1.21&&value<2.01){
		return .4;
	}
	else if(value>=2.01&&value<2.26){
		return 0;
	}
	else if(value>=2.26&&value<=2.6){
		return .2;
	}
	else
		return 0;	
}	
	
float newdistance(float x1, float y1, float x2, float y2, float x3){
	float m;
	m=(y2-y1)/(x2-x1);
	float b;
	b=y1-m*x1;
	return m*x3+b;
}
	
float timetowall(float value,float speed){	
	return converttodistance(value)/speed;
}
	
void getDistance(float value){
	cout<<converttodistance(value)<<"m\n";
}

enum WallCommand{
	OFF,RIGHT,LEFT,FRONT
};

/*WallCommand wallstop(double x,double y){
	if(x>=-0.2&&x<=0.2&&y>=-0.2&&y<=0.2){
		return OFF;
	}
	else if() 
*/	
	


#ifdef MAIN_TEST
int main(){
	/*Main m;
	cout<<m<<"\n";
	cout<<m(Robot_inputs())<<"\n";
	cout<<m<<"\n";
	float a;
	getDistance(2);
	for(float i = 0.26; i <= 3; i = i+0.5){
		cout<<i << " \n"; 
		getDistance(i);
	}*/
	/*cout<<timetowall(2.5,1)<<"\n";
	cout<<newdistance(.2,3,.7,2,.5)<<"\n";
	for(float input=0;input<3;input=input+0.1){
		a=converttodistance(input);
		cout<<input<<"	"<<a<<"\n";
	}*/
	cout<<func(0, 1, 0)<<"\n";
	cout<<func(0, -1, 0)<<"\n";
	cout<<func(0, 0, 1)<<"\n";
	cout<<func(0, 0, -1)<<"\n";
	cout<<func(1, 0, 0)<<"\n";
	cout<<func(-1, 0, 0)<<"\n";
	cout<<func(0, 0, 0)<<"\n";
}
#endif
