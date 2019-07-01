#include "fmod.hpp"
#include "common.h"
#include "Mx3.hpp"

int FMOD_Main(int argc, char **argv)
{
	void *extradiverdata = 0;
	Mx3 *audio = new Mx3(32, FMOD_INIT_NORMAL, extradiverdata);

	Common_Init(&extradiverdata);



	return 0;
}