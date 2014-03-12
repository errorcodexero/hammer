#include "main.h"
#include<iostream>
#include<sstream>
#include<cassert>
#include <math.h>
#include "holonomic.h"
#include "toplevel.h"
#include "fire_control.h"
#include "control_status.h"
#include "../input/util.h"
#include "../input/panel2014.h"
#include "../util/util.h"

using namespace std;

double convert_output(Collector_mode m){
	switch(m){
		case ON: return -1; //Actually collecting
		case OFF: return 0;
		case REVERSE: return 1; //Ejecting
		default: assert(0);
	}
}

static const int JAG_TOP_FEEDBACK=1;
static const int JAG_BOTTOM_FEEDBACK=3;
static const int JAG_TOP_OPEN_LOOP=0;
static const int JAG_BOTTOM_OPEN_LOOP=2;

Robot_outputs convert_output(Toplevel::Output a){
	Robot_outputs r;
	r.pwm[0]=pwm_convert(a.drive.a);
	r.pwm[1]=pwm_convert(a.drive.b);
	r.pwm[2]=pwm_convert(a.drive.c);
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

	//cerr<<a.shooter_wheels<<"\r\n";
	r.jaguar[JAG_TOP_FEEDBACK]=a.shooter_wheels.top[Shooter_wheels::Output::FEEDBACK];
	r.jaguar[JAG_BOTTOM_FEEDBACK]=a.shooter_wheels.bottom[Shooter_wheels::Output::FEEDBACK];
	r.jaguar[JAG_TOP_OPEN_LOOP]=a.shooter_wheels.top[Shooter_wheels::Output::OPEN_LOOP];
	r.jaguar[JAG_BOTTOM_OPEN_LOOP]=a.shooter_wheels.bottom[Shooter_wheels::Output::OPEN_LOOP];
	/*for(unsigned i=0;i<4;i++){
		cerr<<r.jaguar[i]<<"\r\n";
	}*/
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

//todo: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():control_status(Control_status::DRIVE_W_BALL),autonomous_start(0){}

Control_status::Control_status next(Control_status::Control_status status,Toplevel::Status part_status,Joystick_data j,Panel,bool autonomous_mode,bool autonomous_mode_start,Time since_switch,wheelcalib);

Drive_goal teleop_drive_goal(double joy_x,double joy_y,double joy_theta,double joy_throttle,bool field_relative){
	
	double throttle = (1 + -joy_throttle) / 2;
	joy_x = clip(joy_x) * throttle;
	joy_y = -clip(joy_y) * throttle;//Invert Y
	joy_theta = clip(joy_theta) * 0.75;
	return Drive_goal(Pt(joy_x,joy_y,joy_theta),field_relative);
}

Drive_goal drive_goal(Control_status::Control_status control_status,double joy_x,double joy_y,double joy_theta,double joy_throttle,bool field_relative){
	if(teleop(control_status)){
		return teleop_drive_goal(joy_x,joy_y,joy_theta,joy_throttle,field_relative);
	}
	Drive_goal r;
	switch(control_status){
		case Control_status::AUTO_COLLECT:
			r.direction.y=-.5;
			break;
		default:
			//otherwise leave at the default, which is 0.
			break;
	}
	return r;
}

Toplevel::Output panel_override(Panel p,Toplevel::Output out){
	#define X(name) if(p.name) out.name=*p.name;
	X(collector)
	X(collector_tilt)
	X(injector)
	//X(injector_arms)
	X(ejector)
	#undef X
	if(p.force_wheels_off){
		out.shooter_wheels=Shooter_wheels::Output();
	}
	return out;
}

bool vowel(char c){
	c=tolower(c);
	return c=='a' || c=='e' || c=='i' || c=='o' || c=='u' || c=='y';
}

string abbreviate_text(string s){
	stringstream ss;
	//bool skipped_last=0;
	for(unsigned i=0;i<s.size();i++){
		if(s[i]==' ' || s[i]==':' || s[i]=='_') continue;
		if(vowel(s[i])){
			//skip
		}else{
			ss<<s[i];
		}
	}
	return ss.str();
}

string fRel(bool field){
	if(field){
		return "True";
	}
	return "False";
}

void log_line(ostream& o,Robot_inputs in,Main m,Robot_outputs out){
	o<<in.now;
	#define X(name) o<<","<<name;
	X(in.robot_mode.autonomous)
	X(in.robot_mode.enabled)
	for(unsigned i=0;i<Joystick_data::AXES;i++){
		X(in.joystick[0].axis[i])
	}
	for(unsigned i=0;i<Joystick_data::BUTTONS;i++){
		X(in.joystick[0].button[i])
	}
	X(in.digital_io[0]) //pressure switch
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		X(in.jaguar[i].speed)
	}
	for(unsigned i=0;i<Driver_station_input::ANALOG_INPUTS;i++){
		X(in.driver_station.analog[i])
	}
	for(unsigned i=0;i<Driver_station_input::DIGITAL_INPUTS;i++){
		X(in.driver_station.digital[i])
	}
	//going to not put in the interpreted version of this in case need to diagnose a noise problem or something.  
	
	//put in the state stuff here...
	//skipping any force stuff because I don't think our drivers know how to use it.  Would be detectable via joystic inputs.
	X(m.control_status)
	Toplevel::Status e=m.est.estimate();
	X(e.collector_tilt)
	X(e.injector)
	X(e.injector_arms)
	X(e.ejector)
	X(e.shooter_wheels)
	//skip pump
	//skip orientation

	//X(m.wheel_calibration) -> this might be good to do
	#define Y(n) X(m.wheel_calibration.calib.n.top) X(m.wheel_calibration.calib.n.bottom)
	Y(highgoal)
	Y(lowgoal)
	Y(overtruss)
	Y(passing)
	#undef Y

	for(unsigned i=0;i<4;i++){ //others aren't used.
		X((int)out.pwm[i])
	}
	for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
		X(out.solenoid[i])
	}
	for(unsigned i=0;i<Robot_outputs::RELAYS && i<2;i++){
		X(out.relay[i])
	}
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		X(out.jaguar[i])
	}
	X(out.driver_station.digital[7])//ready light
	#undef X
	o<<"\n";
}

