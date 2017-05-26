#include <string>
#include "data.h"

//sprites
//should be labed from sequentially
//these are the names of the texture buffers that images get loaded into
//in another location is the name to file name paired with a name
const int DATA::textureCount = 5;
const int DATA::sprite64 = 0;
const int DATA::spriteBackground     = 1;
const int DATA::spriteTileBackground = 2;
const int DATA::spriteForeground     = 3;
const int DATA::sprite128 = 4;

#ifndef NDEBUG
const int DATA::spriteNon = -1; //used if you don't want to draw anything and just use debug box
#endif 

//sounds/music
const int DATA::soundEffectCount = 1;
const int DATA::soundEffect01    = 0;

const int DATA::musicCount = 1;
const int DATA::music01    = 0;


//depths
const int DATA::depthUI = 2;
const int DATA::depthForeground01 =  1;
const int DATA::depthPlayer =  0;
const int DATA::depthMiddleGround = -1;
const int DATA::depthBackground01 = -2;
const int DATA::depthBackground02 = -3;

#ifndef NDEBUG
const int DATA::depthspriteDebug  = 100; //draws ontop of everything
#endif

//physics values
//hspeed
float DATA::gravity = 0.8f; //+ values mean down (keep gravity positive)
float DATA::maxFall = 13.0f;   //you can do more hspeed than this but this is the max for gravity
float DATA::maxFallWall = 2.0f;   //you can do more hspeed than this but this is the max for gravity
//vspeed
//friction (in %)
float DATA::frictionAir    = 0.01f;
float DATA::frictionGround = 0.10f;
float DATA::frictionMin    = 0.1f; //smallest friction can be (not a %)


//window settings
const std::string DATA::windowName = "Nyanbow";
const int DATA::windowWidth  = 480;
const int DATA::windowHeight = 270;
const int DATA::maxFrameRate = 60;
bool DATA::vsync = true;

//font settings
const int DATA::fontSize = 8;
const unsigned int DATA::fontColor = 0xBBBBBBFF; //light gray color with no alpha

//camara settings
//int DATA::camaraSpeed    = 15;
//int DATA::minDistance    = 130;
//int DATA::minCamaraSPeed = 3;
int DATA::CamX = 240; //how offset the camara will be from its main location ( I have it defaulted to center )
int DATA::CamY = 135 + 30;

//file folder locations
std::string DATA::startingLocation = "";

#ifdef _WIN32
	const std::string DATA::folGraphics = "graphics\\";
	const std::string DATA::folSound    = "sound\\";
	const std::string DATA::folData     = "data\\";
#elif __linux
	const std::string DATA::folGraphics = "graphics/";
	const std::string DATA::folSound    = "sound/";
	const std::string DATA::folData     = "data/";
#endif



//file names
//when adding a file you will need to add a new named+id and then add the data to the switch case
std::string DATA::getTextureFileName( nyanFileGraphic id )
{
	std::string loc = DATA::startingLocation + DATA::folGraphics;

	switch( id )
	{
		//The value of these case statements are defined at the top of this case statment, make sure they match
		case fileSprite64: //fileSprite64
			return loc + "spriteSheet64.png";
			break;

		case fileBackgroundTest: //backgroundTest
			return loc + "background.png";
			break;
		
		case fileTileBackgroundTest: //tileBackgroundTest
			return loc + "scrollBackground.png";
			break;

		case fileForegroundTest: //foregroundTest
			return loc + "foreground.png";
			break;

		case fileSprite128: //fileSprite128
			return loc + "spriteSheet128.png";
			break;
	}

	return "No image file name with that ID";
}

//sound
std::string DATA::getSoundFileName( nyanFileSound id )
{
	std::string loc = DATA::startingLocation + DATA::folSound;

	switch( id )
	{
		case soundEffectTest:
			return loc + "testSoundEffect.flac";
			break;

		case BgmTest:
			return loc + "musicTest.flac";
			break;
	}

	return "No sound file name with that ID";
}

//data files
std::string DATA::getDataFileName()
{
	return DATA::startingLocation + DATA::folData + "levelData";
}

//font file
std::string DATA::getFontFileName()
{
	return DATA::startingLocation + DATA::folGraphics + "PressStart2P-Regular.ttf";
}
