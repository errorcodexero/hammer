#include <cmath>
#include "common.h"

Angle normalize_angle(Angle in){
	//put in -PI to +PI
	while(in<-M_PI) in+=2*M_PI;
	while(in>M_PI) in-=2*M_PI;
	return in;
}
