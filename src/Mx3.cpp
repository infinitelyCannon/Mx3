#include "Mx3.hpp"
#include "fmod_errors.h"
#include "Song.hpp"
#include <cstdlib>

#define ERRCHECK(x) ErrorCheck(x, __FILE__, __LINE__)
#define HEAD_CHANNEL "Mx3_Main"
#define SOLO_CHANNEL "Mx3_Solo"

ErrorCallback Mx3::mErrorCallback = nullptr;

/*
* TODO: Find a way to have the library be imported without the user having to use/include FMOD headers
* (Might help with glue implementations)
*/
Mx3::Mx3() : mSystem(nullptr)
{
	// TODO: Add call to CoInitialize for Windows
	ERRCHECK(FMOD::System_Create(&mSystem, FMOD_VERSION));
	ERRCHECK(mSystem->init(32, FMOD_INIT_NORMAL, 0));

	mTracks[HEAD_CHANNEL] = nullptr;
	mTracks[SOLO_CHANNEL] = nullptr;

	ERRCHECK(mSystem->createChannelGroup(HEAD_CHANNEL, &mTracks[HEAD_CHANNEL]));
	ERRCHECK(mSystem->createChannelGroup(SOLO_CHANNEL, &mTracks[SOLO_CHANNEL]));
	ERRCHECK(mTracks[HEAD_CHANNEL]->addGroup(mTracks[SOLO_CHANNEL]));
}

Mx3::Mx3(int maxchannels, FMOD_INITFLAGS flags, void* driverdata):
	mSystem(nullptr)
{
	ERRCHECK(FMOD::System_Create(&mSystem, FMOD_VERSION));

	ERRCHECK(mSystem->init(maxchannels, flags, driverdata));

	mTracks[HEAD_CHANNEL] = nullptr;
	mTracks[SOLO_CHANNEL] = nullptr;

	ERRCHECK(mSystem->createChannelGroup(HEAD_CHANNEL, &mTracks[HEAD_CHANNEL]));
	ERRCHECK(mSystem->createChannelGroup(SOLO_CHANNEL, &mTracks[SOLO_CHANNEL]));
	ERRCHECK(mTracks[HEAD_CHANNEL]->addGroup(mTracks[SOLO_CHANNEL]));
}

void Mx3::AddToQueue(std::string file)
{
	if (file.empty())
		return;

	mQueue.push_back(file);
}

void Mx3::AddToQueue(std::vector<std::string> &files)
{
	mQueue.insert(mQueue.end(), files.begin(), files.end());
}

void Mx3::ClearQueue()
{
	Stop();
	mQueue.clear();
	for (auto sound : mSounds)
		ERRCHECK(sound.second->release());
	mSounds.clear();
}

void Mx3::Debug()
{
	bDebug = true;
}

void Mx3::ErrorCheck(FMOD_RESULT result, const char *file, int line)
{
	if(result != FMOD_OK)
	{
		if(mErrorCallback)
			mErrorCallback(FMOD_ErrorString(result), file, line);
	}
}

unsigned int Mx3::GetLength()
{
	if(mChannels.empty())
		return 0;
	
	return mChannels.begin()->second.GetLength();
}

bool Mx3::GetPaused()
{
	bool paused = false;
	ERRCHECK(mTracks[HEAD_CHANNEL]->getPaused(&paused));
	return paused;
}

unsigned int Mx3::GetPosition()
{
	if (bDebug)
		bDebug = false;

	if(mChannels.empty())
		return 0;

	return mChannels.begin()->second.GetPosition();
}

float Mx3::GetVolume()
{
	float volume = 0;
	ERRCHECK(mTracks[HEAD_CHANNEL]->getVolume(&volume));
	return volume;
}

bool Mx3::IsPlaying()
{
	bool result = false;;
	ERRCHECK(mTracks[HEAD_CHANNEL]->isPlaying(&result));
	return result;
}

bool Mx3::IsSoundLoaded(const std::string file)
{
	auto songRef = mSounds.find(file);
	if(songRef == mSounds.end())
		return false;

	if(songRef->second)
	{
		FMOD_OPENSTATE state;
		ERRCHECK(songRef->second->getOpenState(&state, 0, 0, 0));
		return state != FMOD_OPENSTATE_ERROR;
	}
	
	return false;
}

