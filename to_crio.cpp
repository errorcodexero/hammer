#include "WPILib.h"
#include "DigitalModule.h"
#include "control/main.h"
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

int read_joysticks(Robot_inputs &r){
	DriverStation *ds=DriverStation::GetInstance();
	if(!ds) return 4;
	for(unsigned i=0;i<r.JOYSTICKS;i++){
		r.joystick[i]=read_joystick(*ds,i);
	}
	return 0;
}

int read_analog(Robot_inputs& r){
	AnalogModule *am=AnalogModule::GetInstance(1);
	if(am){
		for(unsigned i=0;i<r.ANALOG_INPUTS;i++){
			r.analog[i]=am->GetAverageVoltage(i+1);
		}
		return 0;
	}else{
		for(unsigned i=0;i<r.ANALOG_INPUTS;i++){
			//could make this NAN.
			r.analog[i]=0;
		}
		return 64;
	}
}

//it might make sense to put this in the Robot_inputs structure.  
Volt battery_voltage(){
	AnalogModule *am=AnalogModule::GetInstance(DriverStation::kBatteryModuleNumber);
	if(!am){
		return 18; //this should look surprising
		//but maybe should actually return NAN.
	}
	float f=am->GetAverageVoltage(DriverStation::kBatteryChannel);
	return f * (1680.0 / 1000.0);//copied from WPIlib's DriverStation.cpp
}

pair<Robot_inputs,int> read(Robot_mode robot_mode){
	int error_code=0;
	Robot_inputs r;
	r.robot_mode=robot_mode;
	r.now=Timer::GetFPGATimestamp();
	error_code|=read_joysticks(r);
	error_code|=read_analog(r);
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
class To_crio
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

	int set_outputs(Robot_outputs out){
		int error_code=0;
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
		return error_code;
	}
	
	void run(Robot_inputs in){
		Robot_outputs out=main(in);
		set_outputs(out);
	}
	
	void run(Robot_mode mode){
		/*cerr<<"Going to set LCD\n";
		cerr.flush();
		
		//string s="hell owrld\n";
		static const unsigned SIZE=USER_DS_LCD_DATA_SIZE;
		char s[SIZE];
		memset(s,' ',SIZE);
		s[0]='a';
		s[1]='b';
		//setUserDsLcdData(s,SIZE,0);
		DriverStationLCD *a=DriverStationLCD::GetInstance();
		if(!a){
			cerr<<"LCD null\n";
		}else{
			a->PrintfLine(DriverStationLCD::kUser_Line1,"d");
			a->UpdateLCD();
			//a->PrintLine("hello");
		}
		cerr<<"Done with LCD\n";
		cerr.flush();*/
		

		pair<Robot_inputs,int> in1=read(mode);
		Robot_inputs in=in1.first;
		error_code|=in1.second;
		run(in);
	}
};

template<typename USER_CODE>
class Robot_adapter: public SimpleRobot{
	To_crio<USER_CODE> u;
	
	void Autonomous(void)
	{
		while(IsAutonomous()){
			//might need a loop here
			Robot_mode mode;
			mode.autonomous=1;
			mode.enabled=IsEnabled();
			u.run(mode);
		}
	}

	void OperatorControl(void)
	{
		//should see what happens when we get rid of this loop.  
		while (IsOperatorControl())
		{
			Robot_mode r;
			r.enabled=IsEnabled();
			u.run(r);
			
			//should see what happpens when this wait is removed.
			Wait(0.005);				// wait for a motor update time
		}
	}
	
	//Runs during test mode
	void Test(){
		while(IsTest()){
			Robot_mode r;
			r.enabled=IsEnabled();
			u.run(r);
		}
	}
	
	void Disabled(){
		while(IsDisabled()){
			Robot_mode r;
			r.autonomous=IsAutonomous();
			u.run(r);
		}
	}
};

void initialize(FRCCommonControlData &a){
    a.packetIndex = 0;
    a.control = 0;

    //joystick axis values to neutral
    for(unsigned i=0;i<Joystick_data::AXES;i++){
    	a.stick0Axes[i]=0;
    	a.stick1Axes[i]=0;
    	a.stick2Axes[i]=0;
    	a.stick3Axes[i]=0;
    }
	
    //buttons initially off
    a.stick0Buttons = 0;
    a.stick1Buttons = 0;
    a.stick2Buttons = 0;
    a.stick3Buttons = 0;

    //analog and digital data from the drivers' station
    a.analog1 = 0;
    a.analog2 = 0;
    a.analog3 = 0;
    a.analog4 = 0;
    a.dsDigitalIn = 0;
}

struct Com_data{
	FRCCommonControlData data;
	
