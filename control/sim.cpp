#include "high_level.h"

struct State{
	double x,y,theta;//of robot
	Angle module[MODULES];

	State():x(0),y(0),theta(0){
		for(unsigned i=0;i<3;i++) module[i]=0;
	}
};

ostream& operator<<(ostream& o,State){
	nyi
	return o;
}

//State next(State,double motor powers...

int main(){
	static const double TIMESTEP=.01;
	nyi
}
