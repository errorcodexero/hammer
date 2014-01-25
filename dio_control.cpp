#include "dio_control.h"
#include<iostream>
#include "DigitalModule.h"

using namespace std;

//Unfortunately, there's not a good way to test all this without WPIlib since it's just an interface to WPIlib.

DigitalModule *digital_module(){
	static const int MODULE=1;
	return DigitalModule::GetInstance(MODULE);
}

DIO_control::DIO_control():mode(FREE),channel(-1){}
DIO_control::DIO_control(int i):mode(FREE),channel(i){}
DIO_control::~DIO_control(){ free(); }

int DIO_control::set_channel(int i){
	if(channel!=-1) return 1;
	channel=i;
	return 0;
}
	
int DIO_control::set(Digital_out d){
	if(channel==-1) return 4;
	if(d==DIO_INPUT) return set_mode(IN);
	{
		int r=set_mode(OUT);
		if(r) return r;
	}
	DigitalModule *dm=digital_module();
	if(!dm) return 1;
	dm->SetDIO(channel+1,d==DIO_1);
	return 0;
}

Digital_in DIO_control::get()const{
	//return DI_0;
	if(mode!=IN){
		cerr<<"Mode is actually: "<<mode<<"\n";
		return DI_OUTPUT;
	}
	DigitalModule *dm=digital_module();
	if(!dm){
		cerr<<"Error: Could not get instance of digital module.\n";
		//might want to do something more than this here.
		return DI_OUTPUT;
	}
	return dm->GetDIO(channel+1)?DI_1:DI_0;
}

int DIO_control::set_mode(Mode m){
	if(m==mode) return 0;
	
	{
		int r=free();
		if(r) return r;
	}
		
	if(m==FREE) return 0;
		
	DigitalModule *dm=digital_module();
	if(!dm) return 1;
		
	int success=dm->AllocateDIO(channel+1,m==IN);
	if(!success){
		return 2;
	}
	mode=m;
	return 0;
}
	
int DIO_control::free(){
	if(mode!=FREE){
		DigitalModule *dm=digital_module();
		if(!dm) return 1;
		dm->FreeDIO(channel+1);
		mode=FREE;
	}
	return 0;
}

ostream& operator<<(ostream& o,DIO_control::Mode a){
	switch(a){
	case DIO_control::IN: return o<<"in";
	case DIO_control::OUT: return o<<"out";
	case DIO_control::FREE: return o<<"free";
	default: return o<<"error";
	}
}

ostream& operator<<(ostream& o,DIO_control const& a){
	o<<"DIO_control(";
	o<<a.mode<<" ";
	o<<a.channel;
	return o<<")";
}
