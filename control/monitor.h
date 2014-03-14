#ifndef MONITOR_H
#define MONITOR_H

#include<sstream>
#include<iostream>

template<typename T>
void print_diff(std::ostream& o,std::string s,T a,T b){
	if(a!=b){
		o<<s<<"From "<<a<<" to "<<b<<"\n";
	}
}

template<typename T>
void print_diff(std::ostream& o,T a,T b){
	print_diff(o,"",a,b);
}

template<typename T>
void print_diff_approx(std::ostream& o,T a,T b){
	if(!approx_equal(a,b)){
		o<<"From "<<a<<" to "<<b<<"\n";
	}
}

class Gyro_tracker;
namespace Toplevel{
	class Status;
	class Estimator;
}
class Main;
class Driver_station_output;
class Robot_outputs;
class Jaguar_input;
class Robot_inputs;

void print_diff(std::ostream&,unsigned char,unsigned char);
void print_diff(std::ostream&,Gyro_tracker const&,Gyro_tracker const&);
void print_diff(std::ostream&,Toplevel::Status const&,Toplevel::Status const&);
void print_diff(std::ostream&,Toplevel::Estimator const&,Toplevel::Estimator const&);
void print_diff(std::ostream&,Main const&,Main const&);
void print_diff(std::ostream&,Driver_station_output const&,Driver_station_output const&);
void print_diff(std::ostream&,Robot_outputs const&,Robot_outputs const&);
void print_diff(std::ostream&,std::string const&,Jaguar_input const&,Jaguar_input const&);
void print_diff(std::ostream&,Robot_inputs const&,Robot_inputs const&);

template<typename T>
struct Monitor{
	T data;

	std::string update(T t){
		std::stringstream ss;
		print_diff(ss,data,t);
		data=t;
		return ss.str();
	}
};

#endif