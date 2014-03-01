#ifndef DRIVER_STATION_INTERFACE_H
#define DRIVER_STATION_INTERFACE_H

#include<iosfwd>
#include<string>

struct Driver_station_output{
	static const unsigned DIGITAL_OUTPUTS=8;
	bool digital[DIGITAL_OUTPUTS];

	static const unsigned LCD_HEIGHT=6;
	std::string lcd[LCD_HEIGHT];

	Driver_station_output();
};
bool operator==(Driver_station_output,Driver_station_output);
bool operator!=(Driver_station_output,Driver_station_output);
std::ostream& operator<<(std::ostream&,Driver_station_output);

struct Driver_station_input{
	//In the traditional mode of the Cybress board there are only 4 available, but there are 8 in the 'enhanced' mode
	static const unsigned ANALOG_INPUTS=8;
	double analog[ANALOG_INPUTS];

	static const unsigned DIGITAL_INPUTS=8;
	bool digital[DIGITAL_INPUTS];

	Driver_station_input();
};
bool operator==(Driver_station_input,Driver_station_input);
bool operator!=(Driver_station_input,Driver_station_input);
std::ostream& operator<<(std::ostream&,Driver_station_input);

#endif