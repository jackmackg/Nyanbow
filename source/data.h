#ifndef NYAN_DATA
#define NYAN_DATA

#include <string>

//values that will be used in the programe will be defined here
//this is used to hold otherwise hardcoded values
//this holds all the data that’s shared between objects. I have decided objects only values will be in the objects definition

//NOTE: this needs to be loaded first as all other things depend on its value!
enum nyanDirection
{
	up,
	down,
	left,
	right
};

enum nyanTag
{
	player,
	ground,
	meta,
	damage,
	enemies,
	triggers
};

enum nyanSpTag
{
	spPlayer,
	spFloor,
	spBg,
	spDialog,

	//damage types
	spDmgFishMelee,

	//enemies
	spMonstTest,

	//triggers
	spTestTrigger
};

enum nyanFileGraphic
{
	fileSprite64,
	fileSprite128,
	fileBackgroundTest,
	fileTileBackgroundTest,
	fileForegroundTest
};

enum nyanFileSound
{
	soundEffectTest,
	BgmTest
};

enum nyanZones
{
	zoneRemoveOnSwitch, 
	zoneAll, 
	zone01, 
	zone02
};

namespace DATA 
{
	//texture lables
	//these are the names of buffer of where textures are stored
	extern const int textureCount, sprite64, spriteBackground, spriteTileBackground, spriteForeground, sprite128;

	#ifndef NDEBUG
	extern const int spriteNon;
	#endif

	//audio lables
	//these are the names of the buffers and their size of audio is stored
	extern const int soundEffectCount, musicCount;
	extern const int soundEffect01;
	extern const int music01;

	//depths
	//these are the standard depths
	extern const int depthMiddleGround, depthBackground01, depthBackground02, depthForeground01, depthUI, depthPlayer;

	#ifndef NDEBUG
	extern const int depthspriteDebug;
	#endif

	//physics data
	extern float gravity, maxFall, maxFallWall, frictionAir, frictionGround, frictionMin;

	//window settings
	extern const int windowHeight, windowWidth, maxFrameRate;
	extern const std::string windowName;
	extern bool vsync;

	//in game font settings
	extern const int fontSize; 
	extern const unsigned int fontColor;

	//camara settings
	extern int camaraSpeed, minDistance, minCamaraSPeed, CamX, CamY;

	//
	//file names
	//
	//where to start looking for resources
	//not a const, needs to be set at run time
	extern std::string startingLocation;

	//folder locations
	extern const std::string folGraphics, folSound, folData;

	//textures
	std::string getTextureFileName( nyanFileGraphic id );

	//audio
	std::string getSoundFileName( nyanFileSound id );

	//data files
	std::string getDataFileName();

	//font file
	std::string getFontFileName();

}

#endif
