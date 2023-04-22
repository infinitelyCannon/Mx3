#include "fmod.hpp"
#include "common.h"
#include <fstream>
#include <string>

/* Needed Data:
* Output rate of the mixer
* postion of sound in PCM samples
* parent Channel clock
* 
* From FMOD Docs:
* samples = ms * samplerate / 1000
*/

#define FILE "C:\\Users\\Sigmund\\Desktop\\Temp\\Title Screen.mp3"

int FMOD_Main()
{
	FMOD::System* system;
	FMOD::Sound* sound;
	FMOD::Channel* channel = 0;
	FMOD::ChannelGroup* parent = 0;
	void* externaldata = 0;
	FMOD_RESULT result;
	int outputrate = 0;

	Common_Init(&externaldata);

	result = FMOD::System_Create(&system);
	ERRCHECK(result);

	result = system->init(32, FMOD_INIT_NORMAL, externaldata);
	ERRCHECK(result);

	ERRCHECK(system->getSoftwareFormat(&outputrate, 0, 0));

	ERRCHECK(system->createStream(FILE, FMOD_2D | FMOD_ACCURATETIME, 0, &sound));

	ERRCHECK(system->createChannelGroup("Parent", &parent));

	ERRCHECK(parent->setPaused(true));

	ERRCHECK(system->playSound(sound, parent, false, &channel));

	unsigned long long clock_start = 0;
	unsigned int len;
	float freq;

	ERRCHECK(channel->getDSPClock(0, &clock_start));

	//ERRCHECK(sound->getLength(&len, FMOD_TIMEUNIT_PCM));
	// Double check this math
	len = 8011 * outputrate / 1000;
	ERRCHECK(sound->getDefaults(&freq, 0));

	len = (unsigned int)((float)len / freq * outputrate);
	clock_start += len;

	ERRCHECK(channel->setDelay(0, clock_start, false));
	ERRCHECK(parent->setPaused(false));

	do
	{
		Common_Update();

		if (Common_BtnPress(BTN_ACTION1))
		{
			bool paused;
			result = parent->getPaused(&paused);
			ERRCHECK(result);
			result = parent->setPaused(!paused);
			ERRCHECK(result);
		}

		if (Common_BtnPress(BTN_ACTION2))
		{
			result = channel->setPosition(8011, FMOD_TIMEUNIT_MS);
			if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE)
			{
				ERRCHECK(result);
			}
		}

		ERRCHECK(system->update());

		{
			unsigned int ms = 0;
			unsigned int lenms = 0;
			bool         playing = false;
			bool         paused = false;
			int          channelsPlaying = 0;

			if (channel)
			{
				result = parent->isPlaying(&playing);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
				{
					ERRCHECK(result);
				}

				result = parent->getPaused(&paused);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
				{
					ERRCHECK(result);
				}

				result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
				{
					ERRCHECK(result);
				}

				result = sound->getLength(&lenms, FMOD_TIMEUNIT_MS);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
				{
					ERRCHECK(result);
				}

				result = system->getChannelsPlaying(&channelsPlaying);
				ERRCHECK(result);
			}

			Common_Draw("==================================================");
			Common_Draw("FMOD API Test");
			Common_Draw("Copyright (c) Firelight Technologies 2004-2021.");
			Common_Draw("==================================================");
			Common_Draw("");
			Common_Draw("Press %s to toggle pause", Common_BtnStr(BTN_ACTION1));
			Common_Draw("Press %s to jump to 3rd measure", Common_BtnStr(BTN_ACTION2));
			Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
			Common_Draw("");
			Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
			Common_Draw("Channels Playing: %d", channelsPlaying);
		}

			Common_Sleep(50);
	} while (!Common_BtnPress(BTN_QUIT));

	result = sound->release();
	ERRCHECK(result);
	result = system->close();
	ERRCHECK(result);
	result = system->release();
	ERRCHECK(result);

	Common_Close();

	return 0;
}