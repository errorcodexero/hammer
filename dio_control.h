#ifndef DIO_CONTROL_H
#define DIO_CONTROL_H

#include "util/interface.h"
class DigitalModule;

DigitalModule *digital_module();

class DIO_control{
	public:
	typedef enum{IN,OUT,FREE} Mode;
	
	private:
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
	
	friend std::ostream& operator<<(std::ostream&,DIO_control const&);
};

std::ostream& operator<<(std::ostream&,DIO_control const&);

#endif
