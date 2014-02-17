#include "main.h"
#include<iostream>
#include<sstream>
#include<cassert>
#include "octocanum.h"
#include "../util/util.h"
#include <math.h>
#include "holonomic.h"
#include "toplevel.h"
#include "fire_control.h"
#include "control_status.h"

using namespace std;

enum Joystick_section{JOY_LEFT,JOY_RIGHT,JOY_UP,JOY_DOWN,JOY_CENTER};

ostream& operator<<(ostream& o,Joystick_section j){
	switch(j){
		#define X(name) case JOY_##name: return o<<""#name;
		X(LEFT)
		X(RIGHT)
		X(UP)
		X(DOWN)
		X(CENTER)
		#undef X
		default: assert(0); 
	}
}

Joystick_section joystick_section(double x,double y){
	static const double LIM=.25;
	if(fabs(x)<LIM && fabs(y)<LIM){
		return JOY_CENTER;
	}
	if(x<y){
		if(x>-y){
			return JOY_DOWN;
		}
		return JOY_LEFT;
	}
	if(x>-y) return JOY_RIGHT;
	return JOY_UP;
}

Joystick_section divide_vertical(double y){ return joystick_section(0,y); }

double convert_output(Collector_mode m){
	switch(m){
		case ON: return -1; //Actually collecting
		case OFF: return 0;
		case REVERSE: return 1; //Ejecting
		default: assert(0);
	}
}

Robot_outputs convert_output(Toplevel::Output a){
	Robot_outputs r;
	Drive_motors d=holonomic_mix(a.drive);
	r.pwm[0]=d.a;
	r.pwm[1]=d.b;
	r.pwm[2]=d.c;
	r.pwm[3]=convert_output(a.collector);
	
	r.relay[0]=(a.pump==Pump::ON)?RELAY_10:RELAY_00;

	r.solenoid[0]=(a.collector_tilt==Collector_tilt::OUTPUT_DOWN);
	r.solenoid[1]=(a.collector_tilt==Collector_tilt::OUTPUT_UP);
	r.solenoid[2]=r.solenoid[3]=(a.injector==Injector::OUTPUT_DOWN);
	r.solenoid[4]=r.solenoid[5]=(a.injector==Injector::OUTPUT_UP);
	r.solenoid[6]=(a.ejector==Ejector::OUTPUT_UP);
	r.solenoid[7]=(a.injector_arms==Injector_arms::OUTPUT_CLOSE);

	//pressure switch
	r.digital_io[0]=DIO_INPUT;

	/*
	TODO: CAN bus stuff
	2 bottom
	6 top
	7 bottom
	9 top
	*/
	
	return r;
}

Toplevel::Mode to_mode(Control_status::Control_status status){
	switch(status){
		case Control_status::AUTO_SPIN_UP: return Toplevel::SHOOT_HIGH_PREP;
		case Control_status::AUTO_FIRE: return Toplevel::SHOOT_HIGH;
		case Control_status::AUTO_TO_COLLECT: return Toplevel::COLLECT;
		case Control_status::AUTO_COLLECT: return Toplevel::COLLECT;
		case Control_status::AUTO_SPIN_UP2: return Toplevel::SHOOT_HIGH_PREP;
		case Control_status::AUTO_FIRE2: return Toplevel::SHOOT_HIGH;
		case Control_status::DRIVE_W_BALL: return Toplevel::DRIVE_W_BALL;
		case Control_status::DRIVE_WO_BALL: return Toplevel::DRIVE_WO_BALL;
		case Control_status::COLLECT: return Toplevel::COLLECT;
		case Control_status::SHOOT_HIGH_PREP: return Toplevel::SHOOT_HIGH_PREP;
		case Control_status::SHOOT_HIGH: return Toplevel::SHOOT_HIGH;
		case Control_status::SHOOT_HIGH_WHEN_READY: return Toplevel::SHOOT_HIGH_PREP;
		case Control_status::TRUSS_TOSS_PREP: return Toplevel::TRUSS_TOSS_PREP;
		case Control_status::TRUSS_TOSS: return Toplevel::TRUSS_TOSS;
		case Control_status::TRUSS_TOSS_WHEN_READY: return Toplevel::TRUSS_TOSS_PREP;
		case Control_status::PASS_PREP: return Toplevel::PASS_PREP;
		case Control_status::PASS: return Toplevel::PASS;
		case Control_status::PASS_WHEN_READY: return Toplevel::PASS_PREP;
		case Control_status::EJECT_PREP: return Toplevel::EJECT_PREP;
		case Control_status::EJECT: return Toplevel::EJECT;
		case Control_status::EJECT_WHEN_READY: return Toplevel::EJECT_PREP;
		case Control_status::CATCH: return Toplevel::CATCH;
		default:
			assert(0);
	}
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

namespace Gamepad_button{
	//how the logitech gamepads appear in the driver station
	static const unsigned A=0,B=1,X=2;
	static const unsigned Y=3,LB=4,RB=5,BACK=6,START=7,L_JOY=8,R_JOY=9;
}
namespace Gamepad_axis{
	//How the axes appear in the DS; though
	static const unsigned LEFTX=0,LEFTY=1,RIGHTX=3,RIGHTY=4,TRIGGER=2,DPADX=5;
	//DPADY does not exist, neither does axis 6
}

//todo: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():control_status(Control_status::DRIVE_W_BALL){
	
	fieldRelative = false;
	isPressed = false;
}

