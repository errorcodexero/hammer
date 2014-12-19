#ifndef SCOOPER_CONTROL_H 
#define SCOOPER_CONTROL_H

#include<iosfwd>

enum class Scooper_control{UP, STOP, DOWN};//Values from control
enum class Sensor_states{UP, DOWN, INBETWEEN, ERROR};//Values for sensors that determine the arm's state

std::ostream& operator<<(std::ostream&,Scooper_control);
std::ostream& operator<<(std::ostream&,Sensor_states);
Scooper_control motor_control(Scooper_control,Sensor_states);

#endif