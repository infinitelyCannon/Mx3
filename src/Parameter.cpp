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

void Parameter::update(std::vector<ComponentEvent> events)
{
	for(ComponentEvent e : events)
	{
		if(e.type.compare(eventType()) == 0)
		{
			float *value = static_cast<float *>(e.data1);
			if(e.target == mType && e.name.compare(mName) == 0)
			{
				ErrorDelegate(mGroup->setVolume(*value), "Param: Volume Change");
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