Robot_outputs Main::operator()(Robot_inputs in){
	gyro.update(in.now,in.analog[0]);
	perf.update(in.now);
	since_switch.update(in.now,0);
	Joystick_data gunner_joystick=in.joystick[1];
	Joystick_data main_joystick=in.joystick[0];
	force.update(
		main_joystick.button[0],
		main_joystick.button[4],
		main_joystick.button[5],
		main_joystick.button[6],
		main_joystick.button[1],
		main_joystick.button[2]
	);


	ball_collecter.update(main_joystick.button[5]);
	bool tanks_full=(in.digital_io[0]==DI_1);

	Panel panel=interpret(in.driver_station);	
	wheelcalib calib=wheel_calibration.update(panel.learn,panel.speed,panel.target);
	//Control_status::Control_status next(Control_status::Control_status status,Toplevel::Status part_status,Joystick_data j,bool autonomous_mode,Time since_switch){
	Toplevel::Status toplevel_status=est.estimate();
	Control_status::Control_status control_status_next=next(
		control_status,toplevel_status,gunner_joystick,panel,
		in.robot_mode.autonomous,
		autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled),
		since_switch.elapsed(),
		calib
	);
	if(control_status_next!=control_status){
		since_switch.update(in.now,1);
	}
	control_status=control_status_next;
	
	field_relative.update(main_joystick.button[Gamepad_button::X]);
	
	Toplevel::Mode mode=to_mode(control_status);
	Drive_goal drive_goal1=drive_goal(
		control_status,
		main_joystick.axis[Gamepad_axis::LEFTX],
		main_joystick.axis[Gamepad_axis::LEFTY],
		main_joystick.axis[Gamepad_axis::RIGHTX],
		main_joystick.axis[Gamepad_axis::TRIGGER],
		field_relative.get()
	);
	Toplevel::Subgoals subgoals_now=subgoals(mode,drive_goal1,calib);
	if(toplevel_status.injector!=Injector::Estimator::DOWN_IDLE&&toplevel_status.injector!=Injector::Estimator::DOWN_VENT){
		subgoals_now.injector_arms=Injector_arms::GOAL_CLOSE;
	}
	Toplevel::Output high_level_outputs=control(toplevel_status,subgoals_now);
	high_level_outputs=panel_override(panel,high_level_outputs);
	if(gunner_joystick.button[Gamepad_button::START]){
		high_level_outputs.collector=REVERSE;
	}
	Robot_outputs r=convert_output(high_level_outputs);
	{
		Shooter_wheels::Status wheel;
		wheel.top=in.jaguar[JAG_TOP_FEEDBACK].speed;
		wheel.bottom=in.jaguar[JAG_BOTTOM_FEEDBACK].speed;
		est.update(in.now,in.robot_mode.enabled,high_level_outputs,tanks_full?Pump::FULL:Pump::NOT_FULL,gyro.angle(),wheel);
	}
	
        // Turn on camera light in autonomous mode:
        r.relay[1] = (in.robot_mode.autonomous) ? RELAY_01 : RELAY_00;

	r=force(r);
	
	r.driver_station.lcd.line[0]="FieldRelative: " + fRel(field_relative.get());//as_string(field_relative.get());
	r.driver_station.lcd.line[1]=as_string(r.jaguar[0]).substr(13, 20);
	r.driver_station.lcd.line[2]=as_string(r.jaguar[1]).substr(13, 20);
	r.driver_station.lcd.line[3]=as_string(r.jaguar[2]).substr(13, 20);
	r.driver_station.lcd.line[4]=as_string(r.jaguar[3]).substr(13, 20);
	stringstream strin;
	strin<<toplevel_status.shooter_wheels;
	r.driver_station.lcd.line[5]="Speeds:"+strin.str().substr(22, 20);

	//r.driver_station.lcd=format_for_lcd(as_string(in.now)+as_string(in.driver_station));
	/*r.driver_station.lcd=format_for_lcd(
		//abbreviate_text(as_string(in.now)+"\n"+as_string(panel)+as_string(in.driver_station))
		as_string(subgoals_now)+as_string(toplevel_status)
	);*/
	//r.driver_station.lcd=format_for_lcd(as_string(panel));
	r.driver_station.digital[7]=ready(toplevel_status,subgoals_now);
	{
		static int i=0;
		if(i==0){
			stringstream ss;
			ss<<in<<"\r\n";//<<*this<<"\r\n";
			ss<<panel<<"\r\n";
			/*
			ss<<"Gyro Voltage"<<in.analog[0]<<" "<<"Update"<<gyro.center<<"\r\n";
			ss<<"Gyro Value:"<<gyro.angle();
			*/
			ss<<in.driver_station<<"\r\n";
			ss<<"\n"<<"Field Relative?:"<<field_relative.get()<<"\n";
			cerr<<ss.str();//putting this all together at once in hope that it'll show up at closer to the same time.  
			//cerr<<subgoals_now<<high_level_outputs<<"\n";
		}
		i=(i+1)%100;
	}
	//cerr<<subgoals_now<<"\r\n";
	//cerr<<toplevel_status<<"\r\n\r\n";
	//cerr<<"Waiting on:"<<not_ready(toplevel_status,subgoals_now)<<"\n";
	
	if(print_button(main_joystick.button[Gamepad_button::LB])){
		cout<<in<<"\r\n";
		cout<<*this<<"\r\n";
		cout<<"\r\n";
	}
	//log_line(cout,in,*this,r);
	return r;
}

