#include "Mx3.hpp"
#include "fmod_errors.h"

#define ERRCHECK(x) ErrorCheck(x, __FILE__, __LINE__)

ErrorCallback Mx3::mErrorCallback = nullptr;

/*
* TODO: Find a way to have the library be imported without the user having to use/include FMOD headers
* (It'll help with glue implementations)
*/
Mx3::Mx3(int maxchannels, FMOD_INITFLAGS flags, void* driverdata):
	mSystem(nullptr)
{
	ERRCHECK(FMOD::System_Create(&mSystem, FMOD_VERSION));

	ERRCHECK(mSystem->init(maxchannels, flags, driverdata));

	mTracks["Mx3_Main"] = nullptr;

	ERRCHECK(mSystem->createChannelGroup("Mx3_Main", &mTracks["Mx3_Main"]));
}

void Mx3::AddToQueue(std::string file)
{
	mQueue.push_back(file);

	if(mSounds.count(file) == 0)
		mSounds.insert({file, nullptr});
}

void Mx3::AddToQueue(std::vector<std::string> &files)
{
	for(std::string file : files)
		AddToQueue(file);
}

void Mx3::ClearQueue()
{
	// Stop all playback
	// Clear all channels/sounds
	ERRCHECK(mTracks["Mx3_Main"]->stop());

	for(auto iter = mSounds.begin(); iter != mSounds.end(); ++iter)
		if(iter->second)
			ERRCHECK(iter->second->release());

	mSounds.clear();
	mQueue.clear();
}

void Mx3::ErrorCheck(FMOD_RESULT result, const char *file, int line)
{
	if(result != FMOD_OK)
	{
		if(mErrorCallback)
			mErrorCallback(FMOD_ErrorString(result), file, line);
	}
}

void Mx3::LoadSound(const std::string name)
{

}

void Mx3::Next()
{

}

void Mx3::Pause()
{

}

void Mx3::Play(int index)
{

}

int Mx3::PlaySound(const std::string name)
{
	return 0;
}

void Mx3::Previous()
{

}

void Mx3::RemoveFromQueue(std::string file)
{

}

void Mx3::SetErrorCallback(ErrorCallback func)
{
	mErrorCallback = func;
}

void Mx3::SetVolume(float volume)
{

}

void Mx3::Stop()
{

}

Mx3::~Mx3()
{

}