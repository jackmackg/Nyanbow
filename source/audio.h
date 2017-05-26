#ifndef NYAN_AUDIO
#define NYAN_AUDIO

#include <SFML/Audio.hpp>

//this class uses the SFML library. it loads and manages the sound and music for the game

namespace Audio
{
	extern sf::SoundBuffer * soundEffects;
	extern sf::Sound * soundEffectsHolder; //is used with sound effects

	extern sf::Music * soundMusic;
	extern int musicIdLast;

	//must be ran first to load all the audio from the drive
	void loadAudio();

	//plays the sound effect of the given id
	//the sounds can play over one another
	//sound id is kept in data.h
	void playEffect( int soundId );
	
	//play the bgm, stops what was playing if anything was 
	//music data kept in data.h
	void playMusic( int musicId );

	//stops all music, not sound effects (may add that at another time)
	void Stop();

	//deletes all audio
	void removeAllAudio();
}

#endif
