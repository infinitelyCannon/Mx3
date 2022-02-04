#include "common.h"
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include "Mx3.hpp"
#include "widgets/widgets.hpp"

#define TO_BTN(x) static_cast<Button>(x)

void ErrCallback(const char* msg, const char* file, int line)
{
	Common_Fatal("%s(%d): FMOD error - %s", file, line, msg);
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
	std::vector<std::string> Queue;
	std::string file;
	auto lastTime = std::chrono::steady_clock::now();
	auto currentTime = lastTime;
	WidgetState wState = WidgetState::Main;
	Tabs mTabs;
	Playlist wQueue;

	Common_Init(0);

	LoadQueue(Queue);
	wQueue.AddSongs(Queue);
	mTabs.AddWidget("Playlist", &wQueue);

	Mx3::SetErrorCallback(ErrCallback);
	Mx3 Engine(32,FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, 0);

	Engine.AddToQueue(Queue);
	Engine.Play(1);
	lastTime = std::chrono::steady_clock::now();

	Engine.SetVolume(0.4f);

	do
	{
		currentTime = std::chrono::steady_clock::now();

		Common_Update();

		if(Common_BtnPress(BTN_ACTION1))
		{
			switch (wState)
			{
				case WidgetState::Main:
				{
					bool paused = Engine.GetPaused();
					Engine.SetPaused(!paused);
				}
					break;
				case WidgetState::Tabs:
					Engine.Play(wQueue.GetIndex());
					break;
				case WidgetState::Blank:
					break;
				default:
					break;
			}
			
		}

		if (Common_BtnPress(BTN_ACTION2))
		{
			switch (wState)
			{
				case WidgetState::Main:
					Engine.SwapSongs(1, 4);
					wQueue.SwapSongs(1, 4);
					break;
				case WidgetState::Tabs:
					Engine.RemoveFromQueue(wQueue.GetIndex());
					wQueue.RemoveSong(wQueue.GetIndex());
					break;
				case WidgetState::Blank:
					break;
				default:
					break;
			}
		}

		if (Common_BtnPress(BTN_ACTION3))
		{
			switch (wState)
			{
				case WidgetState::Tabs:
					Engine.ClearQueue();
					wQueue.ClearList();
					break;
				default:
					break;
			}
		}

		if (Common_BtnPress(BTN_ACTION4))
		{
			switch (wState)
			{
				case WidgetState::Tabs:
					LoadQueue(Queue);
					Engine.ClearQueue();
					Engine.AddToQueue(Queue);
					wQueue.ClearList();
					wQueue.AddSongs(Queue);
					Engine.Play(0);
					break;
				default:
					break;
			}
		}

		if(Common_BtnPress(BTN_UP))
		{
			switch (wState)
			{
				case WidgetState::Main:
					Engine.SetVolume(
						Engine.GetVolume() + 0.1f
					);
					break;
				case WidgetState::Tabs:
					mTabs.OnButton(TO_BTN(BTN_UP));
					break;
				case WidgetState::Blank:
					break;
				default:
					break;
			}
			
		}

		if(Common_BtnPress(BTN_DOWN))
		{
			switch (wState)
			{
				case WidgetState::Main:
					Engine.SetVolume(
						Engine.GetVolume() - 0.1f
					);
					break;
				case WidgetState::Tabs:
					mTabs.OnButton(TO_BTN(BTN_DOWN));
					break;
				case WidgetState::Blank:
					break;
				default:
					break;
			}
			
		}

		if(Common_BtnPress(BTN_LEFT))
		{
			switch (wState)
			{
				case WidgetState::Main:
					Engine.Previous();
					break;
				case WidgetState::Tabs:
					mTabs.OnButton(TO_BTN(BTN_LEFT));
					break;
				case WidgetState::Blank:
					break;
				default:
					break;
			}
			
		}		
		
		if(Common_BtnPress(BTN_RIGHT))
		{
			switch (wState)
			{
				case WidgetState::Main:
					Engine.Next();
					break;
				case WidgetState::Tabs:
					mTabs.OnButton(TO_BTN(BTN_RIGHT));
					break;
				case WidgetState::Blank:
					break;
				default:
					break;
			}
		}

		if (Common_BtnPress(BTN_MORE))
		{
			switch (wState)
			{
				case WidgetState::Main:
					wState = WidgetState::Tabs;
					break;
				case WidgetState::Tabs:
					wState = WidgetState::Main;
					break;
				case WidgetState::Blank:
					break;
				default:
					break;
			}
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
			Common_Draw("Press %s to swap 1 with 4", Common_BtnStr(BTN_ACTION2));
			Common_Draw("Press %s to raise volume", Common_BtnStr(BTN_UP));
			Common_Draw("Press %s to lower volume", Common_BtnStr(BTN_DOWN));
			Common_Draw("Press %s to skip back", Common_BtnStr(BTN_LEFT));
			Common_Draw("Press %s to skip forward", Common_BtnStr(BTN_RIGHT));
			Common_Draw("Press %s to change modes", Common_BtnStr(BTN_MORE));
			Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
			Common_Draw("");
			Common_Draw("Mode: %s", Widget::PrintState(wState));
			Common_Draw("Volume: %f", Engine.GetVolume());
			Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
			Common_Draw("");
			Common_Draw(mTabs.Print().c_str());
		}

		lastTime = currentTime;
		Common_Sleep(50);
	} while (!Common_BtnPress(BTN_QUIT));
	
	Common_Close();

	return 0;
}