#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "common.h"
#include <cstdio>
#include <string>
#include <vector>
#include <debugapi.h>

void CheckFlags(FMOD_MODE num, int channelID)
{
    std::string yes = "True";
    std::string no = "False";
    std::string result = "\nResults: Channel #(" + std::to_string(channelID) + ")\n";

    result += "FMOD_DEFAULT:\t\t" + (num & FMOD_DEFAULT ? yes : no) + "\n";
    result += "FMOD_LOOP_OFF:\t\t" + (num & FMOD_LOOP_OFF ? yes : no) + "\n";
    result += "FMOD_LOOP_NORMAL:\t\t" + (num & FMOD_LOOP_NORMAL ? yes : no) + "\n";
    result += "FMOD_LOOP_BIDI:\t\t" + (num & FMOD_LOOP_BIDI ? yes : no) + "\n";
    result += "FMOD_2D:\t\t" + (num & FMOD_2D ? yes : no) + "\n";
    result += "FMOD_3D:\t\t" + (num & FMOD_3D ? yes : no) + "\n";
    result += "FMOD_CREATESTREAM:\t\t" + (num & FMOD_CREATESTREAM ? yes : no) + "\n";
    result += "FMOD_CREATESAMPLE:\t\t" + (num & FMOD_CREATESAMPLE ? yes : no) + "\n";
    result += "FMOD_CREATECOMPRESSEDSAMPLE:\t\t" + (num & FMOD_CREATECOMPRESSEDSAMPLE ? yes : no) + "\n";
    result += "FMOD_OPENUSER:\t\t" + (num & FMOD_OPENUSER ? yes : no) + "\n";
    result += "FMOD_OPENMEMORY:\t\t" + (num & FMOD_OPENMEMORY ? yes : no) + "\n";
    result += "FMOD_OPENMEMORY_POINT:\t\t" + (num & FMOD_OPENMEMORY_POINT ? yes : no) + "\n";
    result += "FMOD_OPENRAW:\t\t" + (num & FMOD_OPENRAW ? yes : no) + "\n";
    result += "FMOD_OPENONLY:\t\t" + (num & FMOD_OPENONLY ? yes : no) + "\n";
    result += "FMOD_ACCURATETIME:\t\t" + (num & FMOD_ACCURATETIME ? yes : no) + "\n";
    result += "FMOD_MPEGSEARCH:\t\t" + (num & FMOD_MPEGSEARCH ? yes : no) + "\n";
    result += "FMOD_NONBLOCKING:\t\t" + (num & FMOD_NONBLOCKING ? yes : no) + "\n";
    result += "FMOD_UNIQUE:\t\t" + (num & FMOD_UNIQUE ? yes : no) + "\n";
    result += "FMOD_3D_HEADRELATIVE:\t\t" + (num & FMOD_3D_HEADRELATIVE ? yes : no) + "\n";
    result += "FMOD_3D_WORLDRELATIVE:\t\t" + (num & FMOD_3D_WORLDRELATIVE ? yes : no) + "\n";
    result += "FMOD_3D_INVERSEROLLOFF:\t\t" + (num & FMOD_3D_INVERSEROLLOFF ? yes : no) + "\n";
    result += "FMOD_3D_LINEARROLLOFF:\t\t" + (num & FMOD_3D_LINEARROLLOFF ? yes : no) + "\n";
    result += "FMOD_3D_LINEARSQUAREROLLOFF:\t\t" + (num & FMOD_3D_LINEARSQUAREROLLOFF ? yes : no) + "\n";
    result += "FMOD_3D_INVERSETAPEREDROLLOFF:\t\t" + (num & FMOD_3D_INVERSETAPEREDROLLOFF ? yes : no) + "\n";
    result += "FMOD_3D_CUSTOMROLLOFF:\t\t" + (num & FMOD_3D_CUSTOMROLLOFF ? yes : no) + "\n";
    result += "FMOD_3D_IGNOREGEOMETRY:\t\t" + (num & FMOD_3D_IGNOREGEOMETRY ? yes : no) + "\n";
    result += "FMOD_IGNORETAGS:\t\t" + (num & FMOD_IGNORETAGS ? yes : no) + "\n";
    result += "FMOD_LOWMEM:\t\t" + (num & FMOD_LOWMEM ? yes : no) + "\n";
    result += "FMOD_VIRTUAL_PLAYFROMSTART:\t\t" + (num & FMOD_VIRTUAL_PLAYFROMSTART ? yes : no) + "\n\n";

    OutputDebugStringA(result.c_str());
}

