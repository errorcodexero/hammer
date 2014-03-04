#ifndef SHOOTER_STATUS
#define SHOOTER_STATUS

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
}

#endif
