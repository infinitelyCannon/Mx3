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
Mx3::Mx3() : 
	mSystem(nullptr),
	MasterChannel(nullptr),
	NextChannelID(0)
{
	// TODO: Add call to CoInitialize for Windows
	ERRCHECK(FMOD::System_Create(&mSystem, FMOD_VERSION));
	ERRCHECK(mSystem->init(32, FMOD_INIT_NORMAL, 0));

	ERRCHECK(mSystem->createChannelGroup("Mx3_Master", &MasterChannel));
}

Mx3::Mx3(int maxchannels, FMOD_INITFLAGS fmodFlags, void* driverdata) :
	mSystem(nullptr),
	MasterChannel(nullptr),
	NextChannelID(0)
{
	ERRCHECK(FMOD::System_Create(&mSystem, FMOD_VERSION));
	ERRCHECK(mSystem->init(maxchannels, fmodFlags, driverdata));

	ERRCHECK(mSystem->createChannelGroup("Mx3_Master", &MasterChannel));
}

FMOD_RESULT F_CALLBACK Mx3::ChannelCallback(FMOD_CHANNELCONTROL* ctrl, FMOD_CHANNELCONTROL_TYPE ctrlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callType, void* data1, void* data2)
{
	return FMOD_OK;
}

double Mx3::DSP2PCM(unsigned long long dsp, double soundRate, double mixerRate)
{
	return dsp / mixerRate * soundRate;
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

unsigned int Mx3::GetLength()
{
	if (mChannels.empty() || !IsPlaying())
		return 0;

	//TODO: Need better heuristic to replace this.
	return mChannels.begin()->second->GetLength();
}

bool Mx3::GetPaused()
{
	bool paused;
	ERRCHECK(MasterChannel->getPaused(&paused));

	return paused;
}

unsigned int Mx3::GetPosition()
{
	if (mChannels.empty() || !IsPlaying())
		return 0;

	//TODO: Need better heuristic to replace this.
	return mChannels.begin()->second->GetPosition();
}

float Mx3::GetVolume()
{
	float volume;

	ERRCHECK(MasterChannel->getVolume(&volume));

	return volume;
}

bool Mx3::IsPlaying()
{
	bool playing;

	ERRCHECK(MasterChannel->isPlaying(&playing));

	return playing;
}

bool Mx3::IsSoundLoaded(const std::string file)
{
	auto foundIt = mSounds.find(file);

	if (foundIt == mSounds.end())
		return false;

	FMOD_OPENSTATE state;
	ERRCHECK(mSounds[file]->getOpenState(&state, 0, 0, 0));

	return state != FMOD_OPENSTATE_LOADING;
}

void Mx3::LoadSound(std::string file)
{
	if (IsSoundLoaded(file))
		return;

	auto foundIt = mSounds.find(file);
	if (foundIt != mSounds.end())
		return;

	mSounds[file] = nullptr;
	FMOD_RESULT result = mSystem->createSound(file.c_str(),
		FMOD_2D |				//Not using any 3D panning
		FMOD_CREATESTREAM |		//Streaming it from file in realtime instead of decompressing into memory
		FMOD_LOOP_NORMAL |		//Have the audio setup to loop (final say gived by the playing channel)
		FMOD_NONBLOCKING |		//Non-blocking file load
		FMOD_ACCURATETIME,		//Scan file for accurate position seeking
		nullptr,
		&mSounds[file]);

	if (result != FMOD_OK)
		mSounds.erase(file);

	ERRCHECK(result);
}

unsigned long long Mx3::PCM2DSP(unsigned long long posiiton, unsigned long long soundRate, unsigned long long mixerRate)
{
	return posiiton / soundRate * mixerRate;
}

unsigned int Mx3::PCM2MS(double pcm, float soundRate)
{
	return (1000.0 * pcm) / soundRate;
}

int Mx3::Play(std::string file)
{
	auto foundIt = mSounds.find(file);
	
	if (foundIt == mSounds.end())
	{
		LoadSound(file);
		foundIt = mSounds.find(file);
		if (foundIt == mSounds.end())
			return -1;
	}

	mChannels[NextChannelID++] = std::make_unique<Song>(mSystem, mSounds[file], MasterChannel);
	return NextChannelID - 1;
}

void Mx3::SetErrorCallback(ErrorCallback func)
{
	mErrorCallback = func;
	Song::SetErrorCallback(func);
}

void Mx3::SetPaused(bool paused)
{
	ERRCHECK(MasterChannel->setPaused(paused));
}

void Mx3::SetVolume(float volume, bool clamp)
{
	if (clamp)
		volume = std::clamp(volume, 0.0f, 1.0f);

	ERRCHECK(MasterChannel->setVolume(volume));
}

void Mx3::UnLoadSound(std::string file)
{
	auto foundIt = mSounds.find(file);

	if (foundIt == mSounds.end())
		return;

	//TODO: Add check to make sure any Songs using this are cleaned up
	ERRCHECK(mSounds[file]->release());
	mSounds.erase(file);
}

void Mx3::Update(float deltaTime)
{
	std::vector<std::unique_ptr<Song>> stoppedSongs;

	for (auto it = mChannels.begin(); it != mChannels.end(); ++it)
	{
		it->second->Update(deltaTime);
		if (it->second->GetState() == Song::State::STOPPED)
			stoppedSongs.push_back(std::move(it->second));
	}

	stoppedSongs.clear();

	ERRCHECK(mSystem->update());
}

Mx3::~Mx3()
{
	for (auto& it : RecycleBin)
		ERRCHECK(it.first->release());

	RecycleBin.clear();
	mChannels.clear();

	for (auto sound : mSounds)
		ERRCHECK(sound.second->release());

	mSounds.clear();
	ERRCHECK(mSystem->close());
	ERRCHECK(mSystem->release());
}