std::string GetInfo(FMOD::Channel* channel)
{
    FMOD::Sound* sound;
    bool stopChannel = false;
    int loopCount = 0, syncPoints = 0, index = 0;
    unsigned int loopstart = 0, loopend = 0, soundStart = 0, soundEnd = 0;
    unsigned long long dspStart = 0, dspEnd = 0;
	std::vector<FMOD_SYNCPOINT*> points;
	std::vector<unsigned int> offsets;
	unsigned int nextPosition = 0;
    std::string result = "";

    ERRCHK(channel->getLoopCount(&loopCount));
    ERRCHK(channel->getLoopPoints(&loopstart, FMOD_TIMEUNIT_MS, &loopend, FMOD_TIMEUNIT_MS));
    ERRCHK(channel->getDelay(&dspStart, &dspEnd, &stopChannel));
    ERRCHK(channel->getPosition(&nextPosition, FMOD_TIMEUNIT_MS));
    ERRCHK(channel->getCurrentSound(&sound));
    if (sound)
    {
        ERRCHK(sound->getNumSyncPoints(&syncPoints));
		offsets.reserve(syncPoints);
        for (int i = 0; i < syncPoints; ++i)
        {
			points.push_back(nullptr);
            ERRCHK(sound->getSyncPoint(i, &points[i]));
            ERRCHK(sound->getSyncPointInfo(points[i], 0, 0, &offsets[i], FMOD_TIMEUNIT_MS));
        }
        ERRCHK(sound->getLoopPoints(&soundStart, FMOD_TIMEUNIT_MS, &soundEnd, FMOD_TIMEUNIT_MS));
    }

    result += "Channel Loop Count: " + std::to_string(loopCount) + "\n";
    result += "POSITION\n";
    result += "Loop Start: " + std::to_string(loopstart) + ", Loop End: " + std::to_string(loopend) + "\n";
    result += "Delay Start: " + std::to_string(dspStart) + ", End: " + std::to_string(dspEnd) + ", Stop: " + (stopChannel ? "True\n" : "False\n");
    result += "Sync Points (" + std::to_string(syncPoints) + "):\n";
	for (int i = 0; i < offsets.size(); ++i)
	{
		result += "#" + std::to_string(i) + " Offset: " + std::to_string(offsets[i]) + " (ms)\n";
	}
    result += "Sound Loop Start: " + std::to_string(soundStart) + ", Loop End: " + std::to_string(soundEnd) + "\n";
    result += "=================\n\n";

    return result;
}

