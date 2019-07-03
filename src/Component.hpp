#ifndef COMPONENT_H
#define COMPONENT_H

#include "fmod.hpp"
#include "fmod_errors.h"

class Component
{
public:
	virtual void update() = 0;
	virtual void entry() = 0;
	virtual void exit() = 0;
};

#endif //COMPONENT_H
