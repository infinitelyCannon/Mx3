#pragma once

#include <string>
#include <map>
#include "fmod.hpp"

struct Implementation
{
	Implementation();
	~Implementation();

	void Update();
	void LoadSound(std::string path);
	void UnloadSound(std::string path);

	FMOD::System* _system;
	int _nextChannelID;

	typedef std::map<std::string, FMOD::Sound*> SoundMap;
	typedef std::map<int, FMOD::Channel*> ChannelMap;
	SoundMap _sounds;
	ChannelMap _channels;
};

class Mx3
{
public:
	Mx3();
	~Mx3();

	void Update(float deltaTime);

	void LoadSound(std::string path);
	void UnloadSound(std::string path);
	int PlaySound();
	void StopChannel(int channelID);
	void StopAllChannels();
	void SetVolume(float volume, int channelID = -1);

private:
	Implementation* _implementation = nullptr;
};