bool operator==(Main a,Main b){
	return a.force==b.force && a.perf==b.perf && a.gyro==b.gyro && a.est==b.est && a.control_status==b.control_status && a.since_switch==b.since_switch && a.ball_collecter==b.ball_collecter && a.print_button==b.print_button && a.field_relative==b.field_relative;
}

bool operator!=(Main a,Main b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Main m){
	o<<"Main(";
	o<<m.force;
	o<<m.perf;
	o<<m.gyro;
	o<<m.est;
	o<<m.control_status;
	o<<m.since_switch;
	o<<m.wheel_calibration;
	//o<<m.control;
	//ball collector
	//print button
	//relative
	//field relative
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

Fire_control::Target to_target(Joystick_section j,Mode_buttons mode_buttons){
	switch(j){
		case JOY_LEFT: return Fire_control::TRUSS;
		case JOY_RIGHT: return Fire_control::PASS;
		case JOY_UP: return Fire_control::HIGH;
		case JOY_DOWN: return Fire_control::EJECT;
		default: break;
	}
	if(mode_buttons.truss_toss) return Fire_control::TRUSS;
	if(mode_buttons.shoot_high) return Fire_control::HIGH;
	if(mode_buttons.pass) return Fire_control::PASS;
	if(mode_buttons.eject) return Fire_control::EJECT;
	return Fire_control::NO_TARGET;
}

Control_status::Control_status next(
	Control_status::Control_status status,
	Toplevel::Status part_status,
	Joystick_data j,
	Panel panel,
	bool autonomous_mode,
	bool autonomous_mode_start,
	Time since_switch,
	wheelcalib calib
){
	using namespace Control_status;

	//if(autonomous_mode && !autonomous(status)){
	if(autonomous_mode_start){
		//TODO: Put the code to select which autonomous mode here.
		return AUTO_SPIN_UP;
	}

	if(!autonomous_mode){
		//at the top here should deal with all the buttons that put you into a specific mode.
		if(j.button[Gamepad_button::A] || panel.mode_buttons.catch_mode) return Control_status::CATCH;
		if(j.button[Gamepad_button::B] || panel.mode_buttons.collect) return COLLECT;
		if(j.button[Gamepad_button::X] || panel.mode_buttons.drive_w_ball) return DRIVE_W_BALL;
		if(j.button[Gamepad_button::Y] || panel.mode_buttons.drive_wo_ball) return DRIVE_WO_BALL;
		//Changed so as not to accidentally time out the robot
		//if(j.button[Gamepad_button::Y] || panel.mode_buttons.drive_wo_ball) return Control_status::SHOOT_LOW;
		//todo: use some sort of constants rather than 0/1 for the axes
		{
			Joystick_section joy_section=joystick_section(j.axis[0],j.axis[1]);
			Fire_control::Target target=to_target(joy_section,panel.mode_buttons);
			if(Fire_control::target(status)!=target && !autonomous_mode){
				switch(target){
					case Fire_control::TRUSS: return TRUSS_TOSS_PREP;
					case Fire_control::PASS: return PASS_PREP;
					case Fire_control::HIGH: return SHOOT_HIGH_PREP;
					case Fire_control::EJECT: return EJECT_PREP;
					default: break;
				}
			}
		}
	}
	bool fire_now,fire_when_ready;
	{
		Joystick_section vert=divide_vertical(j.axis[Gamepad_axis::RIGHTY]);
		fire_now=(vert==JOY_UP) || panel.fire;
		fire_when_ready=(vert==JOY_DOWN); //No equivalent on the switchpanel.
	}

	bool ready_to_shoot=ready(part_status,subgoals(Toplevel::SHOOT_HIGH_PREP,Drive_goal(),calib));
	bool ready_to_truss_toss=ready(part_status,subgoals(Toplevel::TRUSS_TOSS_PREP,Drive_goal(),calib));
	bool ready_to_pass=ready(part_status,subgoals(Toplevel::PASS_PREP,Drive_goal(),calib));
	bool ready_to_collect=ready(part_status,subgoals(Toplevel::COLLECT,Drive_goal(),calib));
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
				return (since_switch>2.4)?/*AUTO_SPIN_UP2*/DRIVE_WO_BALL:AUTO_COLLECT;
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

template<typename T>
void print_diff(ostream& o,string s,T a,T b){
	if(a!=b){
		o<<s<<"From "<<a<<" to "<<b<<"\n";
	}
}

template<typename T>
void print_diff(ostream& o,T a,T b){
	print_diff(o,"",a,b);
}

template<typename T>
void print_diff_approx(ostream& o,T a,T b){
	if(!approx_equal(a,b)){
		o<<"From "<<a<<" to "<<b<<"\n";
	}
}

void print_diff(ostream& o,unsigned char a,unsigned char b){
	print_diff(o,(int)a,(int)b);
}

void print_diff(ostream& o,Gyro_tracker a,Gyro_tracker b){ print_diff_approx(o,a,b); }

void print_diff(ostream& o,Toplevel::Status a,Toplevel::Status b){
	#define X(name) print_diff(o,""#name ": ",a.name,b.name);
	X(collector_tilt)
	X(injector)
	X(injector_arms)
	X(ejector)
	print_diff_approx(o,a.shooter_wheels,b.shooter_wheels);
	X(pump)
	//X(orientation)
	//print_diff_approx(o,a.orientation,b.orientation);
	#undef X
}

void print_diff(ostream& o,Toplevel::Estimator a,Toplevel::Estimator b){
	print_diff(o,a.estimate(),b.estimate());
}

void print_diff(ostream& o,Main a,Main b){
	#define X(name) print_diff(o,""#name ": ",a.name,b.name);
	#define Y(name) print_diff(o,a.name,b.name);
	X(force)
	Y(gyro);
	Y(est)
	X(control_status)
	X(ball_collecter)
	X(field_relative)
	#undef Y
	#undef X
}

void print_diff(ostream& o,Driver_station_output a,Driver_station_output b){
	#define X(name) print_diff(o,a.name,b.name);
	//X(lcd) skipping this for now since it really clutters the prinout right now since it shows shooter wheel speeds
	for(unsigned i=0;i<Driver_station_output::DIGITAL_OUTPUTS;i++){
		if(a.digital[i]!=b.digital[i]){
			o<<"Driver_station_output::digital["<<i<<"]:"<<a.digital[i]<<"->"<<b.digital[i]<<"\n";
		}
	}
	#undef X
}

void print_diff(ostream& o,Robot_outputs a,Robot_outputs b){
	for(unsigned i=0;i<Robot_outputs::PWMS;i++){
		if(a.pwm[i]!=b.pwm[i]){
			o<<"pwm"<<i<<" "<<(int)a.pwm[i]<<"->"<<(int)b.pwm[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
		if(a.solenoid[i]!=b.solenoid[i]){
			o<<"solenoid"<<i<<" "<<a.solenoid[i]<<"->"<<b.solenoid[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::RELAYS;i++){
		if(a.relay[i]!=b.relay[i]){
			o<<"relay"<<i<<" "<<a.relay[i]<<"->"<<b.relay[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		if(a.digital_io[i]!=b.digital_io[i]){
			o<<"digital_io"<<i<<" "<<a.digital_io[i]<<"->"<<b.digital_io[i]<<"\n";
		}
	}
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		if(a.jaguar[i]!=b.jaguar[i]){
			o<<"jaguar"<<i<<" "<<a.jaguar[i]<<"->"<<b.jaguar[i]<<"\n";
		}
	}
	print_diff(o,a.driver_station,b.driver_station);
}

void print_diff(ostream& o,string s,Jaguar_input a,Jaguar_input b){
	if(!approx_equal(a,b)){
		o<<s<<a<<"->"<<b<<"\n";
	}
}

void print_diff(ostream& o,Robot_inputs a,Robot_inputs b){
	#define X(name) print_diff(o,""#name ": ",a.name,b.name);
	X(robot_mode)
	//X(now)
	for(unsigned i=0;i<Robot_inputs::JOYSTICKS;i++){
		X(joystick[i])
	}
	for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
		X(digital_io[i])
	}
	for(unsigned i=0;i<Robot_inputs::ANALOG_INPUTS;i++){
		X(analog[i])
	}
	for(unsigned i=0;i<Robot_outputs::CAN_JAGUARS;i++){
		X(jaguar[i])
	}
	X(driver_station)
	#undef X
}

bool approx_equal(Main a,Main b){
	//cout<<"a\n";
	if(a.force!=b.force) return 0;
	//cout<<"b\n";
	//if(!approx_equal(a.perf,b.perf)) return 0;
	if(!approx_equal(a.gyro,b.gyro)) return 0;
	//cout<<"1\n";
	if(!approx_equal(a.est,b.est)) return 0;
	//cout<<"a\n";
	if(a.control_status!=b.control_status) return 0;
	if(a.ball_collecter!=b.ball_collecter) return 0;
	//cout<<"b\n";
	if(a.print_button!=b.print_button) return 0;
	if(a.field_relative!=b.field_relative) return 0;
	return 1;
}

#ifdef MAIN_TEST
#include<fstream>
#include "wheel_sim.h"

template<typename T>
struct Monitor{
	T data;

	string update(T t){
		stringstream ss;
		print_diff(ss,data,t);
		data=t;
		return ss.str();
	}
};

Jaguar_input jag_at_speed(double speed){
	Jaguar_input r;
	r.speed=speed;
	return r;
}

Shooter_wheels::Output shooter_output(Robot_outputs out){
	Shooter_wheels::Output r;
	r.top[Shooter_wheels::Output::FEEDBACK]=out.jaguar[JAG_TOP_FEEDBACK];
	r.top[Shooter_wheels::Output::OPEN_LOOP]=out.jaguar[JAG_TOP_OPEN_LOOP];
	r.bottom[Shooter_wheels::Output::FEEDBACK]=out.jaguar[JAG_BOTTOM_FEEDBACK];
	r.bottom[Shooter_wheels::Output::OPEN_LOOP]=out.jaguar[JAG_BOTTOM_OPEN_LOOP];
	return r;
}

void auto_test(){
	Main m;
	Monitor<Robot_inputs> inputs;
	Monitor<Main> state;
	Monitor<Robot_outputs> outputs;
	Shooter_sim shooter_sim;
	for(unsigned i=0;i<1500;i++){
		Robot_inputs in;
		in.now=i/100.0;
		in.robot_mode.autonomous=1;
		in.robot_mode.enabled=1;
		//in.jaguar[JAG_TOP_OPEN_LOOP]=leave at 0
		in.jaguar[JAG_TOP_FEEDBACK]=jag_at_speed(shooter_sim.estimate().top);
		//in.jaguar[JAG_BOTTOM_OPEN_LOOP]=
		in.jaguar[JAG_BOTTOM_FEEDBACK]=jag_at_speed(shooter_sim.estimate().bottom);
		auto out_now=m(in);
		shooter_sim.update(in.now,shooter_output(out_now));
		string change;
		change+=inputs.update(in);
		change+=state.update(m);
		change+=outputs.update(out_now);
		if(change.size()){
			cout<<"Now="<<in.now<<"\n";
			cout<<change<<"\n";
		}
	}
}

void mode_table(){
	static ofstream f("control_modes.html");
	struct Tag{
		string s;
		Tag(string s1):s(s1){
			f<<"<"<<s<<">";
			s=split(s1)[0];
		}

		~Tag(){
			f<<"</"<<s<<">";
		}
	};
	Tag t("html");

	Tag b("body");
	Tag a("table border");
	{
		Tag a("tr");
		{
			Tag b("th");
			f<<"Control status";
		}
		Tag c("th");
		f<<"Mode";
	}
	for(auto control_status:Control_status::all()){
		Tag r("tr");
		{
			Tag d("td");
			f<<control_status;
		}
		Tag d("td");
		f<<to_mode(control_status);
	}
}

void mode_diagram(){
	ofstream f("control_modes.dot");
	f<<"digraph G{\n";
	for(auto a:Control_status::all()){
		string x=as_string(a),y=as_string(to_mode(a));
		if(x!=y){
			f<<"\t"<<a<<"->"<<to_mode(a)<<"\n";
		}
	}
	f<<"}\n";
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
	bool test_control_status=1;
	if(test_control_status){
		for(Control_status::Control_status control_status:Control_status::all()){
			cout<<control_status<<" "<<to_mode(control_status)<<"\n";
		}
	}
	auto_test();
	mode_table();
	mode_diagram();
}
#endif
