#include "high_level.h"
#include "common.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <cassert>

struct tuple {
	double double1;
	double double2;
	double double3;
	bool bool1;
};

using namespace std;

tuple makeTuple (double double1, double double2, double double3, bool bool1=1) {
	tuple t;
	t.double1 = double1;
	t.double2 = double2;
	t.double3 = double3;
	t.bool1 = bool1;
	return t;
}

Module_target makeModuleTarget (Angle angle, Portion power) {
	Module_target m;
	m.angle = angle;
	m.power = power;
	return m;
}

Drivebase_target makeDrivebaseTarget (Module_target module1, Module_target module2, Module_target module3) {
	Drivebase_target d;
	d.mod[0] = module1;
	d.mod[1] = module2;
	d.mod[2] = module3;
	return d;
}

Joy_in makeJoyIn (Portion x, Portion y, Portion theta) {
	Joy_in j;
	j.x = x;
	j.y = y;
	j.theta = theta;
	return j;
}

double clip(double max_magnitude,double value){
	if(value>max_magnitude) return max_magnitude;
	if(value<-max_magnitude) return -max_magnitude;
	return value;
}

double vectorMax(vector<double> v){
	assert(v.size());
	double r=v[0];
	for(unsigned int i=0;i<v.size();i++){
		r=max(r,v[i]);
	}
	return r;
}

vector<double>& operator/=(vector<double>& v,double d){
	for(unsigned int i=0;i<v.size();i++) {
		v[i]/=d;
	}
	return v;
}

//this is the math function "signum".  I thought it was in the standard library somewhere.
int signum(double d){
	if(d>0) return 1;
	if(d<0) return -1;
	return 0;
}

typedef double Portion;//in -1 to 1

ostream& operator<<(ostream& o,Module_target a){
	return o<<"Module_target("<<a.angle<<","<<a.power<<")";
}

ostream& operator<<(ostream& o,Drivebase_target const& a){
	o<<"Drivebase_target(";
	for(unsigned int i=0;i<MODULES;i++) {
		o<<a.mod[i]<<" ";
	}
	return o<<")";
}

ostream& operator<<(ostream& o,Joy_in a){
	return o<<"Joy_in("<<a.x<<" "<<a.y<<" "<<a.theta<<")";
}

Testcase makeTestcase(Joy_in in, Drivebase_target out) {
	Testcase t;
	t.in = in;
	t.out = out;
	return t;
}

ostream& operator<<(ostream& o,Testcase a){
	return o<<"Testcase("<<a.in<<","<<a.out<<")";
}

double angle_to_point(double x,double y){
	//angle is clockwise from straight ahead in radians
	//this was much easier than expected.  It even does something sort of reasonable with the singularity.
	return atan2(x,y);
}

void test_angle_to_point(){
	vector<tuple> t;
	t.push_back(makeTuple(0,1,0));
	t.push_back(makeTuple(1,0,M_PI/2));
	t.push_back(makeTuple(-1,0,-M_PI/2));
	t.push_back(makeTuple(0,-1,M_PI));
	for(unsigned int i=0;i<t.size();i++){
		double x=t[i].double1;
		double y=t[i].double2;
		double expected=t[i].double3;
		double calculated=angle_to_point(x,y);
		if(fabs(expected-calculated)>.01){
			nyi
		}
	}
}

struct Point{
	double x,y;
};

Point makePoint (double x, double y) {
	Point p;
	p.x=x;
	p.y=y;
	return p;
}

ostream& operator<<(ostream& o,Point a){
	return o<<"("<<a.x<<","<<a.y<<")";
}

Point operator+(Point a,Point b){
	Point p;
	p.x=a.x+b.x;
	p.y=a.y+b.y;
	return p;
}

Point operator-(Point a,Point b){
	Point p;
	p.x=a.x-b.x;
	p.y=a.y-b.y;
	return p;
}