int FMOD_Main()
{
	Common_Init(0);

    FMOD::Studio::System* system = NULL;
    FMOD::System* coreSystem = NULL;
    FMOD::Studio::Bank* masterBank = 0, * stringsBank = 0, * touchBank = 0;
    FMOD::Studio::EventDescription* eventDesc = NULL;
    FMOD::Studio::EventInstance* eventInst = NULL;
    FMOD::Channel* channel = NULL;
    FMOD::Sound* sound = NULL;
    int ms = 0;
    bool bStarted = false;
    int index = 0;
    std::string info = "", rates = "";
    float freq;
    int outputrate;
    std::string data = "";

	ERRCHK(FMOD::Studio::System::create(&system));
	ERRCHK(system->getCoreSystem(&coreSystem));

	int numDrivers = 0;
	int chosenDriver = -1;
	int toggledDriver = 0;
	std::vector<char*> names;
	ERRCHK(coreSystem->getNumDrivers(&numDrivers));

	for (int i = 0; i < numDrivers; ++i)
	{
		names.push_back(new char[MAX_PATH]);
		coreSystem->getDriverInfo(i, names[i], MAX_PATH, 0, 0, 0, 0);
	}

	do
	{
		Common_Update();

		Common_Draw("Please pick an output device:");
		for (int i = 0; i < numDrivers; ++i)
		{
			Common_Draw("%d - %s", i, names[i]);
		}
		Common_Draw("Press %s to cycle up", Common_BtnStr(BTN_UP));
		Common_Draw("Press %s to cycle down", Common_BtnStr(BTN_DOWN));
		Common_Draw("Press %s to select driver", Common_BtnStr(BTN_ACTION1));

		if (Common_BtnPress(BTN_UP))
		{
			toggledDriver = Common_Clamp(0, toggledDriver + 1, numDrivers - 1);
		}
		if (Common_BtnPress(BTN_DOWN))
		{
			toggledDriver = Common_Clamp(0, toggledDriver - 1, numDrivers - 1);
		}
		if (Common_BtnPress(BTN_ACTION1))
		{
			chosenDriver = toggledDriver;
		}

		Common_Draw("Selected Driver: %d", toggledDriver);
		Common_Sleep(50);
	} while (chosenDriver < 0);

	ERRCHK(coreSystem->setDriver(chosenDriver));
	ERRCHK(system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, 0));

	ERRCHK(system->loadBankFile("C:\\Users\\Sigmund\\Documents\\FMOD Studio\\Music Prototype\\Build\\Desktop\\Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
	ERRCHK(system->loadBankFile("C:\\Users\\Sigmund\\Documents\\FMOD Studio\\Music Prototype\\Build\\Desktop\\Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));
	ERRCHK(system->loadBankFile("C:\\Users\\Sigmund\\Documents\\FMOD Studio\\Music Prototype\\Build\\Desktop\\Please Dont Touch.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &touchBank));

	ERRCHK(system->getEvent("event:/Please Dont Touch/Title", &eventDesc));
	ERRCHK(eventDesc->createInstance(&eventInst));

	ERRCHK(coreSystem->getSoftwareFormat(&outputrate, 0, 0));

	/*
		Main loop
	*/
	do
	{
		Common_Update();

		if (Common_BtnPress(BTN_MORE))
		{
			ERRCHK(eventInst->start());
		}

		ERRCHK(system->update());

		info = "";
		for (int k = 0; k < 1024; ++k)
		{
			ERRCHK(coreSystem->getChannel(k, &channel));
			FMOD_RESULT result;
			bool playing = false;
			result = channel->isPlaying(&playing);
			if (result == FMOD_OK)
			{
				index = k;
				bStarted = true;
				ERRCHK(coreSystem->getChannel(index, &channel));
				info += GetInfo(channel);
			}
			else if (result != FMOD_ERR_INVALID_HANDLE)
				ERRCHK(result);
		}

		if (Common_BtnPress(BTN_ACTION2))
		{
			int position = 0;
			ERRCHK(eventInst->getTimelinePosition(&position));
			ERRCHK(eventInst->setTimelinePosition(position - 10000));
		}

		if (Common_BtnPress(BTN_ACTION3))
		{
			int position = 0;
			ERRCHK(eventInst->getTimelinePosition(&position));
			ERRCHK(eventInst->setTimelinePosition(position + 10000));
		}

		if (Common_BtnPress(BTN_ACTION4))
		{
			FMOD::Channel* chan = 0;

			for (int k = 0; k < 1024; ++k)
			{
				ERRCHK(coreSystem->getChannel(k, &chan));
				FMOD_RESULT result;
				bool playing = false;
				FMOD_MODE mode;
				result = chan->isPlaying(&playing);
				if (result == FMOD_OK && playing)
				{
					ERRCHK(chan->getMode(&mode));
					CheckFlags(mode, k);
					continue;
				}
				else if (result != FMOD_ERR_INVALID_HANDLE)
					ERRCHK(result);
			}
		}

		ERRCHK(eventInst->getTimelinePosition(&ms));

		//Common_Draw("==================================================");
		//Common_Draw("Event Parameter Example.");
		//Common_Draw("Copyright (c) Firelight Technologies 2012-2019.");
		//Common_Draw("==================================================");
		Common_Draw("Press %s to play event", Common_BtnStr(BTN_MORE));
		Common_Draw("Press %s to rewind by 10 secs.", Common_BtnStr(BTN_ACTION2));
		Common_Draw("Press %s to skip ahead by 10 secs.", Common_BtnStr(BTN_ACTION3));
		Common_Draw("Press %s to print channel flags.", Common_BtnStr(BTN_ACTION4));
		Common_Draw("Time: %02d:%02d:%02d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100);
		Common_Draw(info.c_str());
		Common_Draw("");
		Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));

		Common_Sleep(50);
	} while (!Common_BtnPress(BTN_QUIT));

	/*
		Shut down
	*/
	ERRCHK(system->release());

	Common_Close();

    return 0;
}