	Com_data(){
		initialize(data);
	}
	
	bool button(unsigned joystick,unsigned number)const{
		UINT16 button_data;
		switch(joystick){
		case 0:
			button_data=data.stick0Buttons;
			break;
		case 1:
			button_data=data.stick1Buttons;
			break;
		case 2:
			button_data=data.stick2Buttons;
			break;
		case 3:
			button_data=data.stick3Buttons;
			break;
		default:
			//it will be nice when we can use exception.
			cerr<<"Error: Bad joystick number.\n";
			return 0;
		}
		if(number>=Joystick_data::BUTTONS){
			//it will be nice when we can use exceptions
			cerr<<"Error: Button number out of range";
			return 0;
		}
		return !!(button_data&(1<<number));
	}
	
	INT8 axis(unsigned joystick,unsigned number)const{
		if(number>=Joystick_data::AXES){
			//wil be nice when we can use exceptions
			cerr<<"Error: Axis number too high.\n";
			return 0;
		}
		switch(joystick){
		case 0: return data.stick0Axes[number];
		case 1: return data.stick1Axes[number];
		case 2: return data.stick2Axes[number];
		case 3: return data.stick3Axes[number];
		default:
			cerr<<"Joystick number too high.\n";
			return 0;
		}
	}
	
	UINT16 analog(unsigned i)const{
		switch(i){
		case 0: return data.analog1;
		case 1: return data.analog2;
		case 2: return data.analog3;
		case 3: return data.analog4;
		default:
			//exceptions would be good here.
			cerr<<"Drivers' station analog input number range is 0-3.\n";
			return 0;
		}
	}
	
	Robot_mode robot_mode()const{
		Robot_mode r;
		r.autonomous=data.autonomous;
		r.enabled=data.enabled;
		return r;
	}
};

Joystick_data read_joystick(Com_data const& cd,int port){
	Joystick_data r;
	for(unsigned i=0;i<r.AXES;i++){
		//TODO: Check whether the inputs can come actually come back as -128, and if not then just divide by 127.
		r.axis[i]=(double)cd.axis(port,i)/128;
	}
	for(unsigned i=0;i<r.BUTTONS;i++) r.button[i]=cd.button(port,i);
	return r;
}

void read_joysticks(Com_data const& cd,Robot_inputs &r){
	for(unsigned i=0;i<Robot_inputs::JOYSTICKS;i++){
		r.joystick[i]=read_joystick(cd,i);
	}
}

pair<Robot_inputs,int> read(Com_data const& cd){
	Robot_inputs r;
	r.robot_mode=cd.robot_mode();
	r.now=Timer::GetFPGATimestamp();
	read_joysticks(cd,r);
	int error_code=read_analog(r);
	return make_pair(r,error_code);
}

void report(Com_data const& cd){
	if(!cd.data.enabled){
		FRC_NetworkCommunication_observeUserProgramDisabled();
		return;
	}
	if(cd.data.autonomous){
		FRC_NetworkCommunication_observeUserProgramAutonomous();
		return;
	}
	FRC_NetworkCommunication_observeUserProgramTeleop();
}

int low_level(...){
	//This is not actually required.
	//FRC_NetworkCommunication_observeUserProgramStarting();
	cerr<<"in main task.\n";
	To_crio<Main> m;
	Com_data cd;
	
	//int update_number=0;
	while(1){
		setStatusData(
			battery_voltage(),//GetBatteryVoltage(),
			0,//UINT8//m_digitalOut,
			0,//update_number++,//m_updateNumber,
			NULL,//userStatusDataHigh, 
			0,//userStatusDataHighSize, 
			NULL,//userStatusDataLow, 
			0,//userStatusDataLowSize, 
			WAIT_FOREVER
		);
		getCommonControlData(&cd.data,40);//I think this means there's a 40 ms timeout.
		
		//This is not actually required.
		//report(cd);
		
		pair<Robot_inputs,int> p=read(cd);
		if(p.second){
			cerr<<"Error code "<<p.second<<"\n";
		}
		m.run(p.first);
	}
	return 0;
}

extern "C"{
	INT32 FRC_UserProgram_StartupLibraryInit(){
		cerr<<"started the low level thing!\n";
		int stack_size=64000;//copied from Task
		int priority=Task::kDefaultPriority;
		INT32 task_id=taskSpawn("tDemo",priority,VX_FP_TASK,stack_size,low_level,0,1,2,3,4,5,6,7,8,9);
		cerr<<"Task id="<<task_id<<" error="<<ERROR<<"\r\n";
		cerr<<"end starter\n";
		return 0;
	}
}

//START_ROBOT_CLASS(Robot_adapter<Main>);