Point operator-(Point a){
	Point p;
	p.x=-a.x;
	p.y=-a.y;
	return p;
}

double magnitude(Point p){
	return sqrt(p.x*p.x+p.y*p.y);
}

//given offset to the center of a circle give a tangent angle to go clockwise around it
Angle drive_wheel(Point center_of_rotation,bool cw){//cw=clockwise
	double a=-atan2(center_of_rotation.y,center_of_rotation.x);
	//auto a=atan2(center_of_rotation.y,center_of_rotation.x)+M_PI;
	if(cw) return a;
	return a+M_PI;
}

bool test_drive_wheel(){
	vector<tuple> v;
	v.push_back(makeTuple(0,1,-M_PI/2,1));
	v.push_back(makeTuple(0,-1,M_PI/2,1));
	v.push_back(makeTuple(1,0,0,1));
	v.push_back(makeTuple(-1,0,-M_PI,1));
	v.push_back(makeTuple(sqrt(2),sqrt(2),-M_PI/4,1));
	v.push_back(makeTuple(.5,sqrt(3)/2,-M_PI/3,1));
	bool pass=1;
	for(unsigned int i; i<v.size();i++){
		Point p;
		p.x=v[i].double1;
		p.y=v[i].double2;
		bool cw=v[i].bool1;
		Angle calc=normalize_angle(drive_wheel(p,cw));
		double expected=v[i].double3;
		if(fabs(calc-expected)>.01){
			cout<<p<<" "<<expected<<" "<<calc<<"\n";
			pass=0;
		}
	}
	if(!pass)nyi
	return pass;
}

double angle_to_center(Joy_in j, bool clockwise) {
	double x=angle_to_point(j.x,j.y)+M_PI/2;
	if(!clockwise) return x+M_PI;
	return x;
}

Drivebase_target high_level(Joy_in j){
	//do the interesting math here...

	//could make it so that didn't saturate till you got to the edges rather than having an "inverted circle" are that is all the same speed.
	Point jj;
	jj.x=j.x;
	jj.y=j.y;
	const double power_portion=min(1.0,max(fabs(j.theta),magnitude(jj)));
	assert(power_portion>=0 && power_portion<=1);

	//center of rotation
	const double distance_to_center=min(10000.0,fabs(1/j.theta-signum(j.theta)));
	assert(distance_to_center>=0);

	const bool clockwise=j.theta>0;
	//cout<<"cl:"<<clockwise<<"\n";
	//cout<<"J.h:"<<j.theta<<"\n";
	//cout<<"Angle to center:"<<angle_to_center<<"\n";
	double dx=sin(angle_to_center(j, clockwise))*distance_to_center;
	double dy=cos(angle_to_center(j, clockwise))*distance_to_center;
	Point to_center;
	to_center.x=dx;
	to_center.y=dy;

	//TODO: Put the real numbers in here.
	const double height=sqrt(3)/2*30;
	const double front_dist=height/3;
	vector<Point> from_center;
	from_center.push_back(makePoint(-15,front_dist));
	from_center.push_back(makePoint(15,front_dist));
	from_center.push_back(makePoint(0,-front_dist*2));

	vector<Angle> angles;
	vector<double> radius;
	for(unsigned int i=0;i<from_center.size();i++){
		Point to_wheel=to_center-from_center[i];
		//cout<<to_wheel;
		//next, calculate the angle back for this particular wheel.
		//auto ang=atan2(to_wheel.x,to_wheel.y)-M_PI/2;
		Angle ang=normalize_angle(drive_wheel(to_wheel,clockwise));
		//cout<<clockwise<<" ";
		//cout<<ang<<"\n";
		angles.push_back(ang);
		radius.push_back(magnitude(to_wheel));
	}
	//cout<<"\n";
	radius/=vectorMax(radius);

	Drivebase_target r;
	for(unsigned i=0;i<MODULES;i++){
		r.mod[i].angle = normalize_angle(angles[i]);
		r.mod[i].power=radius[i]*power_portion;
	}
	return r;
}

