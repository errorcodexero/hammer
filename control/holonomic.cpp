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
Drive_motors rotate_vector(double x, double y, double theta, double angle){
	double cosA = cos(angle * (3.14159 / 180.0));
	double sinA = sin(angle * (3.14159 / 180.0));
	double xOut = x * cosA - y * sinA;
	double yOut = x * sinA + y * cosA;
	
	Drive_motors r=func_inner(xOut,yOut,theta);


	return r;
}

Drive_motors holonomic_mix(double x,double y,double theta,double orientation,bool fieldRelative){
	//This function exists in order to pull the full power out of the drivetrain.  
	//It makes some of the areas of the x/y/theta space have funny edges/non-smooth areas, but I think this is an acceptable tradeoff.
	//Also rotates the vectors of the drive if the robot is in field relative mode
	if (fieldRelative){
		Drive_motors r=rotate_vector(x,y,theta,orientation);
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
	} 
	//Eric should come back and explain how this could be a separate function
	else {
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
}

Drive_motors holonomic_mix(Pt p){
	return holonomic_mix(p.x,p.y,p.theta,0,false);
}

ostream& operator<<(ostream& o, Drive_motors d){
	return o<<"dm("<<d.a<<","<<d.b<<","<<d.c<<")";
}

#ifdef HOLONOMIC_TEST
int main(){
	
}
#endif