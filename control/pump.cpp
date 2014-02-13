#include "pump.h"
#include<iostream>
#include<cassert>
#include<vector>

using namespace std;

namespace Pump{
	ostream& operator<<(ostream& o,Output a){
		#define X(name) if(a==name) return o<<""#name;
		X(ON)
		X(OFF)
		#undef X
		assert(0);
	}

	ostream& operator<<(ostream& o,Status a){
		#define X(name) if(a==name) return o<<""#name;
		X(FULL)
		X(NOT_FULL)
		#undef X
		assert(0);
	}

	Output control(Status s){
		return (s==FULL)?OFF:ON;
	}
}

#ifdef PUMP_TEST

int main(){
	static const vector<Pump::Status> STATUS_LIST{Pump::FULL,Pump::NOT_FULL};
	for(unsigned i=0;i<STATUS_LIST.size();i++){
		auto a=STATUS_LIST[i];
		cout<<a<<":"<<control(a)<<"\n";
	}
}
#endif
