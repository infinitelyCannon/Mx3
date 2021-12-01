#include "common.h"
/*
#include "fmod.hpp"
#include "fmod_errors.h"
#include "common.h"

#include <vector>
#include <algorithm>
#include <Windows.h>
#include <debugapi.h>
#include <string>
#include <cstdio>
#include <chrono>

#define LOOP_START 16.019640231035808
#define LOOP_END 80086
#define LOOP_LEN 64.067886145552563
#define LOOP_SONG "C:\\Users\\Sigmund\\Desktop\\Temp\\Title Screen.mp3"
*/

/*
* This struct is to simplify the task of chosing which
* channel pointer to reference during playback.
*/
/*
struct Swappa
{
	FMOD::Channel *Main;
	FMOD::Channel *Swap;
	bool IsMainOldest;

	Swappa() :
		Main(NULL),
		Swap(NULL),
		IsMainOldest(false)
	{
	}

	FMOD::Channel *Newest() { return (IsMainOldest ? Swap : Main); }

	FMOD::Channel *operator->()
	{
		if (Main == NULL && Swap == NULL)
			return NULL;
		if (Main == NULL)
			return Swap;
		if (Swap == NULL)
			return Main;

		bool playA, playB;
		FMOD_RESULT resultA, resultB;

		resultA = Main->isPlaying(&playA);
		resultB = Swap->isPlaying(&playB);

		if (resultA == FMOD_OK && resultB == FMOD_OK)
			return (IsMainOldest ? Main : Swap);
		if (resultA == FMOD_OK)
			return Main;
		if (resultB == FMOD_OK)
			return Swap;
		else
			return Main;
	}

	FMOD::Channel **operator&()
	{
		if (IsMainOldest)
		{
			IsMainOldest = false;
			return &Main;
		}
		else
		{
			IsMainOldest = true;
			return &Swap;
		}
	}
};

Swappa Wappa;
FMOD::Sound *SwapSound;
FMOD::Sound *Sound = NULL;
unsigned int soundRate = 0;
int outputRate;
bool Swapping = false;
std::vector<FMOD::Sound *> Cleanup;

unsigned long long PCM2DSP(unsigned long long position, unsigned long long soundRate, unsigned long long mixerRate)
{
	return (position / soundRate * mixerRate);
}

double DSP2PCM(unsigned long long dsp, double soundrate, double mixerRate)
{
	return (dsp / mixerRate * soundrate);
}

unsigned int PCM2MS(double pcm, float soundrate)
{
	return (1000.0 * pcm) / soundRate;
}

FMOD_RESULT F_CALLBACK HotSwap(FMOD_CHANNELCONTROL *control, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *data1, void *data2)
{
	if (controlType == FMOD_CHANNELCONTROL_CHANNEL && callbackType == FMOD_CHANNELCONTROL_CALLBACK_SYNCPOINT)
	{
		FMOD::System *sys = NULL;
		FMOD::Channel *channel = (FMOD::Channel *)control;
		ERRCHECK(channel->getSystemObject(&sys));
		unsigned int pos;
		//char buffer[FILENAME_MAX]{};

		ERRCHECK(sys->createStream(LOOP_SONG, FMOD_NONBLOCKING | FMOD_ACCURATETIME, 0, &Sound));

		unsigned long long targetPos, loopEnd, currentPos;
		unsigned long long parent, local;

		ERRCHECK(channel->getPosition(&pos, FMOD_TIMEUNIT_PCM));
		loopEnd = LOOP_END * soundRate / 1000;
		currentPos = PCM2DSP(pos, soundRate, outputRate);
		targetPos = PCM2DSP(loopEnd, soundRate, outputRate);
		ERRCHECK(channel->getDSPClock(&local, &parent));
		targetPos -= currentPos;
		ERRCHECK(channel->setDelay(0, parent + targetPos, true));
		Swapping = true;
		//std::sprintf(buffer, "[USER] PCM: %u, DSP: %llu, Dist: %llu\n", pos, currentPos, targetPos);
		//OutputDebugString(buffer);
	}
	if (controlType == FMOD_CHANNELCONTROL_CHANNEL && callbackType == FMOD_CHANNELCONTROL_CALLBACK_END)
	{
		FMOD::Channel *channel = (FMOD::Channel *)control;

		Cleanup.push_back(0);
		ERRCHECK(channel->getCurrentSound(&Cleanup.back()));
	}

	return FMOD_OK;
}
*/

