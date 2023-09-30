#include "Mx3.hpp"
#include "Song.hpp"
#include "fmod_errors.h"

Mx3* Mx3::Instance = nullptr;
ErrorCallback Mx3::_errorCallback = nullptr;

#define ERRCHK(_result) Mx3::_errorCallback(_result, __FILE__, __LINE__)

#pragma region AudioAPI
Implementation::Implementation() :
	Implementation(32, FMOD_INIT_NORMAL, 0)
{
}

Implementation::Implementation(int maxChannels, unsigned int flags, void* driverData)
{
	ERRCHK(FMOD::System_Create(&_system));
	ERRCHK(_system->init(maxChannels, flags, driverData));

	ERRCHK(_system->createChannelGroup("Mx3_Main", &_mainChannelGroup));
}

bool Implementation::GetPaused()
{
	bool paused = false;
	ERRCHK(_mainChannelGroup->getPaused(&paused));
	return paused;
}

bool Implementation::HasSound(const std::string file)
{
	return _sounds.contains(file) && !_sounds[file].empty();
}

void Implementation::LoadSound(const std::string file)
{
	_sounds[file].push_back(nullptr);
	ERRCHK(_system->createSound(file.c_str(),
		FMOD_2D |
		FMOD_CREATESTREAM |
		FMOD_ACCURATETIME |
		FMOD_NONBLOCKING,
		nullptr, &(*_sounds[file].rbegin())
		));
}

void Implementation::SetChannelVolume(int channelID, float volume, bool clamp)
{
	if (clamp && volume > 1)
	{
		volume = 1;
	}
	else if (clamp && volume < 0)
	{
		volume = 0;
	}

	if (_channels.find(channelID) != _channels.end())
	{
		_channels[channelID]->SetVolume(volume);
	}
}

void Implementation::SetPaused(bool paused)
{
	ERRCHK(_mainChannelGroup->setPaused(paused));
}

void Implementation::SetVolume(float volume, const bool clamp)
{
	if (clamp && volume > 1)
	{
		volume = 1;
	}
	else if (clamp && volume < 0)
	{
		volume = 0;
	}

	ERRCHK(_mainChannelGroup->setVolume(volume));
}

void Implementation::Stop()
{
	ERRCHK(_mainChannelGroup->stop());
	_channels.clear();

	for (auto it = _sounds.begin(); it != _sounds.end(); ++it)
	{
		for (auto vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			ERRCHK((*vIt)->release());
		}
	}

	_sounds.clear();
}

void Implementation::UnloadSound(const std::string file)
{
	if (!HasSound(file))
	{
		return;
	}

	ERRCHK(_sounds[file].back()->release());
	_sounds[file].pop_back();

	if (_sounds[file].empty())
	{
		_sounds.erase(file);
	}
}

void Implementation::Update(const float deltaTime)
{
	std::vector<ChannelMap::iterator> stoppedChannels;

	ERRCHK(_system->update());

	for (auto it = _channels.begin(); it != _channels.end(); ++it)
	{
		it->second->Update(deltaTime);
		if (it->second->GetState() == Song::State::Stopped)
		{
			stoppedChannels.push_back(it);
		}
	}

	for (auto& it : stoppedChannels)
	{
		_channels.erase(it);
	}
}

Implementation::~Implementation()
{
	ERRCHK(_mainChannelGroup->stop());

	_channels.clear();

	for (auto it = _sounds.begin(); it != _sounds.end(); ++it)
	{
		for (auto vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			ERRCHK((*vIt)->release());
		}
	}

	_sounds.clear();
	ERRCHK(_mainChannelGroup->release());
	ERRCHK(_system->release());
}
#pragma endregion AudioAPI

#pragma region EngineAPI
Mx3::Mx3() : Mx3(32, FMOD_INIT_NORMAL, 0)
{
}

//TODO: Once you get out of the FMOD example code, add calls to init COM on Windows
Mx3::Mx3(int maxChannels, unsigned int flags, void* driverData)
{
	_implementation = new Implementation(maxChannels, flags, driverData);
	Instance = this;
}

//double Mx3::DSP2PCM(unsigned long long dsp, double soundRate, double mixerRate)
//{
//	return dsp / mixerRate * soundRate;
//}

bool Mx3::GetPaused()
{
	return _implementation->GetPaused();
}

void Mx3::LoadSound(const std::string file)
{
	if (!_implementation->HasSound(file))
	{
		return;
	}

	_implementation->LoadSound(file);
}

void Mx3::PlaySound(const std::string path)
{
	if (!_implementation->HasSound(path))
	{
		_implementation->LoadSound(path);
	}

	_implementation->_channels[_implementation->_nextChannelID++] =
		std::make_unique<Song>(_implementation, _implementation->_sounds[path].back());
}

//unsigned long long Mx3::PCM2DSP(unsigned long long posiiton, unsigned long long soundRate, unsigned long long mixerRate)
//{
//	return posiiton / soundRate * mixerRate;
//}

//unsigned int Mx3::PCM2MS(double pcm, float soundRate)
//{
//	return (1000.0 * pcm) / soundRate;
//}

void Mx3::SetChannelVolume(int channelID, float volume, bool clamp)
{
	_implementation->SetChannelVolume(channelID, volume, clamp);
}

void Mx3::SetErrorCallback(ErrorCallback callback)
{
	Mx3::_errorCallback = callback;
	Song::_errorCallback = callback;
}

void Mx3::SetPaused(bool paused)
{
	_implementation->SetPaused(paused);
}

void Mx3::SetVolume(float volume, bool clamp)
{
	_implementation->SetVolume(volume, clamp);
}

void Mx3::Stop()
{
	_implementation->Stop();
}

void Mx3::UnloadSound(const std::string file)
{
	_implementation->UnloadSound(file);
}

void Mx3::Update(const float deltaTime)
{
	_implementation->Update(deltaTime);
}

Mx3::~Mx3()
{
	delete _implementation;
	Instance = nullptr;
}
#pragma endregion EnginePAI