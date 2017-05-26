#ifndef NYAN_OBJECTS
#define NYAN_OBJECTS
#include "tools.h"
#include <vector>

//main objects all other callses will be based off of
//Note: when you make a new object you may want to add how to spawn it from file in objMaster.cpp
//Note: Look at setDefaults() to see what values you need!
class GameObj
{
	public: //Note! I think all values will need to be set unless you know for sure not too
		nyanTag   tag;   //general tag
		nyanSpTag spTag; //A second tag thats more specific than the first
		nyanZones zone;  //holds what zone the object is in

		float hspeed, vspeed; //hspeedLast used for find acceleration
		float maxHSpeed;      //make hspeed you can move youself, keep this positive
		hitbox box;           //main hitbox of the obj, used in the physics
		bool physics, gravity, friction; //switches for phyics and its items
		bool solid;           //if the object has a draw event and if it has collision


		bool visible;   //for personal record, draw code does not use this
		int hp;

		bool meta;      //if you are a meta object, only meta steps will be ran for you meta

		int dirFacing;               //direction facing
		std::vector<Draw*> *sprites; //holds all the graphics for the object
		Draw* mainSprite; //holds pointer to main sprite that will be drawn, does not need to be used


		GameObj();

		//default all values, should be ran at the start of the constructor before anything else
		void setDefaults();
			
		//these are events that will be ran in order from top to bottom
		virtual void stepEvent();    //IO and AI is processed here
		virtual void attackEvent();  //attacking code next
		//virtual void metaStep();     //used for metaObjects so they can to their processing
		virtual void deadCheck();    //code to check if you died and what to do if you where (things that attack can kill gets removed here)
		virtual void doPhysics();    //runs platforming physics, also sets onground 
		virtual void draw();         //this is also ran for meta objects, sets up draw objects to be drawn

		//events that fire when appropriate
		virtual void zoneEntered(); //when objMaster updates to your zone
		virtual void zoneExit();    //when the zone has changed away from yours

		//io
		//
		//objmaster takes the vector and saves it to a file
		//this is used for saving the objects in the level editor
		virtual std::vector<int>* saveFile();

		//tools
		bool directionMoving( nyanDirection directionToCheck );     //check if your moving a direction
		void hspeedAdderWithLimter( float acceleration ); //if you want to add speed but not go over your adding limit
		void deleteDraw(); //removes all sprite data. should be used when deleting an item.
		
		//Different ways to search though the game objects:wa
		GameObj* returnFirstCdTag( hitbox hit, int tag ); //returns first obj seen with a general tag

		virtual ~GameObj();
};

#endif