int FMOD_Main()
{
	/*
	FMOD::System *System = NULL;
	FMOD::Channel *Channel = NULL;
	FMOD::ChannelGroup *Group = NULL;
	FMOD::ChannelGroup *Master = NULL;
	FMOD_SYNCPOINT *Point;
	FMOD_RESULT result;
	unsigned int version;
	unsigned long long clock, lastClock;
	unsigned long long delta = 0, Position = 0, lastDiff = 0;
	float soundrate = 0.0f;
	float volume = 1.0f;
	std::string msg = "";
	std::chrono::steady_clock::time_point time, lastTime;
	std::chrono::duration<float, std::milli> deltaTime;

	Common_Init(0);

	ERRCHECK(FMOD::System_Create(&System));
	ERRCHECK(System->getVersion(&version));

	if (version < FMOD_VERSION)
		Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);

	ERRCHECK(System->init(32, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, 0));

	ERRCHECK(System->getSoftwareFormat(&outputRate, 0, 0));

	ERRCHECK(System->createChannelGroup("Parent", &Group));
	ERRCHECK(Group->setPaused(true));
	ERRCHECK(System->getMasterChannelGroup(&Master));

	ERRCHECK(System->createStream(LOOP_SONG, FMOD_ACCURATETIME | FMOD_2D, 0, &Sound));

	ERRCHECK(System->playSound(Sound, Group, true, &Wappa.Main));

	ERRCHECK(Sound->getDefaults(&soundrate, 0));
	//ERRCHECK(Wappa->setDelay(0, ));
	soundRate = static_cast<unsigned int>(soundrate);
	//ERRCHECK(Sound->addSyncPoint(LOOP_END - 500, FMOD_TIMEUNIT_MS, "LoopTest", &Point));
	//ERRCHECK(Wappa->setCallback(HotSwap));
	ERRCHECK(Wappa->setPaused(false));
	ERRCHECK(Group->setPaused(false));
	ERRCHECK(Group->getDSPClock(&lastClock, 0));
	lastClock = PCM2MS(DSP2PCM(lastClock, soundrate, outputRate), soundrate);
	time = std::chrono::steady_clock::now();

	do
	{
		Common_Update();

		if (Common_BtnPress(BTN_ACTION1) && (Wappa.Main || Wappa.Swap))
		{
			bool paused;
			ERRCHECK(Group->getPaused(&paused));

			ERRCHECK(Group->setPaused(!paused));
		}

		if (Common_BtnPress(BTN_ACTION2))
		{
			ERRCHECK(Group->getVolume(&volume));
			volume -= 0.1f;
			ERRCHECK(Group->setVolume(std::clamp(volume, 0.0f, 1.0f)));
		}

		if (Common_BtnPress(BTN_ACTION3))
		{
			ERRCHECK(Group->getVolume(&volume));
			volume += 0.1f;
			ERRCHECK(Group->setVolume(std::clamp(volume, 0.0f, 1.0f)));
		}

		lastTime = time;
		time = std::chrono::steady_clock::now();
		deltaTime = time - lastTime;

		ERRCHECK(System->update());

		ERRCHECK(Group->getDSPClock(&clock, 0));
		Position = PCM2MS(DSP2PCM(clock, soundrate, outputRate), soundrate) - lastClock;
		*/

		/*if (delta < 0)
		{
			char buffer[FILENAME_MAX]{};
			std::sprintf(buffer, "[USER]: Negative delta! (%llu - %llu = %llu)\n", clock, lastClock, delta);
			OutputDebugString(buffer);
		}*/

/*
		if (Swapping)
		{
			FMOD_OPENSTATE state;
			ERRCHECK(Sound->getOpenState(&state, 0, 0, 0));
			if (state == FMOD_OPENSTATE_READY)
			{
				Swapping = false;
				unsigned long long delay = 0;
				ERRCHECK(Wappa->getDelay(0, &delay));
				ERRCHECK(System->playSound(Sound, Group, true, &Wappa));
				ERRCHECK(Sound->addSyncPoint(LOOP_END - 500, FMOD_TIMEUNIT_MS, "LoopTest", &Point));
				ERRCHECK(Wappa.Newest()->setCallback(HotSwap));
				ERRCHECK(Wappa.Newest()->setDelay(delay, 0, false));
				ERRCHECK(Wappa.Newest()->setPosition(LOOP_START, FMOD_TIMEUNIT_MS));
				ERRCHECK(Wappa.Newest()->setPaused(false));
			}
		}

		{
			unsigned int ms = 0;
			unsigned int lenms = 0;
			bool playing = false;
			bool paused = false;
			int channelsplaying = 0;

			ERRCHECK(System->getChannelsPlaying(&channelsplaying, NULL));

			if (Wappa.Main || Wappa.Swap)
			{
				FMOD::Sound *currentsound = 0;

				result = Group->isPlaying(&playing);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
				{
					ERRCHECK(result);
				}

				result = Group->getPaused(&paused);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
				{
					ERRCHECK(result);
				}

				result = Wappa->getPosition(&ms, FMOD_TIMEUNIT_MS);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
				{
					ERRCHECK(result);
				}

				Wappa->getCurrentSound(&currentsound);
				if (currentsound)
				{
					result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
					if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
					{
						ERRCHECK(result);
					}
				}
			}

			if (Common_BtnPress(BTN_ACTION4))
			{
				char buffer[FILENAME_MAX]{};
				std::sprintf(buffer, "[USER] MS: %u, DSP: %llu, Clock: %llu, LastClock: %llu, Delta: %llu\n", ms, Position, clock, lastClock, delta);
				OutputDebugString(buffer);
			}

			Common_Draw("==================================================");
			Common_Draw("Play Sound Example.");
			Common_Draw("Testing Channel loop points w. Profiler");
			Common_Draw("==================================================");
			Common_Draw("");
			Common_Draw("Press %s to toggle pause.", Common_BtnStr(BTN_ACTION1));
			Common_Draw("Press %s to lower volume.", Common_BtnStr(BTN_ACTION2));
			Common_Draw("Press %s to raise volume.", Common_BtnStr(BTN_ACTION3));
			Common_Draw("Press %s to print.", Common_BtnStr(BTN_ACTION4));
			Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
			Common_Draw("");
			Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
			Common_Draw("Channels Playing %d", channelsplaying);
			Common_Draw("Volume: %f", volume);
			Common_Draw("Difference: %d, Delta: %f", ms - Position, deltaTime.count() / 1000.0f);
			Common_Draw("DSP Time: %02d:%02d:%02d", Position / 1000 / 60, Position / 1000 % 60, Position / 10 % 100);
		}

		if (!Cleanup.empty())
		{
			for (FMOD::Sound *sound : Cleanup)
				ERRCHECK(sound->release());
			Cleanup.clear();
		}

		Common_Sleep(50);
	} while (!Common_BtnPress(BTN_QUIT));

	if(Sound)
		ERRCHECK(Sound->release());
	ERRCHECK(System->close());
	ERRCHECK(System->release());
	*/

	Common_Close();

	return 0;
}