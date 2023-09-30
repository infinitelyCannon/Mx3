#include "common.h"
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include "Mx3.hpp"

int FMOD_Main()
{
	Mx3* Engine = nullptr;

	Common_Init(0);

	Engine = new Mx3(32, FMOD_INIT_NORMAL, 0);

	Engine->SetErrorCallback(ERRCHK_fn);

	Engine->PlaySound("C:\\Users\\Sigmund\\Desktop\\Temp\\Null.mp3");

	do
	{
		Common_Update();

		if (Common_BtnPress(BTN_ACTION1))
		{
			Engine->SetPaused(
				!Engine->GetPaused()
			);
		}

		if (Common_BtnPress(BTN_ACTION2))
		{
			Engine->Stop();
		}

		if (Common_BtnPress(BTN_ACTION3))
		{
			Engine->PlaySound("C:\\Users\\Sigmund\\Desktop\\Temp\\Null.mp3");
		}

		Engine->Update(0);

		Common_Draw("==================================================");
		Common_Draw("Mx3 Class Test");
		Common_Draw("Copyright (c) Firelight Technologies 2004-2021.");
		Common_Draw("==================================================");
		Common_Draw("");
		Common_Draw("Press %s to toggle pause", Common_BtnStr(BTN_ACTION1));
		Common_Draw("Press %s to stop", Common_BtnStr(BTN_ACTION2));
		Common_Draw("Press %s to restart", Common_BtnStr(BTN_ACTION3));
		Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
		Common_Draw("");

		Common_Sleep(50);
	} while (!Common_BtnPress(BTN_QUIT));

	delete Engine;
	Common_Close();

	return 0;
}