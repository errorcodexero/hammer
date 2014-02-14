#include "holonomic.h"
#include<math.h>
#include<algorithm>
#include<iostream>

using namespace std;

double max3(double a,double b,double c){
	return max(max(a,b),c);
}

Drive_motors func_inner(double x, double y, double theta){	
	Drive_motors r;
	r.a=-double(1)/3* theta- double(1)/3* x -(double(1)/sqrt(3))*y;
	r.b=-double(1)/3* theta- double(1)/3* x +(double(1)/sqrt(3))*y;
	r.c=(-(double(1)/3)* theta) + ((double(2)/3)* x);
	return r;
}

Drive_motors holonomic_mix(double x,double y,double theta){
	//This function exists in order to pull the full power out of the drivetrain.  It makes some of the areas of the x/y/theta space have funny edges/non-smooth areas, but I think this is an acceptable tradeoff.
	Drive_motors r=func_inner(x,y,theta);
	const double s=sqrt(3);
	r.a*=s;
	r.b*=s;
	r.c*=s;
	const double m=max3(fabs(r.a),fabs(r.b),fabs(r.c));
	if(m>1){
		r.a/=m;
		r.b/=m;
		r.c/=m;
	}
	return r;
}

Drive_motors holonomic_mix(Pt p){
	return holonomic_mix(p.x,p.y,p.theta);
}

ostream& operator<<(ostream& o, Drive_motors d){
	return o<<"dm("<<d.a<<","<<d.b<<","<<d.c<<")";
}

#ifdef HOLONOMIC_TEST
int main(){
	
}
#endif