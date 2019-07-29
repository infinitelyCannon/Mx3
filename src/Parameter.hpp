#ifndef MX3_PARAMETER_H
#define MX3_PARAMETER_H

#include <string>

#include "Component.hpp"
#include "Track.hpp"

enum ParameterType
{
	VOLUME
};

class Parameter : public Component
{
public:
	Parameter(Track *target, std::string name, std::string type);
	~Parameter();

	void entry() override;
	void update(std::vector<ComponentEvent> events) override;
	void exit() override;

	void change(float val);

private:
	Track *mTrack;
	std::string mName;
	std::string mType;
	FMOD::ChannelGroup *mGroup;
	float value;
};

#endif // !MX3_PARAMETER_H
