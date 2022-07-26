#include "common.h"
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include "Mx3.hpp"
//#include "widgets/widgets.hpp"

//#define TO_BTN(x) static_cast<Button>(x)

void ErrCallback(const char* msg)
{
	Common_Fatal(msg);
}

void LoadQueue(std::vector<std::string> &queue)
{
	std::ifstream file;

	file.open("./args.txt");

	if (!file.is_open())
		Common_Fatal("Arguments file (args.txt) not found");

	queue.clear();
	for (std::string line; std::getline(file, line);)
		queue.push_back(line);

	file.close();
}

int FMOD_Main()
{
	std::string file;
	auto lastTime = std::chrono::steady_clock::now();
	auto currentTime = lastTime;
	int RepeatMode = 1;

	Common_Init(0);

	std::ifstream args;
	args.open("./args.txt");

	if (!args.is_open())
		Common_Fatal("Arguments file (args.txt) not found.");

	std::getline(args, file);

	Mx3::SetErrorCallback(ErrCallback);
	Mx3 Engine(32, FMOD_INIT_NORMAL, 0);

	Engine.LoadSound(file);
	Engine.Play(file);

	lastTime = std::chrono::steady_clock::now();

	do
	{
		currentTime = std::chrono::steady_clock::now();

		Common_Update();

		if(Common_BtnPress(BTN_ACTION1))
		{
			bool isPaused = Engine.GetPaused();
			Engine.SetPaused(!isPaused);
		}

		if (Common_BtnPress(BTN_ACTION2))
		{
			Engine.UnLoadSound(file);
		}

		if (Common_BtnPress(BTN_UP))
		{
			float volume = Engine.GetVolume();
			Engine.SetVolume(volume + 0.1f);
		}

		if (Common_BtnPress(BTN_DOWN))
		{
			float volume = Engine.GetVolume();
			Engine.SetVolume(volume - 0.1f);
		}

		std::chrono::duration<float> deltaTime = lastTime - currentTime;
		Engine.Update(deltaTime.count());

		{
			unsigned int ms = 0;
			unsigned int lenms = 0;
			bool		 paused = false;
			bool		 playing = Engine.IsPlaying();

			if(playing)
			{
				paused = Engine.GetPaused();
				ms = Engine.GetPosition();
				lenms = Engine.GetLength();
			}

			Common_Draw("==================================================");
			Common_Draw("Play Stream Example.");
			Common_Draw("Copyright (c) Firelight Technologies 2004-2021.");
			Common_Draw("==================================================");
			Common_Draw("");
			Common_Draw("Press %s to toggle pause", Common_BtnStr(BTN_ACTION1));
			Common_Draw("Press %s to unload", Common_BtnStr(BTN_ACTION2));
			Common_Draw("Press %s to increase volume", Common_BtnStr(BTN_UP));
			Common_Draw("Press %s to decrease volume", Common_BtnStr(BTN_DOWN));
			Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
			Common_Draw("");
			Common_Draw("Volume: %f", Engine.GetVolume());
			Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
			Common_Draw("");
		}

		lastTime = currentTime;
		Common_Sleep(50);
	} while (!Common_BtnPress(BTN_QUIT));
	
	Common_Close();

	return 0;
}