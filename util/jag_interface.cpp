#include "jag_interface.h"
#include<iostream>
#include<cmath>
#include "util.h"

using namespace std;

Jaguar_output::Jaguar_output(){
	speed = 0;
	voltage = 0;
	controlSpeed = false;
}

Jaguar_output Jaguar_output::speedOut(double a){
	Jaguar_output j;
	j.controlSpeed = true;
	j.speed = a;
	return j;
}

Jaguar_output Jaguar_output::voltageOut(double a){
	Jaguar_output j;
	j.controlSpeed = false;
	j.voltage = a;
	return j;
}

Maybe<Jaguar_output> Jaguar_output::parse(std::string const& s){
	Jaguar_output r;
	vector<string> v=split(inside_parens(s),'=');
	if(v.size()!=2) return Maybe<Jaguar_output>();
	if(v[0]=="speed"){
		r=speedOut(atof(v[1]));
	}else if(v[0]=="voltage"){
		r=voltageOut(atof(v[1]));
	}else{
		return Maybe<Jaguar_output>();
	}
	return Maybe<Jaguar_output>(r);
}

bool operator==(Jaguar_output a,Jaguar_output b){
	return a.speed==b.speed && a.voltage==b.voltage && a.controlSpeed==b.controlSpeed;
}

bool operator!=(Jaguar_output a,Jaguar_output b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Jaguar_output a){
	o<<"Jaguar_output(";
	if(a.controlSpeed){
		o<<"speed="<<a.speed<<")";
	} else{
		o<<"voltage="<<a.voltage<<")";
	}
	return o;
}

Jaguar_input::Jaguar_input():speed(0),current(0){}

Maybe<Jaguar_input> Jaguar_input::parse(string const& s){
	cout<<"s="<<s<<"\n";
	vector<string> v=split(inside_parens(s));
	if(v.size()!=2) return Maybe<Jaguar_input>();
	cout<<v<<"\n";
	Jaguar_input r;
	{
		vector<string> a=split(v[0],':');
		if(a.size()!=2) return Maybe<Jaguar_input>();
		r.speed=atof(a[1]);
	}
	{
		vector<string> a=split(v[1],':');
		if(a.size()!=2) return Maybe<Jaguar_input>();
		r.current=atof(a[1]);
	}
	return Maybe<Jaguar_input>(r);
}

bool operator==(Jaguar_input a,Jaguar_input b){
	return a.speed==b.speed && a.current==b.current;
}

bool operator!=(Jaguar_input a,Jaguar_input b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Jaguar_input a){
	o<<"Jaguar_input(";
	o<<"speed:"<<a.speed;
	o<<" current:"<<a.current;
	return o<<")";
}

bool approx_equal(Jaguar_input a,Jaguar_input b){
	return a.current==b.current && fabs(a.speed-b.speed)<140;//this threshold is totally arbitrary.
}

#ifdef JAG_INTERFACE_TEST
#include "util.h"

int main(){
	Jaguar_input a;
	cout<<a<<"\n";
	Jaguar_output b;
	cout<<b<<"\n";
	a.current=3;
	a.speed=2.33;
	assert(a==Jaguar_input::parse(as_string(a)));
	b=Jaguar_output::speedOut(3);
	assert(b==Jaguar_output::parse(as_string(b)));
}
#endif
