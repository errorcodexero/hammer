#include"shooter_status.h"
#include<iostream>
#include<cassert>

using namespace std;

namespace Shooter_wheels{
	Status::Status():top(0),bottom(0){}
	
	Status::Status(RPM a,RPM b):top(a),bottom(b){}

	bool operator==(Status a,Status b){
		return a.top==b.top && a.bottom==b.bottom;
	}

	bool operator!=(Status a,Status b){
		return !(a==b);
	}

	Status& operator-=(Status& a,Status b){
		a.top-=b.top;
		a.bottom-=b.bottom;
		return a;
	}
	
	ostream& operator<<(ostream& o,Status s){
		return o<<"Shooter_wheels::Status("<<s.top<<","<<s.bottom<<")";
	}
}
