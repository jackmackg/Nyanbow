#include <SFML/Audio.hpp>
#include "audio.h"
#include "data.h"

sf::SoundBuffer * Audio::soundEffects;
sf::Sound * Audio::soundEffectsHolder; //is used with sound effects

sf::Music * Audio::soundMusic;
int Audio::musicIdLast;

void Audio::loadAudio()
{	
	soundEffects = new sf::SoundBuffer[ DATA::soundEffectCount ];
	soundEffectsHolder = new sf::Sound[ DATA::musicCount ];

	soundMusic = new sf::Music[1];
	musicIdLast = 0; //holds last music was played so it can stop it

	//loading in sound effects
	
	//this is loading in one sound, must be added manually	
	//sound effect 1
	soundEffects      [ DATA::soundEffect01 ].loadFromFile( DATA::getSoundFileName( nyanFileSound::soundEffectTest ) ); 
	soundEffectsHolder[ DATA::soundEffect01 ].setBuffer   ( soundEffects[ DATA::soundEffect01 ] );


	//loading in music
	soundMusic[ DATA::music01 ].openFromFile(  DATA::getSoundFileName( nyanFileSound::soundEffectTest ) );
	soundMusic[ DATA::music01 ].setLoop( true );
}

void Audio::playEffect( int soundId )
{
	soundEffectsHolder[ soundId ].play();
}

void Audio::playMusic( int musicId )
{
	soundMusic[ musicIdLast ].stop();
	musicIdLast = musicId;

	soundMusic[ musicId ].play();
}

void Audio::Stop()
{
	soundMusic[ musicIdLast ].stop();
}

void Audio::removeAllAudio()
{
	delete soundEffects;
	delete soundEffectsHolder;
	delete soundMusic;
}
