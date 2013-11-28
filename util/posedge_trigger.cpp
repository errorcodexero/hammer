#include "posedge_trigger.h"
#include<iostream>

using namespace std;

Posedge_trigger::Posedge_trigger():last(1){}

bool Posedge_trigger::operator()(bool b){
	bool r=!last && b;
	last=b;
	return r;
}

ostream& operator<<(ostream& o,Posedge_trigger p){
	return o<<"+e("<<p.last<<")";
}

#ifdef POSEDGE_TRIGGER_TEST
#include<cassert>

int main(){
	Posedge_trigger p;
	bool init=p(1);
	bool a=p(0);
	bool b=p(1);
	bool c=p(1);
	assert(!init);
	assert(!a);
	assert(b);
	assert(!c);
	return 0;
}
#endif