Point to_point(Module_target a){
	return makePoint(a.power*sin(a.angle),a.power*cos(a.angle));
}

double max_deviation(Module_target a,Module_target b){
	return max(fabs(a.angle-b.angle),fabs(a.power-b.power));
}

double max_deviation(Drivebase_target a,Drivebase_target b){
	double r=0;
	for(unsigned i=0;i<MODULES;i++){
		r=max(r,magnitude(to_point(a.mod[i])-to_point(b.mod[i])));
	}
	return r;
}

void high_level_test(){
	vector<Testcase> v;
	//0
	v.push_back(makeTestcase(makeJoyIn(0,0,0),makeDrivebaseTarget(makeModuleTarget(0,0),makeModuleTarget(0,0),makeModuleTarget(0,0))));
	//up
	v.push_back(makeTestcase(makeJoyIn(0,1,0.0001),makeDrivebaseTarget(makeModuleTarget(0,1),makeModuleTarget(0,1),makeModuleTarget(0,1))));
	v.push_back(makeTestcase(makeJoyIn(0,1,-.0001),makeDrivebaseTarget(makeModuleTarget(0,1),makeModuleTarget(0,1),makeModuleTarget(0,1))));
	//down
	v.push_back(makeTestcase(makeJoyIn(0,-1,0),makeDrivebaseTarget(makeModuleTarget(0,-1),makeModuleTarget(0,-1),makeModuleTarget(0,-1))));
	//right
	v.push_back(makeTestcase(makeJoyIn(1,0,0),makeDrivebaseTarget(makeModuleTarget(M_PI/2,1),makeModuleTarget(M_PI/2,1),makeModuleTarget(M_PI/2,1))));
	//left
	v.push_back(makeTestcase(makeJoyIn(-1,0,0.0001),makeDrivebaseTarget(makeModuleTarget(M_PI/2,-1),makeModuleTarget(M_PI/2,-1),makeModuleTarget(M_PI/2,-1))));
	v.push_back(makeTestcase(makeJoyIn(-1,0,-.0001),makeDrivebaseTarget(makeModuleTarget(M_PI/2,-1),makeModuleTarget(M_PI/2,-1),makeModuleTarget(M_PI/2,-1))));
	//up right
	v.push_back(makeTestcase(makeJoyIn(1,1,0),makeDrivebaseTarget(makeModuleTarget(M_PI/4,1),makeModuleTarget(M_PI/4,1),makeModuleTarget(M_PI/4,1))));
	//turn in place, clockwise
	v.push_back(makeTestcase(makeJoyIn(0,0,1),makeDrivebaseTarget(makeModuleTarget(M_PI/6,1),makeModuleTarget(5*M_PI/6,1),makeModuleTarget(-M_PI/2,1))));
	//counterclockwise...
	v.push_back(makeTestcase(makeJoyIn(0,0,-1),makeDrivebaseTarget(makeModuleTarget(M_PI/6,-1),makeModuleTarget(5*M_PI/6,-1),makeModuleTarget(-M_PI/2,-1))));

		//Testcase{Joy_in{1,1,1},Drivebase_target{Module_target{M_PI/4,1},Module_target{M_PI/4,1},Module_target{M_PI/4,1}}}
	bool fail=0;
	for(unsigned int i=0;i<v.size();i++){
		Drivebase_target h=high_level(v[i].in);
		double dev=max_deviation(h,v[i].out);
		if(dev>.01){
			cout<<"Deviated by "<<dev<<"\n";
			cout<<v[i]<<"\n";
			cout<<h<<"\n";
			fail=1;
		}
	}
	if(fail)nyi
}

#ifdef HIGH_LEVEL_TEST
int main(){
	test_angle_to_point();
	test_drive_wheel();
	high_level_test();
	return 0;
}
#endif
