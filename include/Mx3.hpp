#ifndef MX3_H
#define MX3_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "fmod.hpp"
#include "Delegate.h"
#include "Track.hpp"
#include "Component.hpp"

class Mx3
{
public:
	Mx3(int maxChannels, FMOD_INITFLAGS flags, void *externalDriverData);
	~Mx3();
	bool isPlaying();
	bool isPaused();
	void play(std::string filepath);
	void pause();
	//void resume();
	void changeTimePosition(unsigned int position);
	void setGlobalVolume(float value);
	unsigned int getLength();
	void stop();
	unsigned int getPosition();
	void update();
	void BindErrorCallback(SA::delegate<void(std::string)> func);
	void AddEvent(ComponentEvent eve);

private:
	void ErrorCheck(FMOD_RESULT result, std::string header = "");

	SA::delegate<void(std::string)> ErrorDelegate;
	bool shouldQuit = false;
	unsigned int test = 0;
	FMOD::System *mSystem;
	std::thread *mUpdate;
	std::mutex mMutex;
	FMOD::ChannelGroup *mTimeline = nullptr;
	FMOD::Channel *mChannel;
	FMOD_RESULT result;
	unsigned int version;
	//std::vector<FMOD::Sound *> mSounds;
	std::vector<Component *> mComponents;
	std::vector<Track *> mTracks;
	std::vector<ComponentEvent> mEvents;
	void stopOne(int index);
	float mVolume = 1.0f;
	unsigned int mLength = 0;
};

/*namespace Mx3
{
	class Component
	{
	public:
		virtual void update() = 0;
		virtual void entry() = 0;
		virtual void exit() = 0;
	};

	struct TrackSource
	{
		FMOD::Channel *channel = nullptr;
		FMOD::Sound *sound = nullptr;
	};

	class Track
	{
	public:
		Track(FMOD::Sound *sound, FMOD::Channel *channel, Mx3::System *system);
		~Track();

		void release();
		friend class Mx3::System;

	private:
		FMOD::ChannelGroup *mGroup;
		std::vector<TrackSource> mSources;
		Mx3::System *sysRef;
		SA::delegate<void(FMOD_RESULT, std::string)> ErrorDelegate;
	};

	
}*/

#endif //MX3_H