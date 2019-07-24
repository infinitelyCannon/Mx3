#ifndef MX3_PARAMETER_H
#define MX3_PARAMETER_H

#include <string>

#include "Component.hpp"
#include "Track.hpp"

class Parameter : public Component
{
public:
	Parameter(Track *target, std::string name);
	~Parameter();

	void entry() override;
	void update(std::vector<ComponentEvent> events) override;
	void exit() override;

	void change(float val);

private:
	Track *mTrack;
	std::string mName;
	FMOD::ChannelGroup *mGroup;
	float value;
};

#endif // !MX3_PARAMETER_H
