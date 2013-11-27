#ifndef CYBORG_JOYSTICK_H
#define CYBORG_JOYSTICK_H

class Joystick_data;

struct Cyborg_joystick{
	double x,y,theta,throttle;
	static const unsigned BUTTONS=6;
	bool button[BUTTONS];
};

std::ostream& operator<<(std::ostream&,Cyborg_joystick);

Cyborg_joystick as_cyborg_joystick(Joystick_data);

#endif