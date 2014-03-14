#ifndef SHOOTER_STATUS_H
#define SHOOTER_STATUS_H

#include<iosfwd>

namespace Shooter_wheels{
	typedef int RPM;
	
	struct Status{
		Status();
		Status(RPM,RPM);
	
		RPM top,bottom;
	};
	bool operator==(Status,Status);
	bool operator!=(Status,Status);
	Status& operator-=(Status&,Status);
	std::ostream& operator<<(std::ostream&,Status);
	bool approx_equal(Status,Status);
}

#endif
