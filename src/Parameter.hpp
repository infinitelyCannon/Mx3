#ifndef MX3_PARAMETER_H
#define MX3_PARAMETER_H

#include <string>

#include "Component.hpp"
#include "Track.hpp"

enum ParameterType
{
	VOLUME
};

struct ParameterEvent : ComponentEvent
{
	float value;
	ParameterType type;
	std::string name;
};

class Parameter : public Component
{
public:
	Parameter(Track *target, std::string name, ParameterType type);
	~Parameter();

	void entry() override;
	void update(std::vector<ComponentEvent> events) override;
	void exit() override;
	std::string getEventType() const override;

	void change(float val);

private:
	Track *mTrack;
	const std::string eventType = "PARAMETER";
	std::string mName;
	ParameterType mType;
	FMOD::ChannelGroup *mGroup = 0;
	float value = 0;
};

#endif // !MX3_PARAMETER_H
