#include "Parameter.hpp"

Parameter::Parameter(Track *target, std::string name) :
	mTrack(target),
	mName(name)
{

}

Parameter::~Parameter()
{

}

void Parameter::entry()
{
	mGroup = mTrack->getChannelGroup();

	ErrorDelegate(mGroup->getVolume(&value), "Parameter.cpp:" + std::to_string(__LINE__ - 1));
}

void Parameter::update(std::vector<ComponentEvent> events)
{
	for(ComponentEvent e : events)
	{
		if(e.type.compare("ParameterVolume") == 0 && e.name.compare(mName) == 0)
		{
			ErrorDelegate(mGroup->setVolume(e.Level), "Param: Change Volume.");
		}
	}
}

void Parameter::exit()
{
	return;
}

void Parameter::change(float val)
{

}