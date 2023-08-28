#include "Song.hpp"

#define ERRCHECK(_result) Song::_errorCallback(_result, __FILE__, __LINE__)
ErrorCallback Song::_errorCallback = nullptr;

Song::Song(Implementation& implementation, FMOD::Sound* sound) :
	_implementation(implementation),
	_sound(sound),
	_channel(nullptr),
	_state(State::Init)
{
}

bool Song::IsPlaying()
{
	bool isPlaying = false;
	ERRCHECK(_channel->isPlaying(&isPlaying));
	return isPlaying;
}

bool Song::IsSoundLoaded()
{
	FMOD_OPENSTATE state;
	ERRCHECK(_sound->getOpenState(&state, nullptr, nullptr, nullptr));
	return state != FMOD_OPENSTATE_LOADING;
}

void Song::SetVolume(float volume)
{
	ERRCHECK(_channel->setVolume(volume));
}

/* Where I left off:
* watch the rest of the video for the state machine
* get the song to the point that it can reach the stopped state,
* and get cleared properly.
* goal: get the whole thing to the point where you can pass a file and two 
* numbers to it to play through example loop songs. So I can test out different
* UI kits for a front end and have a skeleton for having the hot-swaping system.
* Esentially it looks like you just need a way to schedule jumps. From one playing song to another (even if it's to a different point of the same song.)
*/
void Song::Update(float deltaTime)
{
	switch (_state)
	{
		case State::Init:
		case State::ToPlay:
			if (!IsSoundLoaded())
			{
				_state = State::Loading;
				return;
			}
			ERRCHECK(_implementation._system->playSound(
				_sound,
				_implementation._mainChannelGroup,
				false,
				&_channel
			));
			_state = State::Playing;
			break;
		case State::Loading:
			if (IsSoundLoaded())
			{
				_state = State::ToPlay;
			}
			break;
		case State::Playing:
			break;
		case State::Stopping:
			break;
		case State::Stopped:
			break;
		default:
			break;
	}
}