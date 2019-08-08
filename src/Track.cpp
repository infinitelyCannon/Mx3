#include "Track.hpp"

Track::Track(std::string name, FMOD::ChannelGroup *group) :
	mSources(),
	mGroup(group),
	name(name)
{
}

void Track::play()
{
	for(TrackSource t : mSources)
		DeferError(t.channel->setPaused(false), "Playing channels");
}

std::vector<TrackSource> *Track::getSourceRef()
{
	return &mSources;
}

void Track::addSource(std::string file, FMOD::System *system)
{
	TrackSource source;
	FMOD::Sound *sound = 0;
	FMOD::Channel *channel = 0;
	unsigned int length = 0;

	DeferError(system->createStream(file.c_str(), FMOD_2D | FMOD_ACCURATETIME | FMOD_LOOP_NORMAL, 0, &sound), std::to_string(__LINE__ - 1));

	DeferError(sound->getLength(&length, FMOD_TIMEUNIT_MS), std::to_string(__LINE__ - 1));

	if(length > mLength)
		mLength = length;

	DeferError(system->playSound(sound, mGroup, false, &channel), std::to_string(__LINE__ - 1));

	source.sound = sound;
	source.channel = channel;

	mSources.push_back(source);
}

FMOD::ChannelGroup *Track::getChannelGroup()
{
	return mGroup;
}

void Track::setVolume(float volume)
{
	DeferError(mGroup->setVolume(volume), std::to_string(__LINE__ - 1));
}

void Track::release()
{
	for(TrackSource t : mSources)
		DeferError(t.sound->release(), "Track cleanup.");

	//if(mGroup != nullptr)
		//DeferError(mGroup->release(), "Track cleanup");

	mSources.clear();
}

void Track::setPosition(unsigned int position)
{
	for(TrackSource s : mSources)
		DeferError(s.channel->setPosition(position, FMOD_TIMEUNIT_MS), "Changing Position");
}

Track::~Track()
{
}