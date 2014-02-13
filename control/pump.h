#ifndef PUMP_H
#define PUMP_H

//This is meant to control the pump for the pneumatic system.
//It may seem a little bit overkill, but this is here basically because there are some fancier things we might want to do later like know when we have at least 60psi.  

#include<iosfwd>

namespace Pump{
	enum Output{ON,OFF};
	std::ostream& operator<<(std::ostream&,Output);

	enum Status{FULL,NOT_FULL};
	std::ostream& operator<<(std::ostream&,Status);

	Output control(Status);
}

#endif
