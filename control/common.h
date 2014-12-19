#ifndef COMMON_H
#define COMMON_H

#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define nyi { std::cerr<<"NYI "<<__FILE__<<" "<<__LINE__<<"\n"; exit(44); }

typedef double Angle;//radians
typedef double Portion;//in -1 to 1

Angle normalize_angle(Angle);

#endif
