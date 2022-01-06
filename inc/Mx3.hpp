#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <utility>
#include "fmod.hpp"
#include "Mx3Types.hpp"

class Mx3
{
public:
	Mx3(int maxchannels, FMOD_INITFLAGS flags, void *driverdata = 0);
	~Mx3();

	void Update(float deltaTime);
	void AddToQueue(std::string file);
	void AddToQueue(std::vector<std::string> &files);
	void RemoveFromQueue(std::string file);
	void ClearQueue();
	void Next();
	void Previous();
	void Play(int index = 0);
	void Pause();
	void Stop();
	void SetVolume(float volume);
	static void SetErrorCallback(ErrorCallback func);

private:
	void LoadSound(const std::string name);
	void UnLoadSound(const std::string name);
	int PlaySound(const std::string name);
	void ErrorCheck(FMOD_RESULT result, const char *file, int line);

	static ErrorCallback mErrorCallback;
	std::vector<std::string> mQueue;
	SoundMap mSounds;
	ChannelMap mChannels;
	TrackMap mTracks;
	FMOD::System* mSystem;
	int NextChannelID = 0;
};