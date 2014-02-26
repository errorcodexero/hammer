#include "posedge_toggle.h"
#include<iostream>

Posedge_toggle::Posedge_toggle():value(0),last(1){}

void Posedge_toggle::update(bool sample){
	if(!last && sample) value=!value;
	last=sample;
}

bool Posedge_toggle::get()const{ return value; }

std::ostream& operator<<(std::ostream& o,Posedge_toggle a){
	return o<<"Posedge_toggle("<<a.last<<" "<<a.value<<")";
}
