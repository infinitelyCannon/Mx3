#include "Parameter.hpp"

Parameter::Parameter(Track *target, std::string name, ParameterType type) :
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

std::string Parameter::getEventType() const
{
	return eventType;
}

void Parameter::update(std::vector<ComponentEvent> events)
{
	for(ComponentEvent e : events)
	{
		if(e.componentType.compare(eventType))
		{
			ParameterEvent &eve = static_cast<ParameterEvent&>(e);

			if(eve.type == mType && eve.name.compare(mName) == 0)
			{
				ErrorDelegate(mGroup->setVolume(eve.value), "Param: Volume Change");
			}
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