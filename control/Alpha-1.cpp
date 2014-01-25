#include <iostream>
 using namespace std;
 
 struct ThermostatOutput{
	bool heater;
	bool ac;
};

bool thermostat(int temperature);
bool acthermostat(int temperature);

ThermostatOutput combinedthermostat(int temperature){
	ThermostatOutput u;
	u.heater=thermostat(temperature);
	u.ac=acthermostat(temperature);
	return u;
}
 
 bool thermostat(int temperature){
	return temperature<=70;
}

bool acthermostat(int temperature){
	return temperature>=80;
}
 
 
/*int main(){
	int temperature;
	ThermostatOutput x;
	temperature=70;	
	for(int i=0;i<100;i++){
		temperature=temperature*.95+.05*95;
		x=combinedthermostat(temperature);
		if(x.heater){
			temperature=temperature+4;
		}
		if (x.ac){
			temperature=temperature-2;
		}
		cout<<temperature<<"\n";
	}
}*/
