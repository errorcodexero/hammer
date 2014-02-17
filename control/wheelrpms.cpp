#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include "shooter_wheels.h"

using namespace std;

struct wheelcalib{
	Shooter_wheels::Status highgoal;
	Shooter_wheels::Status lowgoal;
	Shooter_wheels::Status overtruss;
	Shooter_wheels::Status passing;
};

struct crow{
	Shooter_wheels::Status newhighgoal;
	Shooter_wheels::Status newlowgoal;
	Shooter_wheels::Status newovertruss;
	Shooter_wheels::Status newpassing;
};

ostream& operator<<(ostream& o,wheelcalib w){
	o<<"High Goal RPMs(Top)	="<<w.highgoal.top<<endl;
	o<<"High Goal RPMs(Bottom)	="<<w.highgoal.bottom<<endl;
	o<<"Low Goal RPMs(Top)	="<<w.lowgoal.top<<endl;
	o<<"Low Goal RPMs(Bottom)	="<<w.lowgoal.bottom<<endl;
	o<<"Over Truss RPMs(Top)	="<<w.overtruss.top<<endl;
	o<<"Over Truss RPMs(Bottom)	="<<w.overtruss.bottom<<endl;
	o<<"Passing RPMs(Top)	="<<w.passing.top<<endl;
	return o<<"Passing RPMs(Bottom)	="<<w.passing.bottom<<endl;
}

int whereequal(string s){
	int k=s.size();
	int i=0;
	while(i<k&&s[i]!='=')i++;
	if(i==k)return -1;			
	return i;		
}

string postequalstr(string s){
	string aft=s.substr(whereequal(s)+1,s.size());
	return aft;
}

int main(){	
	wheelcalib rpms;
	wheelcalib newcrow;
	rpms.highgoal=Shooter_wheels::Status(1200,3205);
	rpms.lowgoal=Shooter_wheels::Status(3999,1);
	rpms.overtruss=Shooter_wheels::Status(12,532);
	rpms.passing=Shooter_wheels::Status(3025,1982);
	ofstream wheelrpms;
	wheelrpms.open("wheelspeeds.txt");
	wheelrpms<<"Shooter Wheel RPMs:"<<endl<<rpms<<endl;
	wheelrpms.close();
	string s;
	ifstream f("wheelspeeds.txt");
	int c=1;
	while(f.good()){
		getline(f,s);
		if(whereequal(s)!=-1){
			if(c==2)newcrow.highgoal.top=atoi(postequalstr(s).c_str());
			if(c==3)newcrow.highgoal.bottom=atoi(postequalstr(s).c_str());
			if(c==4)newcrow.lowgoal.top=atoi(postequalstr(s).c_str());
			if(c==5)newcrow.lowgoal.bottom=atoi(postequalstr(s).c_str());
			if(c==6)newcrow.overtruss.top=atoi(postequalstr(s).c_str());
			if(c==7)newcrow.overtruss.bottom=atoi(postequalstr(s).c_str());
			if(c==8)newcrow.passing.top=atoi(postequalstr(s).c_str());
			if(c==9)newcrow.passing.bottom=atoi(postequalstr(s).c_str());
		}
		c++;	
	}
	f.close();
	cout<<newcrow;
	return 0;
}