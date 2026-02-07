#ifndef WIPE_HPP
#define WIPE_HPP

#include <string>
#include "dev.hpp" 

bool wipeDisk(const std::string& devicePath, WipeMethod method);

#endif
