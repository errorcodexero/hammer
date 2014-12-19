#include <iostream>
#include <vector>
#include <iomanip>
#include <assert.h>
#include "../util/interface.h"

using namespace std;

struct Encoder_output{//This is a type for the input received from the encoder -- "a" (pin two) and "b" (pin four)
	bool a,b;
};

struct Encoder_return{//The important data values for each encoder are stored in this type
	char wheel;
	double estimated_rotation;
	unsigned int time;
	Encoder_output states;
	//Encoder_return():estimated_rotation(0), time(0){}
};

ostream & operator<<(ostream & o, Encoder_output in){//Outputs from the vector if type Input the states of the a and b channels as this code interprets it
	o<<in.a<<" | "<<in.b;
	return o;
}

ostream & operator<<(ostream & o, vector<Encoder_output> v){
	unsigned int i=0;
	for(; i<v.size(); i++){
		o<<v[i]<<"|";
	}
	return o;
}

void print_encoder_values(Encoder_return encoder_return_l, Encoder_output pre_encoder_states_l, Encoder_return encoder_return_b, Encoder_output pre_encoder_states_b, Encoder_return encoder_return_r, Encoder_output pre_encoder_states_r){//Prints out the values of a and b and the estimated rotation direction for each wheel over time
	if(encoder_return_l.time<10){
		cerr<<"  "<<encoder_return_l.time<<"        | "<<pre_encoder_states_l<<" | "<<encoder_return_l.estimated_rotation<<"                     | "<<pre_encoder_states_b<<" | "<<encoder_return_b.estimated_rotation<<"                     | "<<pre_encoder_states_r<<" | "<<encoder_return_r.estimated_rotation<<endl<<endl;
	}
	else if(encoder_return_l.time<100){
		cerr<<"  "<<encoder_return_l.time<<"       | "<<pre_encoder_states_l<<" | "<<encoder_return_l.estimated_rotation<<"                     | "<<pre_encoder_states_b<<" | "<<encoder_return_b.estimated_rotation<<"                     | "<<pre_encoder_states_r<<" | "<<encoder_return_r.estimated_rotation<<endl<<endl;
	}
	else if (encoder_return_l.time<1000){
		cerr<<"  "<<encoder_return_l.time<<"       | "<<pre_encoder_states_l<<" | "<<encoder_return_l.estimated_rotation<<"                     | "<<pre_encoder_states_b<<" | "<<encoder_return_b.estimated_rotation<<"                     | "<<pre_encoder_states_r<<" | "<<encoder_return_r.estimated_rotation<<endl<<endl;
	}
	else{
		cerr<<"  "<<encoder_return_l.time<<"      | "<<pre_encoder_states_l<<" | "<<encoder_return_l.estimated_rotation<<"                     | "<<pre_encoder_states_b<<" | "<<encoder_return_b.estimated_rotation<<"                     | "<<pre_encoder_states_r<<" | "<<encoder_return_r.estimated_rotation<<endl<<endl;
	}
}

Encoder_return current_rotation_direction(Encoder_return encoder_return, Encoder_output encoder_states){//Uses the order of channel outputs for pins two (a) and four (b) to determine wheel rotating direction and estimates the amount of rotational change
	bool now_a=encoder_return.states.a, now_b=encoder_return.states.b, pre_a=encoder_states.a, pre_b=encoder_states.b;
	if((encoder_return.time!=0) && (now_a==pre_a && now_b==pre_b)){
		//Null. Nothing has changed.
	}
	else if((((encoder_return.time!=0) && (now_a==0 && now_b==0) && (pre_a==0 && pre_b==1)) || ((encoder_return.time!=0) && (now_a==1 && now_b==1) && (pre_a==1 && pre_b==0))) || (((now_a==1 && now_b==0) && (pre_a==0 && pre_b==0)) || ((now_a==0 && now_b==1) && (pre_a==1 && pre_b==1)))){//Determines if wheel is rotating clockwise
		encoder_return.estimated_rotation+=0.0261799388;
	}
	else if((((encoder_return.time!=0) && (now_a==0 && now_b==0) && (pre_a==1 && pre_b==0)) || ((encoder_return.time!=0) && (now_a==1 && now_b==1) && (pre_a==0 && pre_b==1))) || (((now_a==0 && now_b==1) && (pre_a==0 && pre_b==0)) || ((now_a==1 && now_b==0) && (pre_a==1 && pre_b==1)))){//Determines if wheel is rotating counter-clockwise
		encoder_return.estimated_rotation-=0.0261799388;
	}
	encoder_return.time++;
	return encoder_return;
}

static Encoder_return encoder_return_b;//Back wheel encoder
static Encoder_output pre_encoder_states_b;

static Encoder_return encoder_return_l;//Left wheel encoder
static Encoder_output pre_encoder_states_l;

static Encoder_return encoder_return_r;//Right wheel encoder
static Encoder_output pre_encoder_states_r;

void get_encoder_states(Robot_inputs in){
	//cerr<<endl<<"L:"<<in.digital_io[7]<<" R:"<<in.digital_io[8]<<" B:"<<in.digital_io[9]<<endl;
	
	encoder_return_l.wheel='l';
	encoder_return_r.wheel='r';
	encoder_return_b.wheel='b';
	
	if(in.digital_io[0]==DI_0)encoder_return_l.states.a=0;//L
	else if(in.digital_io[0]==DI_1)encoder_return_l.states.a=1;
	if(in.digital_io[1]==DI_0)encoder_return_l.states.b=0;
	else if(in.digital_io[1]==DI_1)encoder_return_l.states.b=1;
	encoder_return_l=current_rotation_direction(encoder_return_l, pre_encoder_states_l);
	
	if(in.digital_io[2]==DI_0)encoder_return_r.states.a=0;//R
	else if(in.digital_io[2]==DI_1)encoder_return_r.states.a=1;
	if(in.digital_io[3]==DI_0)encoder_return_r.states.b=0;
	else if(in.digital_io[3]==DI_1)encoder_return_r.states.b=1;
	encoder_return_r=current_rotation_direction(encoder_return_r, pre_encoder_states_r);
	
	if(in.digital_io[4]==DI_0)encoder_return_b.states.a=0;//B
	else if(in.digital_io[4]==DI_1)encoder_return_b.states.a=1;
	if(in.digital_io[5]==DI_0)encoder_return_b.states.b=0;
	else if(in.digital_io[5]==DI_1)encoder_return_b.states.b=1;
	encoder_return_b=current_rotation_direction(encoder_return_b, pre_encoder_states_b);
	
	pre_encoder_states_l=encoder_return_l.states;
	pre_encoder_states_r=encoder_return_r.states;
	pre_encoder_states_b=encoder_return_b.states;
	//cerr<<endl<<endl<<pre_encoder_states_l<<endl<<endl;
	print_encoder_values(encoder_return_l, pre_encoder_states_l, encoder_return_b, pre_encoder_states_b, encoder_return_r, pre_encoder_states_r);
}