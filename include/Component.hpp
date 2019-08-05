#ifndef COMPONENT_H
#define COMPONENT_H

#include "fmod.hpp"
#include "fmod_errors.h"
#include "Delegate.h"

#include <vector>
#include <string>

enum ParameterType
{
	VOLUME
};

struct ComponentEvent
{
	std::string name;
	ParameterType target;
	std::string type;
	void *data1;
	void *data2;
	void *data3;
	void *data4;
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
