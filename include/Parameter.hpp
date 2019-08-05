#ifndef MX3_PARAMETER_H
#define MX3_PARAMETER_H

#include <string>

#include "Component.hpp"
#include "Track.hpp"

class Parameter : public Component
{
public:
	Parameter(Track *target, std::string name, ParameterType type);
	~Parameter();

	void entry() override;
	void update(std::vector<ComponentEvent> events) override;
	void exit() override;
	static std::string eventType()
	{
		return "PARAMETER";
	}

	void change(float val);

private:
	Track *mTrack;
	std::string mName;
	ParameterType mType;
	FMOD::ChannelGroup *mGroup = 0;
	float value = 0;
};

#endif // !MX3_PARAMETER_H
