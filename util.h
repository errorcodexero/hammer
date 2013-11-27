#ifndef UTIL_H
#define UTIL_H

#include<string>
#include "interface.h"

Pwm_output pwm_convert(double);

int write_file(std::string const& filename,std::string const& contents);
int read_file(std::string const& filename,std::string &out);//I don't like out parameters.

#endif