void Mx3::LoadSound(const std::string name)
{
	if (IsSoundLoaded(name))
		return;

	auto soundRef = mSounds.find(name);
	if (soundRef != mSounds.end())
		return;

	mSounds[name] = nullptr;
	FMOD_MODE mode = FMOD_NONBLOCKING | FMOD_CREATESTREAM | FMOD_2D | FMOD_LOOP_NORMAL;
	ERRCHECK(mSystem->createSound(name.c_str(), mode, 0, &mSounds[name]));
}

void Mx3::Next()
{
	if (mQueue.empty())
		return;
	else if (mQueue.size() == 1)
		Play(NowPlayingIdx);

	Play((NowPlayingIdx + 1) % mQueue.size());
}

void Mx3::Play(int index)
{
	if (index >= mQueue.size() || index < 0)
		return;

	if (!mChannels.empty())
		Stop();

	LoadSound(mQueue[index]);

	NowPlayingIdx = index;
	
	mChannels.emplace(
		std::make_pair(NextChannelID++, Song(mSystem, mSounds[mQueue[index]], mTracks[SOLO_CHANNEL]))
	);
}

void Mx3::PlaySound(std::string file)
{
	auto songRef = mSounds.find(file);

	if(songRef == mSounds.end())
	{
		LoadSound(file);
		songRef = mSounds.find(file);
		if(songRef == mSounds.end())
			return;
	}

	mChannels.emplace(
		std::make_pair(NextChannelID++, Song(mSystem, mSounds[file], mTracks[SOLO_CHANNEL]))
	);
}

void Mx3::Previous()
{
	// TODO: Have it restart currrent song if at/before certain position
	if (mQueue.empty() || NowPlayingIdx == 0)
		return;
	else if (mQueue.size() == 1)
		Play(NowPlayingIdx);
	
	Play((NowPlayingIdx - 1) % mQueue.size());
}

void Mx3::RemoveFromQueue(int index)
{
	if (index < 0 || index >= mQueue.size())
		return;
	else if (mQueue.size() == 1)
	{
		ClearQueue();
		return;
	}

	//NOTE: Might need to switch to having Songs manage the Sound *'s
	mQueue.erase(mQueue.begin() + index);

	if (index < NowPlayingIdx)
		--NowPlayingIdx;
	else if (index == NowPlayingIdx)
		Play(NowPlayingIdx);
}

void Mx3::SetErrorCallback(ErrorCallback func)
{
	mErrorCallback = func;
	Song::SetErrorCallback(func);
}

void Mx3::SetPaused(bool paused)
{
	ERRCHECK(mTracks[HEAD_CHANNEL]->setPaused(paused));
}

void Mx3::SetVolume(float volume, bool clamp)
{
	if(clamp)
	{
		if(volume < 0) volume = 0;
		else if(volume > 1.0f) volume = 1.0f;
	}

	ERRCHECK(mTracks[HEAD_CHANNEL]->setVolume(volume));
}

void Mx3::Stop()
{
	for (auto it = mTracks.begin(); it != mTracks.end();)
	{
		if (it->first == HEAD_CHANNEL || it->first == SOLO_CHANNEL)
			++it;
		else
		{
			ERRCHECK(it->second->stop());
			it = mTracks.erase(it);
		}
	}

	mChannels.clear();
	NextChannelID = 0;
}

void Mx3::SwapSongs(int first, int second)
{
	if (first < 0 || first >= mQueue.size() || second < 0 || second >= mQueue.size())
		return;

	if (first == second)
		return;
	
	std::string temp = mQueue[first];
	mQueue[first] = mQueue[second];
	mQueue[second] = temp;

	if (NowPlayingIdx == first)
		NowPlayingIdx = second;
	else if (NowPlayingIdx == second)
		NowPlayingIdx = first;
}

void Mx3::UnLoadSound(std::string file)
{
	auto soundRef = mSounds.find(file);

	if(soundRef != mSounds.end())
		return;

	ERRCHECK(mSounds[file]->release());
	mSounds.erase(soundRef);
}

void Mx3::Update(float deltaTime)
{
	ERRCHECK(mSystem->update());

	for (auto it = mChannels.begin(); it != mChannels.end();)
	{
		it->second.Update(deltaTime);
		if (it->second.IsStopped())
			it = mChannels.erase(it);
		else
			++it;
	}
}

Mx3::~Mx3()
{
	ERRCHECK(mTracks[HEAD_CHANNEL]->stop());
	for(auto sound : mSounds)
		ERRCHECK(sound.second->release());

	ERRCHECK(mSystem->release());
}