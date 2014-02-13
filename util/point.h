#ifndef POINT_H
#define POINT_H

#include<iosfwd>

struct Pt{
	double x,y;//feet
	double theta;//radians

	Pt();
	Pt(double,double,double);
};

std::ostream& operator<<(std::ostream&,Pt);
Pt operator*(Pt,double);
Pt& operator+=(Pt&,Pt);

bool approx_equal(double,double);
bool approx_equal(Pt,Pt);

typedef double Rad;//radians

Pt shift_axes_clockwise(Pt p,Rad theta);

#endif
