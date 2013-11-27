#include "util.h"
#include<fstream>

using namespace std;

//probably want to decouple the deadzones from the rest of this.
Pwm_output pwm_convert(double p){
//going to emulate this so that we don't need to recalibrate stuff.
//	    void SetBounds(INT32 max, INT32 deadbandMax, INT32 center, INT32 deadbandMin, INT32 min);
//    SetBounds(206, 131, 128, 125, 56);
	static const unsigned char MAX=206,CENTER=128,MIN=56;
	double range=MAX-MIN;
	double range_multiplier=range/2;
	double out=range_multiplier*p+CENTER;
	if(out>125 && out<131) return CENTER;
	if(out>MAX) return MAX;
	if(out<MIN) return MIN;
	return (unsigned char)out;
}

int write_file(string const& filename,string const& contents){
	ofstream file(filename.c_str());
	if(!file.is_open()) return 1;
	file<<contents;
	if(!file.good()) return 2;
	return 0;
}

int read_file(string const& filename,string &out){
	//slurp the whole file in
	ifstream file(filename.c_str());
	if(!file.is_open()) return 1;
	
	//the syntax of this next line is rather unfortunate.  If the extra parenthesis are removed it suddenly looks like a function declaration.
	string s((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	
	//could do some error checking here...
	out=s;
	return 0;
}

#ifdef UTIL_TEST

#include<cassert>
#include<iostream>

int main(){
	string data="line1\nline2";
	string filename="f1.tmp";
	{
		int r=write_file(filename,data);
		assert(!r);
	}
	string out;
	{
		int r=read_file(filename,out);
		assert(!r);
	}
	assert(out==data);
}
#endif
