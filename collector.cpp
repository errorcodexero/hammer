#include <iostream>
#include <stdio.h>

using namespace std;

enum Collector_Mode{ON, OFF, REVERSE};
Collector_Mode collecting(Collector_Mode state,bool press_on, bool press_reverse, bool release_reverse) {
	if (press_on && state == OFF) {
		return ON;
	}
	if (press_on && state == ON) {
		return OFF;
	}
	if ((press_reverse && state == OFF) || (press_reverse && state == ON)) {
		return REVERSE;
	}
	if (release_reverse && state == REVERSE){
		return OFF;
	}
	if (state == OFF) {
		return OFF;
	}
	if (state == ON) {
		return ON;
	}
	if (state == REVERSE){
		return REVERSE;
	}
	throw 5;
}


ostream& operator << (ostream& o, Collector_Mode c) {
	if (c == ON) {
		o << "ON\n";
	}
	else if (c == OFF) {
		o << "OFF\n";
	}
	if (c == REVERSE) {
		o << "REVERSE\n";
	}
	
	
	return o;
}

int main() {
	for(int j=0;j<2;j++){
		for(int k=0;k<2;k++){
			for(int i=0;i<2;i++){
				cout<< "j;"<<j<<k<<i<<"\n";
				cout.flush();
				cout<<"ON:"<< collecting(ON,j,k,i);
				cout.flush();
				cout<<"REVERSE:"<< collecting(REVERSE,j,k,i);
				cout.flush();
				cout<<"OFF:"<< collecting(OFF,j,k,i);
				cout.flush();
			}
		}
	}
	return 0;
}