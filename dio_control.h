#ifndef DIO_CONTROL_H
#define DIO_CONTROL_H

#include "interface.h"
class DigitalModule;

DigitalModule *digital_module();

class DIO_control{
	typedef enum{IN,OUT,FREE} Mode;
	Mode mode;
	int channel;

	public:
	DIO_control();
	explicit DIO_control(int);
	~DIO_control();

	int set_channel(int);
	int set(Digital_out);
	Digital_in get()const;
	
	private:
	int set_mode(Mode);
	int free();
};

#endif
