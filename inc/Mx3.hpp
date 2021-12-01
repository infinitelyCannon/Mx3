#pragma once

#include <string>
#include <map>
#include <vector>
#include "fmod.hpp"

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef std::map<int, FMOD::Channel*> ChannelMap;
typedef std::map<std::string, FMOD::ChannelGroup*> TrackMap;

class Mx3
{
public:
	Mx3();
	~Mx3();

	void Update(float deltaTime);
	void AddToQueue(std::string file);
	void RemoveFromQueue(std::string file);
	void ClearQueue();
	void Next();
	void Previous();
	void Play(int index = 0);
	void Pause();
	void Stop();
	void SetVolume(float volume);

private:
	void LoadSound(const std::string name);
	void UnLoadSound(const std::string name);
	int PlaySound(const std::string name);

	std::vector<std::string> mQueue;
	SoundMap mSounds;
	ChannelMap mChannels;
	TrackMap mTracks;
	int NextChannelID = 0;
};