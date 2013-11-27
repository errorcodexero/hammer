#ifndef GAMEPAD_H
#define GAMEPAD_H

class Joystick_data;

struct Gamepad{
	struct Stick{
		double x,y;
		Stick();
	};
	
	Stick l,r;
	static const unsigned BUTTONS=12;
	bool button[BUTTONS];
	
	Gamepad();
};

std::ostream& operator<<(std::ostream&,Gamepad::Stick);
std::ostream& operator<<(std::ostream&,Gamepad);
Gamepad as_gamepad(Joystick_data);

#endif