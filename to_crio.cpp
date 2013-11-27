#include "WPILib.h"
#include "DigitalModule.h"
#include "interface.h"
#include "main.h"
#include "dio_control.h"

Joystick_data read_joystick(DriverStation& ds,int port){
	//I don't know what the DriverStation does when port is out of range.
	Joystick_data r;
	for(unsigned i=0;i<Joystick_data::AXES;i++){
		r.axis[i]=ds.GetStickAxis(port+1,i+1);
	}
	
	uint16_t buttons=ds.GetStickButtons(port+1);
	for(unsigned i=0;i<Joystick_data::BUTTONS;i++){
		if(buttons&(1<<i)) r.button[i]=1;
	}
	return r;
}

pair<Robot_inputs,int> read(Robot_mode robot_mode){
	int error_code=0;
	Robot_inputs r;
	r.robot_mode=robot_mode;
	r.now=Timer::GetFPGATimestamp();
	{
		DriverStation *ds=DriverStation::GetInstance();
		if(ds){
			for(unsigned i=0;i<MAX_JOYSTICKS;i++){
				r.joystick[i]=read_joystick(*ds,i);
			}
		}else{
			error_code|=4;
		}
	}
	{
		AnalogModule *am=AnalogModule::GetInstance(1);
		if(am){
			for(unsigned i=0;i<r.ANALOG_INPUTS;i++){
				r.analog[i]=am->GetAverageVoltage(i+1);
			}
		}else{
			for(unsigned i=0;i<r.ANALOG_INPUTS;i++){
				//could make this NAN.
				r.analog[i]=0;
			}
			error_code|=64;
		}
	}
	return make_pair(r,error_code);
}

int find_solenoid_module(){
	for(unsigned i=0;i<8;i++){
		if(nLoadOut::getModulePresence(nLoadOut::kModuleType_Solenoid,i)){
			return i;
		}
	}
	return -1;
}

int set_pwm(unsigned i,Pwm_output p){
	if(i>=Robot_outputs::PWMS) return 1;
	DigitalModule *dm=digital_module();
	if(!dm) return 2;
	dm->SetPWM(i+1,p);
	return 0;
}

int set_relay(unsigned i,Relay_output v){
	if(i>=Robot_outputs::RELAYS) return 1;
	DigitalModule *dm=digital_module();
	if(!dm) return 2;
	dm->SetRelayForward(i+1,v==RELAY_10 || v==RELAY_11);
	dm->SetRelayReverse(i+1,v==RELAY_01 || v==RELAY_11);
	return 0;
}

/*int demo(int a0,int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9){
	cerr<<"In demo\n";
}*/
int demo(...){
	cerr<<"In demo\n";
	return 0;
}

template<typename USER_CODE>
class To_crio : public SimpleRobot
{
	Solenoid *solenoid[Robot_outputs::SOLENOIDS];
	DIO_control digital_io[Robot_outputs::DIGITAL_IOS];
	
	int error_code;
	USER_CODE main;
	int skipped;
	
public:
	To_crio():error_code(0),skipped(0)
	{
		int solenoid_module=find_solenoid_module();
		for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
			if(solenoid_module==-1){
				solenoid[i]=NULL;
			}else{
				solenoid[i]=new Solenoid(solenoid_module-1,i+1);
				if(!solenoid[i]) error_code|=8;
			}
		}
		
		for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
			digital_io[i].set_channel(i);
		}
	}

	int set_solenoid(unsigned i,Solenoid_output v){
		if(i>=Robot_outputs::SOLENOIDS) return 1;
		if(!solenoid[i]) return 2;
		solenoid[i]->Set(v);
		return 0;
	}
	
	void run(Robot_mode mode){
		pair<Robot_inputs,int> in1=read(mode);
		Robot_inputs in=in1.first;
		error_code|=in1.second;
		Robot_outputs out=main(in);
		for(unsigned i=0;i<Robot_outputs::PWMS;i++){
			int r=set_pwm(i,out.pwm[i]);
			if(r) error_code|=2;
		}
		for(unsigned i=0;i<Robot_outputs::SOLENOIDS;i++){
			int r=set_solenoid(i,out.solenoid[i]);
			if(r) error_code|=16;
		}
		for(unsigned i=0;i<Robot_outputs::RELAYS;i++){
			int r=set_relay(i,out.relay[i]);
			if(r) error_code|=32;
		}
		for(unsigned i=0;i<Robot_outputs::DIGITAL_IOS;i++){
			digital_io[i].set(out.digital_io[i]);
		}
		
		//rate limiting the output  
		if(skipped==0){
			//cerr<<"Ran "<<mode<<"\r\n";
			/*cerr<<in<<"\r\n";
			cerr<<main<<"\r\n";
			cerr<<"errorcode="<<error_code<<"\n";
			cerr.flush();*/
			//cerr<<out<<"\r\n";
			/*cerr<<"Going to start task\r\n";
			int priority=Task::kDefaultPriority;
			int stack_size=64000;//copied from Task
			INT32 task_id=taskSpawn("tDemo",priority,VX_FP_TASK,stack_size,demo,0,1,2,3,4,5,6,7,8,9);
			cerr<<"Task id="<<task_id<<" error="<<ERROR<<"\r\n";*/
		}
		
		skipped=(skipped+1)%100;
	}

	void Autonomous(void)
	{
		while(IsAutonomous()){
			//might need a loop here
			Robot_mode mode;
			mode.autonomous=1;
			mode.enabled=IsEnabled();
			run(mode);
		}
	}

	void OperatorControl(void)
	{
		//should see what happens when we get rid of this loop.  
		while (IsOperatorControl())
		{
			Robot_mode r;
			r.enabled=IsEnabled();
			run(r);
			
			//should see what happpens when this wait is removed.
			Wait(0.005);				// wait for a motor update time
		}
	}
	
	//Runs during test mode
	void Test(){
		while(IsTest()){
			Robot_mode r;
			r.enabled=IsEnabled();
			run(r);
		}
	}
	
	void Disabled(){
		while(IsDisabled()){
			Robot_mode r;
			r.autonomous=IsAutonomous();
			run(r);
		}
	}
};

START_ROBOT_CLASS(To_crio<Main>);
