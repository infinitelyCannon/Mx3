#include "Mx3.hpp"
#include "fmod_errors.h"
#include "Song.hpp"
#include <iostream>
#include <format>

#define ERRCHECK(x) ErrorCheck(x, __FILE__, __LINE__)

ErrorCallback Mx3::mErrorCallback = nullptr;

/*
* TODO: Find a way to have the library be imported without the user having to use/include FMOD headers
* (Might help with glue implementations)
*/
Mx3::Mx3()
{
	// TODO: Add call to CoInitialize for Windows
	mImplementation = new Implementation;
	ERRCHECK(FMOD::System_Create(&mImplementation->mSystem, FMOD_VERSION));
	ERRCHECK(mImplementation->mSystem->init(32, FMOD_INIT_NORMAL, 0));
}

Mx3::Mx3(int maxchannels, FMOD_INITFLAGS flags, void* driverdata)
{
	mImplementation = new Implementation;
	ERRCHECK(FMOD::System_Create(&mImplementation->mSystem, FMOD_VERSION));
	ERRCHECK(mImplementation->mSystem->init(maxchannels, flags, driverdata));
}

FMOD_RESULT F_CALLBACK Mx3::ChannelCallback(FMOD_CHANNELCONTROL* ctrl, FMOD_CHANNELCONTROL_TYPE ctrlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callType, void* data1, void* data2)
{
	return FMOD_OK;
}

double Mx3::DSP2PCM(unsigned long long dsp, double soundRate, double mixerRate)
{
	return 0;
}

void Mx3::ErrorCheck(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		std::string msg = std::format("[Mx3]:\t{} ({}) - {}", file, line, FMOD_ErrorString(result));
		if (mErrorCallback)
		{
			mErrorCallback(msg.c_str());
		}
		else
			std::cerr << msg << std::endl;
	}
}

float Mx3::GetVolume()
{
	return 0;
}

bool Mx3::IsPlaying()
{
	return false;
}

bool Mx3::IsSoundLoaded(const std::string file)
{
	return false;
}

void Mx3::LoadSound(std::string file)
{
	if (IsSoundLoaded(file))
		return;

	auto foundIt = mImplementation->Sounds.find(file);
	if (foundIt != mImplementation->Sounds.end())
		return;
}

unsigned long long Mx3::PCM2DSP(unsigned long long posiiton, unsigned long long soundRate, unsigned long long mixerRate)
{
	return 0;
}

unsigned int Mx3::PCM2MS(double pcm, float soundRate)
{
	return 0;
}

void Mx3::PlaySound(std::string file)
{

}

void Mx3::SetErrorCallback(ErrorCallback func)
{
	mErrorCallback = func;
}

void Mx3::SetPaused(bool paused)
{

}

void Mx3::SetVolume(float volume, bool clamp)
{

}

void Mx3::UnLoadSound(std::string file)
{

}

void Mx3::Update(float deltaTime)
{

}

Implementation::~Implementation()
{
	for (auto &it : Cleanup)
		it.first->release();

	Cleanup.clear();
	Channels.clear();

	for (auto sound : Sounds)
		sound.second->release();

	Sounds.clear();
	mSystem->close();
	mSystem->release();
}

Mx3::~Mx3()
{
	delete mImplementation;
}