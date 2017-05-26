#ifndef NDEBUG
	//#include <iostream>
	#include <SFML/System.hpp>
#endif

//homemade
#include "data.h" //must be loaded first
#include "tools.h"
#include "objMaster.h"
#include "baseGameObj.h"
#include "gameObjPlayer.h"
#include "nonLivingObj.h"
#include "npc.h"

//use SFML libs
#include "io.h"
#include "graphics.h"
//#include "audio.h"

#ifndef NDEBUG
	//outside game tools (still uses SFML libs)
	#include "imgToMap.h"
#endif


int main( int argc, char* argv[] )
{
	//get exe location to find resource files on windows
	//must be ran before loading resources
	#ifdef _WIN32
		std::string argv_str( argv[0] );
		DATA::startingLocation = argv_str.substr( 0, argv_str.find_last_of( "\\" ) ) + "\\";
	#endif

	//test code to test the image to map tool
	#ifndef NDEBUG
		//TOMAP::startConvert( DATA::startingLocation );
		//return 0;
	#endif

	//	Draw * testDraw  = new Draw( DATA::spritePlayer, 32, 32, 0, 0, 0 );
	//	testDraw->drawSetFrames( 0, 7, 30, -1 );
	//	Audio::loadAudio();
	//	Audio::playMusic( DATA::musicOne );
	//	Audio::playEffect( DATA::soundEffectOne );
	//	Audio::Stop();

	//test data setup
	Graphics::loadGraphics(); //load starting textures

	objMaster::update( nyanZones::zone01 ); //set starting zone
	objMaster::updateZone(); //do the work to set the zone

	objMaster::addFromFile( DATA::getDataFileName() ); //loading level objects

	//test objects
	//test 1 way plats
	//objMaster::add( new ObjWall( 100, 120, 20, 200, nyanZones::zone01, true, true ) ); //test this one way platfrom
	//objMaster::add( new ObjWall( 100, 200, 20, 200, nyanZones::zone01, true, true ) ); //test this one way platfrom

	////test dialog
	//bool done = false;
	//objMaster::add( new ObjDialog( "[I00Please read this first line!\n[E[I01[X01This is line 2 test. end bye!", &done ) );


	//test enemies
	//for (unsigned int i = 0; i < 3; ++i)
	//	objMaster::add( new ObjEnemies( nyanSpTag::spMonstTest, nyanZones::zone01, 300, 40 ) );


	//test triggers
	 //objMaster::add( new ObjTrigger( nyanSpTag::spTestTrigger, nyanZones::zone01, 100, 100, 100, 100, true ) );


	//varables used in main loop
	unsigned int i;

	//main game loop, space quits the game
	while ( !IO::keyExit() )
	{
		//update game timer
		TOOLS::timerUpdate();

		//step
		for ( i = 0; i < objMaster::inZone->size(); ++i )
			(*objMaster::inZone)[i]->stepEvent();

		objMaster::removeMidEvent(); //ran after an event to clean up items that remove themselves during the event
		objMaster::updateZone();     //check if a trigger updated the zone

		//attack
		for ( i = 0; i < objMaster::inZone->size(); ++i )
			(*objMaster::inZone)[i]->attackEvent();


		//dead check, what should you do if you got hit
		for ( i = 0; i < objMaster::inZone->size(); ++i )
			(*objMaster::inZone)[i]->deadCheck();
		
		objMaster::removeMidEvent();

		//physics
		for ( i = 0; i < objMaster::inZone->size(); ++i )
			(*objMaster::inZone)[i]->doPhysics();


		//get draw requests
		for ( unsigned int i = 0; i < objMaster::inZone->size(); ++i )
			(*objMaster::inZone)[i]->draw();


		//draw
		Graphics::updateCamara();
		Graphics::draw();

		
		#ifndef NDEBUG
			//slow down game for debug
			//sf::sleep(sf::milliseconds(250));
		#endif
	};

	return 0;
}
