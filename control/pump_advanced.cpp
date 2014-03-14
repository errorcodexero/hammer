#include "pump_advanced.h"

/*
Important note to anyone reading this code:
Use the pressure estimates to control the pump at your own risk.  As far as I can tell, it follows the letter of the law as far as the 2014 FIRST regulations, but it _will not_ pass inspection.  This is because the inspectors check to see that the pump is following the sensor rather that it stops at the right level.  
*/

using namespace std;

double square(double x){ return x*x; }
static const double PI=3.14159;

typedef double Inch;
typedef double In3;

In3 cylinder_volume(Inch bore,Inch stroke){
	//ignoring the size of the shaft
	return PI*square(bore/2)*stroke;
}

//use constexpr when available
In3 injector_volume(){
	return 2*cylinder_volume(.75,8); //sizes are total guesses
}

In3 injector_arms_volume(){
	return cylinder_volume(.5,3);
}

In3 ejector_volume(){
	return 2*cylinder_volume(.5,3);
}

In3 collector_tilt_volume(){
	return 2*cylinder_volume(2,4);
}

In3 tank_volume(){
	//am-2477 per the AndyMark website, (.5 L); we should double check that this is what we're using.
	static const In3 SINGLE_TANK=30.5;

	return SINGLE_TANK*3;
}

namespace Pump_advanced{
	static const PSI SWITCH_CHANGE_LOW=85;
	static const PSI SWITCH_CHANGE_HIGH=110;

	struct Pressure_switch_sim{
		Pump::Status est;

		Pressure_switch_sim():est(Pump::NOT_FULL){}

		void update(PSI psi){
			//If these are >/< rather than >=/<= then feeding the sim back into itself will cause it to be confused.  This should not matter on the robot.
			if(psi>=SWITCH_CHANGE_HIGH) est=Pump::FULL;
			if(psi<=SWITCH_CHANGE_LOW) est=Pump::NOT_FULL;
		}

		Pump::Status estimate()const{ return est; }
	};
	ostream& operator<<(ostream& o,Pressure_switch_sim a){
		return o<<"Pressure_switch_sim("<<a.est<<")";
	}

	Estimator::Estimator():last_time(-1),last_reading(Pump::NOT_FULL),psi(0){} //or could initially start at full.
	
	void Estimator::update(Time now,Collector_tilt::Output collector_tilt_out,Injector::Output injector_out,Injector_arms::Output injector_arms_out,Ejector::Output ejector_out,Pump::Output pump_out,Pump::Status pressure_switch){
		if(last_time==-1) last_time=now;
		//cout<<"ch:"<<collector_tilt_out<<collector_tilt<<"\n";

		In3 air_use=0;
		if(collector_tilt_out!=collector_tilt){
			air_use+=collector_tilt_volume();
			cout<<"c\n";
			collector_tilt=collector_tilt_out;
			cout<<"d\n";
		}
		if(injector_out!=injector){
			air_use+=injector_volume();
			injector=injector_out;
		}
		if(injector_arms_out!=injector_arms){
			air_use+=injector_arms_volume();
			injector_arms=injector_arms_out;
		}
		if(ejector_out!=ejector){
			air_use+=ejector_volume();
			ejector=ejector_out;
		}
		//could add something here to account for leaks in the system.

		psi-=60*(air_use/tank_volume());
		if(psi<0) psi=0;

		//cout<<"m="<<psi<<"\n";

		Time elapsed=now-last_time;
		last_time=now;

		//cout<<"elapsed:"<<elapsed<<"\n";

		static const double PUMP_SPEED=1.8;//psi per second.  This is wrong because the pump goes slower a the pressure gets higher.
		if(pump_out==Pump::ON){
			psi+=elapsed*PUMP_SPEED;
		}

		//cout<<"n="<<psi<<"\n";

		//pressure releif valve
		if(psi>125) psi=125;

		if(pressure_switch==Pump::NOT_FULL){
			if(last_reading==Pump::FULL){
				psi=SWITCH_CHANGE_LOW;
			}
			if(psi>SWITCH_CHANGE_HIGH) psi=SWITCH_CHANGE_HIGH;
		}else{
			if(last_reading==Pump::NOT_FULL){
				psi=SWITCH_CHANGE_HIGH;
			}
			if(psi<SWITCH_CHANGE_LOW) psi=SWITCH_CHANGE_LOW;
		}
		last_reading=pressure_switch;
	}

	PSI Estimator::estimate()const{ return psi; }

	ostream& operator<<(ostream& o,Estimator a){
		o<<"Pump_advanced::Estimator(";
		#define X(name) o<<""#name<<":"<<a.name<<" ";
		X(last_time)
		X(last_reading)
		X(psi)
		X(collector_tilt)
		X(injector)
		X(injector_arms)
		X(ejector)
		#undef X
		return o<<")";
	}

	Pump::Output control(PSI psi){
		return (psi>=120)?Pump::OFF:Pump::ON;
	}
}

#ifdef PUMP_ADVANCED_TEST

void est_test(){
	using namespace Pump_advanced;
	Estimator est;
	Pressure_switch_sim sensor;
	cout<<est<<"\n";
	for(Time t=0;t<120;t++){
		Pump::Output out=control(est.estimate());
		sensor.update(est.estimate());
		est.update(t,Collector_tilt::OUTPUT_UP,Injector::OUTPUT_DOWN,Injector_arms::OUTPUT_CLOSE,Ejector::OUTPUT_DOWN,out,sensor.estimate());
		cout<<est.estimate()<<" "<<out<<" "<<sensor.estimate()<<"\n";
	}
}

void switch_sim_test(){
	using namespace Pump_advanced;
	Pressure_switch_sim s;
	for(PSI p=0;p<130;p++){
		s.update(p);
		cout<<p<<":"<<s.estimate()<<"\n";
	}
	for(PSI p=130;p>70;p--){
		s.update(p);
		cout<<p<<":"<<s.estimate()<<"\n";
	}
}

void print_volumes(){
	#define X(name) cout<<""#name<<":"<<name()<<"\n";
	X(injector_volume)
	X(injector_arms_volume)
	X(ejector_volume)
	X(collector_tilt_volume)
	X(tank_volume)
	#undef X
}

int main(){
	//switch_sim_test();
	est_test();
	//print_volumes();
}
#endif