Control_status::Control_status next(Control_status::Control_status status,Toplevel::Status part_status,Joystick_data j,bool autonomous_mode,Time since_switch);

Robot_outputs Main::operator()(Robot_inputs in){
	gyro.update(in.now,in.analog[0]);
	perf.update(in.now);
	since_switch.update(in.now,0);

	Joystick_data main_joystick=in.joystick[0];
	force.update(
		main_joystick.button[0],
		main_joystick.button[4],
		main_joystick.button[5],
		main_joystick.button[6],
		main_joystick.button[1],
		main_joystick.button[2]
	);

	Robot_outputs r;
	ball_collecter.update(main_joystick.button[5]);
	
	
	double throttle = 1.0;
	/*
	if (main_joystick.axis[Gamepad_axis::TRIGGER] > 0.5 || 
		main_joystick.axis[Gamepad_axis::TRIGGER] < -0.5)
	{
			throttle = 0.5;
	}
	*/
	
	//Well they said the robot needs to go full speed all the time
	//Throttle now scales down speeds by 50% and activates when either of the triggers is pulled
	//Start in NOT fieldRelative Mode
	
	//Check to see if somebody pushed the field relative button and turn on/off the mode
	if (main_joystick.button[Gamepad_button::X]) {
		if (!isPressed) {
			isPressed = true;
			fieldRelative = !fieldRelative; //Turn fieldRelative on/off
			if(fieldRelative==false){
				cout<<"Field Relative is now OFF!"<<"\n";
			}
			else{
				cout<<"Field Relative is now ON!"<<"\n";
			}
		}
	} else {
		isPressed = false;
	}
	
	Drive_motors d=holonomic_mix( 
			main_joystick.axis[Gamepad_axis::LEFTX] * throttle, 
			-main_joystick.axis[Gamepad_axis::LEFTY] * throttle, 
			main_joystick.axis[Gamepad_axis::RIGHTX] * throttle,
			gyro.angle(),
			fieldRelative);
	r.pwm[0]=pwm_convert(d.a);
	r.pwm[1]=pwm_convert(d.b);
	r.pwm[2]=pwm_convert(d.c);
	
	//todo: double check that this is right.
	bool tanks_full=(in.digital_io[0]==DI_1);
	r.relay[0]=tanks_full?RELAY_00:RELAY_10;
	r.pwm[3]=(ball_collecter.get());
	//r.pwm[3] = main_joystick.button[5]?1:(main_joystick.button[6?-1:0]);
	
	//Control_status::Control_status next(Control_status::Control_status status,Toplevel::Status part_status,Joystick_data j,bool autonomous_mode,Time since_switch){
	Toplevel::Status toplevel_status=est.estimate();
	control_status=next(control_status,toplevel_status,in.joystick[1],in.robot_mode.autonomous,since_switch.elapsed());

	Toplevel::Mode mode=to_mode(control_status);
	Toplevel::Subgoals subgoals_now=subgoals(mode);
	Toplevel::Output high_level_outputs=control(toplevel_status,subgoals_now);
	r=convert_output(high_level_outputs);
	est.update(in.now,high_level_outputs,tanks_full?Pump::FULL:Pump::NOT_FULL);

	{
		Drive_motors d=holonomic_mix( 
			main_joystick.axis[0] * throttle, 
			-main_joystick.axis[1] * throttle, 
			main_joystick.axis[3] * throttle,
			gyro.angle(),
			fieldRelative
		);
		r.pwm[0]=pwm_convert(d.a);
		r.pwm[1]=pwm_convert(d.b);
		r.pwm[2]=pwm_convert(d.c);
	}

	r=force(r);
	
	static int i=0;
	if(i==0){
		stringstream ss;
		ss<<in<<"\r\n"<<*this<<"\r\n";
		cerr<<ss.str();//putting this all together at once in hope that it'll show up at closer to the same time.  
		cerr<<subgoals_now<<high_level_outputs<<"\n";
	}
	i=(i+1)%100;
	if(print_button(main_joystick.button[Gamepad_button::LB])){
		cout<<in<<"\r\n";
		cout<<*this<<"\r\n";
		cout<<"\r\n";
	}
	
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
	o<<m.force;
	o<<m.perf;
	o<<m.gyro;
	o<<m.control_status;
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

/*enum WallCommand{
	OFF,RIGHT,LEFT,FRONT
};*/

/*WallCommand wallstop(double x,double y){
	if(x>=-0.2&&x<=0.2&&y>=-0.2&&y<=0.2){
		return OFF;
	}
	else if() 
*/	

Control_status::Control_status next(Control_status::Control_status status,Toplevel::Status part_status,Joystick_data j,bool autonomous_mode,Time since_switch){
	using namespace Control_status;
	//at the top here should deal with all the buttons that put you into a specific mode.
	if(j.button[Gamepad_button::A]) return Control_status::CATCH;
	if(j.button[Gamepad_button::B]) return COLLECT;
	if(j.button[Gamepad_button::X]) return DRIVE_W_BALL;
	if(j.button[Gamepad_button::Y]) return DRIVE_WO_BALL;

	//todo: use some sort of constants rather than 0/1 for the axes
	if(Fire_control::target(status)==Fire_control::NO_TARGET){
		switch(joystick_section(j.axis[0],j.axis[1])){
			case JOY_LEFT: return TRUSS_TOSS_PREP;
			case JOY_RIGHT: return PASS_PREP;
			case JOY_UP: return SHOOT_HIGH_PREP;
			case JOY_DOWN: return EJECT_PREP;
			default: break;
		}
	}
	bool fire_now,fire_when_ready;
	{
		//I think 3 is the right axis
		Joystick_section vert=divide_vertical(j.axis[Gamepad_axis::RIGHTY]);
		//cerr<<"vert ="<<vert<<"\n";
		fire_now=(vert==JOY_UP);
		fire_when_ready=(vert==JOY_DOWN);
	}

	bool ready_to_shoot=ready(part_status,subgoals(Toplevel::SHOOT_HIGH_PREP));
	bool ready_to_truss_toss=ready(part_status,subgoals(Toplevel::TRUSS_TOSS_PREP));
	bool ready_to_pass=ready(part_status,subgoals(Toplevel::PASS_PREP));
	bool ready_to_collect=ready(part_status,subgoals(Toplevel::COLLECT));
	bool took_shot=location_to_status(part_status.injector)==Injector::RECOVERY;
	bool have_collected_question = false;
	switch(status){
		case AUTO_SPIN_UP:
			if(autonomous_mode){
				return ready_to_shoot?AUTO_FIRE:AUTO_SPIN_UP;
			}
			return SHOOT_HIGH_PREP;
		case AUTO_FIRE:
			if(autonomous_mode){
				return took_shot?AUTO_TO_COLLECT:AUTO_FIRE;
			}
			return SHOOT_HIGH;
		case AUTO_TO_COLLECT:
			if(autonomous_mode){
				return ready_to_collect?AUTO_COLLECT:AUTO_TO_COLLECT;
			}
			return COLLECT;
		case AUTO_COLLECT:
			if(autonomous_mode){
				//this is a very non-scientific way of driving, and not really the right way to do this.
				return (since_switch>2.4)?AUTO_SPIN_UP2:AUTO_COLLECT;
			}
			return COLLECT;
		case AUTO_SPIN_UP2:
			if(autonomous_mode){
				return ready_to_shoot?AUTO_FIRE2:AUTO_SPIN_UP2;
			}
			return SHOOT_HIGH_PREP;
		case AUTO_FIRE2:
			if(autonomous_mode){
				return took_shot?DRIVE_WO_BALL:AUTO_FIRE2;
			}
			return SHOOT_HIGH;
		case DRIVE_W_BALL:
		case DRIVE_WO_BALL:
			return status;
		case COLLECT:
			return have_collected_question?DRIVE_W_BALL:COLLECT;
		case SHOOT_HIGH_PREP:
			if(fire_now){
				return SHOOT_HIGH;
			}
			if(fire_when_ready){
				return SHOOT_HIGH_WHEN_READY;
			}
			return SHOOT_HIGH_PREP;
		case SHOOT_HIGH: return took_shot?DRIVE_WO_BALL:SHOOT_HIGH;
		case SHOOT_HIGH_WHEN_READY:
			if(fire_now){
				return SHOOT_HIGH;
			}
			if(!fire_when_ready){
				return SHOOT_HIGH_PREP;
			}
			return ready_to_shoot?SHOOT_HIGH:SHOOT_HIGH_WHEN_READY;
		case TRUSS_TOSS_PREP: 
			if(fire_now){
				return TRUSS_TOSS;
			}
			if(fire_when_ready){
				return TRUSS_TOSS_WHEN_READY;
			}
			return TRUSS_TOSS_PREP;
		case TRUSS_TOSS: return took_shot?DRIVE_WO_BALL:TRUSS_TOSS;
		case TRUSS_TOSS_WHEN_READY: 
			if(fire_now){
				return TRUSS_TOSS;
			}
			if(!fire_when_ready){
				return TRUSS_TOSS_PREP;
			}
			return ready_to_truss_toss?TRUSS_TOSS:TRUSS_TOSS_WHEN_READY;
		case PASS_PREP: 
			if(fire_now){
				return PASS;
			}
			if(fire_when_ready){
				return PASS_WHEN_READY;
			}
			return PASS_PREP;
		case PASS: return took_shot?DRIVE_WO_BALL:PASS;
		case PASS_WHEN_READY: 
			if(fire_now){
				return PASS;
			}
			if(!fire_when_ready){
				return PASS_PREP;
			}
			return ready_to_pass?PASS:PASS_WHEN_READY;
		case EJECT_PREP: 
			if(fire_now){
				return EJECT;
			}
			if(fire_when_ready){
				return EJECT_WHEN_READY;
			}
			return EJECT_PREP;
		case EJECT: return (location_to_status(part_status.ejector)==Ejector::RECOVERY)?DRIVE_WO_BALL:EJECT;
		case EJECT_WHEN_READY:
		{
			bool ready_to_eject=(location_to_status(part_status.ejector)==Ejector::IDLE);
			return ready_to_eject?EJECT:EJECT_WHEN_READY;
		}
		case CATCH: return status;
		default:
			assert(0);
	}
}

/*struct Gunner_input{
	//this could happen in a more elegant way.
	Posedge_trigger drive_w_ball,drive_wo_ball,catch_mode,collect,prep_high,prep_toss,prep_pass,prep_eject,high,toss,pass,eject;
	Mode current;
	
	Gunner_input():current(DRIVE_W_BALL){}
	
	void update(Joystick_data j){
		using namespace Gamepad_button;
		using namespace Toplevel;
		if(drive_w_ball.update(j.button[X])){
			current=DRIVE_W_BALL;
		}
		if(drive_wo_ball.update(j.button[Y])){
			current=DRIVE_WO_BALL;
		}
		if(catch_mode.update(j.button[B])) current=CATCH;
		if(collect.update(j.button[A])) current=COLLECT;
		
		Joystick_section ls=joystick_section(j.axis[0],j.axis[1]);
		if(prep_high.update(ls==JOY_UP)) current=SHOOT_HIGH_PREP;
		if(prep_toss.update(ls==JOY_LEFT)) current=TRUSS_TOSS_PREP;
		if(prep_eject.update(ls==JOY_DOWN)) current=EJECT_PREP;
		if(prep_pass.update(ls==JOY_RIGHT)) current=PASS_PREP;
		
		Joystick_section rs=joystick_section(j.axis[2],j.axis[3]);
		if(high.update(rs==JOY_UP)) current=SHOOT_HIGH;
		if(toss.update(rs==JOY_LEFT)) current=TRUSS_TOSS;
		if(eject.update(rs==JOY_DOWN)) current=EJECT;
		if(pass.update(rs==JOY_RIGHT)) current=PASS;
	}
};*/

#ifdef MAIN_TEST
void joystick_section_test(){
	assert(joystick_section(0,0)==JOY_CENTER);
	assert(joystick_section(-1,0)==JOY_LEFT);
	assert(joystick_section(1,0)==JOY_RIGHT);
	assert(joystick_section(0,-1)==JOY_UP);
	assert(joystick_section(0,1)==JOY_DOWN);
}

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
	/*
	cout<<func(0, 1, 0)<<"\n";
	cout<<func(0, -1, 0)<<"\n";
	cout<<func(0, 0, 1)<<"\n";
	cout<<func(0, 0, -1)<<"\n";
	cout<<func(1, 0, 0)<<"\n";
	cout<<func(-1, 0, 0)<<"\n";
	cout<<func(0, 0, 0)<<"\n";
	*/
	joystick_section_test();
	Main m;
	cout<<m<<"\n";
	for(Control_status::Control_status control_status:Control_status::all()){
		cout<<control_status<<" "<<to_mode(control_status)<<"\n";
	}
}
#endif
