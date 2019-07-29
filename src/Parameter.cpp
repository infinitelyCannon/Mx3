#include "Parameter.hpp"

Parameter::Parameter(Track *target, std::string name, std::string type) :
	mTrack(target),
	mName(name),
	mType(type)
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
		if(mType.compare("VOLUME") == 0 && e.name.compare(mName) == 0)
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