#ifndef COMPONENT_H
#define COMPONENT_H

#include "fmod.hpp"
#include "fmod_errors.h"
#include "Delegate.h"

struct ComponentEvent
{
	int Start;
	int End;
	float Level;
	std::string type;
	std::string name;
};

class Component
{
public:
	virtual void update(std::vector<ComponentEvent> events) = 0;
	virtual void entry() = 0;
	virtual void exit() = 0;
	void setDelegate(SA::delegate<void(FMOD_RESULT, std::string)> func)
	{
		ErrorDelegate = func;
	}

protected:
	SA::delegate<void(FMOD_RESULT, std::string)> ErrorDelegate;
};

#endif //COMPONENT_H
