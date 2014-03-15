#include "pump.h"
#include<iostream>
#include<cassert>
#include<vector>
#include "../util/util.h"

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

	vector<Pump::Status> status_list(){
		vector<Pump::Status> r;
		r|=Pump::FULL;
		r|=Pump::NOT_FULL;
		return r;
	}

	Maybe<Status> parse_status(string const& s){
		return parse_enum(status_list(),s);
	}

	Output control(Status s){
		return (s==FULL)?OFF:ON;
	}
}

#ifdef PUMP_TEST
int main(){
	for(auto a:Pump::status_list()){
		cout<<a<<":"<<control(a)<<"\n";
		assert(a==Pump::parse_status(as_string(a)));
	}
}
#endif
