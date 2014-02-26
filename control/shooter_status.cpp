#include"shooter_status.h"
#include<iostream>

using namespace std;

namespace Shooter_wheels{
	Status::Status():top(0),bottom(0){}
	
	Status::Status(RPM a,RPM b):top(a),bottom(b){}
	
	ostream& operator<<(ostream& o,Status s){
		return o<<"Shooter_wheels::Status("<<s.top<<","<<s.bottom<<")";
	}
}
