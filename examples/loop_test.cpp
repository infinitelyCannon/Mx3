#include "fmod.hpp"
#include "common.h"
#include <fstream>
#include <string>
#include <debugapi.h>

/* Needed Data:
* Output rate of the mixer
* postion of sound in PCM samples
* parent Channel clock
*
* From FMOD Docs:
* samples = ms * samplerate / 1000
*/

#define FILE "C:\\Users\\Sigmund\\Documents\\FMOD Studio\\Music Prototype\\Assets\\RIVE\\waterworks_murky_medium_01.ogg"
#define START 16019
#define END 80086

FMOD::Channel* channel = 0;
FMOD::Channel* swap = 0;
FMOD::Sound* sound;
FMOD::Sound* sound2;


FMOD_RESULT F_CALLBACK ChannelCallback(FMOD_CHANNELCONTROL* channelCtrl, FMOD_CHANNELCONTROL_TYPE ctrlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* data1, void* data2)
{
	OutputDebugString("Callback #1!");
	return FMOD_OK;
}

int FMOD_Main()
{
	FMOD::System* system;
	FMOD::ChannelGroup* parent = 0;
	void* externaldata = 0;
	FMOD_RESULT result;
	int outputrate = 0;
	bool usingMain = true;

	int num = 0;
	float volume = 0;

	Common_Init(&externaldata);

	result = FMOD::System_Create(&system);
	ERRCHK(result);

	result = system->init(32, FMOD_INIT_NORMAL, externaldata);
	ERRCHK(result);

	ERRCHK(system->getSoftwareFormat(&outputrate, 0, 0));

	ERRCHK(system->createStream(FILE, FMOD_2D | FMOD_ACCURATETIME | FMOD_LOOP_NORMAL, 0, &sound));

	ERRCHK(system->createChannelGroup("Parent", &parent));

	ERRCHK(system->playSound(sound, parent, false, &channel));

	//ERRCHK(channel->setCallback(ChannelCallback));

	//ERRCHK(sound->addSyncPoint(END - 500, FMOD_TIMEUNIT_MS, "Loop_1_End", 0));

	//unsigned long long clock_start = 0;
	//unsigned int len;
	//float freq;

	//ERRCHK(channel->getDSPClock(0, &clock_start));

	////ERRCHK(sound->getLength(&len, FMOD_TIMEUNIT_PCM));
	//// Double check this math
	//len = 8011 * outputrate / 1000;
	//ERRCHK(sound->getDefaults(&freq, 0));

	//len = (unsigned int)((float)len / freq * outputrate);
	//clock_start += len;

	//ERRCHK(channel->setDelay(0, clock_start, false));
	//ERRCHK(parent->setPaused(false));

	do
	{
		Common_Update();

		if (Common_BtnPress(BTN_ACTION1))
		{
			bool paused;
			result = parent->getPaused(&paused);
			ERRCHK(result);
			result = parent->setPaused(!paused);
			ERRCHK(result);
		}

		if (Common_BtnPress(BTN_ACTION2))
		{
			ERRCHK(parent->getNumChannels(&num));
			ERRCHK(parent->getVolume(&volume));
		}

		if (Common_BtnPress(BTN_ACTION3))
		{
			ERRCHK(parent->stop());
		}

		ERRCHK(system->update());

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
					ERRCHK(result);
				}

				result = parent->getPaused(&paused);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
				{
					ERRCHK(result);
				}

				result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
				{
					ERRCHK(result);
				}

				result = sound->getLength(&lenms, FMOD_TIMEUNIT_MS);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
				{
					ERRCHK(result);
				}

				result = system->getChannelsPlaying(&channelsPlaying);
				ERRCHK(result);
			}

			Common_Draw("==================================================");
			Common_Draw("FMOD API Test");
			Common_Draw("Copyright (c) Firelight Technologies 2004-2021.");
			Common_Draw("==================================================");
			Common_Draw("");
			Common_Draw("Press %s to toggle pause", Common_BtnStr(BTN_ACTION1));
			Common_Draw("Press %s to check parent", Common_BtnStr(BTN_ACTION2));
			Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
			Common_Draw("");
			Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
			Common_Draw("Channels Playing: %d", channelsPlaying);
			Common_Draw("ChannelGroup: %f (%d)", volume, num);
		}

			Common_Sleep(50);
	} while (!Common_BtnPress(BTN_QUIT));

	result = sound->release();
	ERRCHK(result);
	result = parent->release();
	ERRCHK(result);
	result = system->close();
	ERRCHK(result);
	result = system->release();
	ERRCHK(result);

	Common_Close();

	return 0;
}