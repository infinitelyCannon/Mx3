#include "fmod.hpp"
#include "common.h"
#include "Mx3.hpp"
#include <iostream>
#include <string>

using namespace std;

int FMOD_Main(int argc, char **argv)
{
	void *extradriverdata = 0;
	Mx3 *audio = new Mx3(32, FMOD_INIT_NORMAL, extradriverdata);
	string path;

	bool isPlaying = false;
	bool isPaused = false;
	unsigned int ms = 0;
	unsigned int length = 0;

	Common_Init(&extradriverdata);

	cout << "Enter filepath to play.\n";
	getline(std::cin, path);
	audio->play(path);

	do
	{
		Common_Update();

		if(Common_BtnPress(BTN_ACTION1))
			audio->pause();

		isPlaying = audio->isPlaying();
		isPaused = audio->isPaused();

		if(isPlaying || isPaused)
		{
			ms = audio->getPosition();
			length = audio->getLength();
		}

		Common_Draw("==================================================");
		Common_Draw("Mx3 Audio Engine Multi-Sample Track Test");
		Common_Draw("==================================================");
		Common_Draw("");
		Common_Draw("Press %s to toggle pause.", Common_BtnStr(BTN_ACTION1));
		Common_Draw("Press %s to toggle loop region.", Common_BtnStr(BTN_ACTION2));
		Common_Draw("Press %s to quit.", Common_BtnStr(BTN_QUIT));
		Common_Draw("");
		Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, length / 1000 / 60, length / 1000 % 60, length / 10 % 100, isPaused ? "Paused " : isPlaying ? "Playing" : "Stopped");

		Common_Sleep(20);
	} while(!Common_BtnPress(BTN_QUIT));

	delete audio;

	Common_Close();

	return 0;
}