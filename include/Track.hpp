#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>
//#include <memory>

#include "fmod.hpp"
#include "Delegate.h"

struct TrackSource
{
	FMOD::Sound *sound = nullptr;
	FMOD::Channel *channel = nullptr;
	std::string name = "";
	unsigned int length = 0; // Measured in PCM samples
	float frequency = 0;
};

class Track
{
public:
	Track(std::string name, FMOD::ChannelGroup *group);
	~Track();
	void release();
	void addSource(std::string file, FMOD::System *system);
	void play();
	std::vector<TrackSource> *getSourceRef();
	FMOD::ChannelGroup *getChannelGroup();
	void setVolume(float volume);
	void setPosition(unsigned int position);

	friend class Mx3;
	std::string name;
	unsigned int mLength = 0;

	bool operator >(const Track &other)
	{
		return this->mLength > other.mLength;
	}

private:
	FMOD::ChannelGroup *mGroup = nullptr;
	SA::delegate<void(FMOD_RESULT, std::string)> DeferError;
	std::vector<TrackSource> mSources;
};

#endif // !TRACK_H
