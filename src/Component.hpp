#ifndef COMPONENT_H
#define COMPONENT_H

#include "fmod.hpp"

class Component
{
public:
	virtual FMOD_RESULT update() = 0;
	virtual FMOD_RESULT entry() = 0;
	virtual FMOD_RESULT exit() = 0;
};

#endif //COMPONENT_H
