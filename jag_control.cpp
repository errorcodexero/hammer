#include "jag_control.h"
#include "WPILib.h"
#include<iostream>
#include<cassert>

using namespace std;

ostream& operator<<(ostream& o,Jag_control::Mode m){
	#define X(name) if(m==Jag_control::name) return o<<""#name;
	X(INIT)
	X(SPEED)
	X(VOLTAGE)
	X(DISABLE)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Jag_control const& j){
	j.out(o);
	return o;
}

void Jag_control::init(int CANBusAddress){
	assert(!jaguar);//initialization is only allowed once.
	assert(mode==INIT);
	jaguar = new CANJaguar(CANBusAddress);
	assert(jaguar);
	jaguar->DisableControl();
	jaguar->SetSafetyEnabled(0);
	mode=DISABLE;
/*	jaguar->SetSafetyEnabled(false);
	jaguar->Set(0.0, SYNC_GROUP);
	jaguar->ChangeControlMode(CANJaguar::kSpeed);
	controlSpeed = true;
	jaguar->ConfigEncoderCodesPerRev(1);*/
}

Jag_control::Jag_control():jaguar(NULL),mode(INIT){}

Jag_control::Jag_control(int CANBusAddress):jaguar(NULL),mode(INIT){
	init(CANBusAddress);
}

Jag_control::~Jag_control(){
	delete jaguar;
}
	
void Jag_control::set(Jaguar_output a,bool enable){
	assert(mode!=INIT);
	if(!enable){
		if(mode==DISABLE){
			return;
		}else{
			jaguar->Set(0,SYNC_GROUP);
			//update sync group here?
			jaguar->DisableControl();
			jaguar->SetSafetyEnabled(0);
			mode=DISABLE;
			return;
		}
	}
	const float kP = 1.000;
	const float kI = 0.005;
	const float kD = 0.000;
	if(a.controlSpeed){
		if(mode!=SPEED){
			jaguar->ChangeControlMode(CANJaguar::kSpeed);
			jaguar->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
			jaguar->ConfigEncoderCodesPerRev(1);
			jaguar->SetPID(kP,kI,kD);
			jaguar->EnableControl();
			jaguar->SetExpiration(2.0);
			jaguar->Set(a.speed,SYNC_GROUP);
			CANJaguar::UpdateSyncGroup(SYNC_GROUP);
			jaguar->SetSafetyEnabled(true);
			mode=SPEED;
		}else{
			jaguar->Set(a.speed,SYNC_GROUP);
			CANJaguar::UpdateSyncGroup(SYNC_GROUP);
		}
	}else{
		if(mode!=VOLTAGE){
			jaguar->ChangeControlMode(CANJaguar::kPercentVbus);
			jaguar->EnableControl();
			jaguar->SetExpiration(2.0);
			jaguar->Set(a.voltage,SYNC_GROUP);
			CANJaguar::UpdateSyncGroup(SYNC_GROUP);
			jaguar->SetSafetyEnabled(true);
			mode=VOLTAGE;
		}else{
			jaguar->Set(a.voltage,SYNC_GROUP);
			CANJaguar::UpdateSyncGroup(SYNC_GROUP);
		}
	}
}

Jaguar_input Jag_control::get()const{
	Jaguar_input x;
	x.speed = jaguar -> GetSpeed();
	return x;
}

void Jag_control::out(ostream& o)const{
	o<<"Jag_control(";
	o<<"init:"<<!!jaguar;
	o<<" "<<mode;
	o<<")";
}