#ifndef NYAN_NPC
#define NYAN_NPC

#include "baseGameObj.h"

//all the monsters you can fight
class ObjEnemies : public GameObj
{

	//for saving functions
	int startingX, startingY;

	//how the enemy will move, ground or sky
	int moveType, speed;

	//used to set if active if player is close or something else like that
	bool active;

	//what to do when inactive and active
	void inactivate();
	void activate();


	public:

		ObjEnemies( nyanSpTag type, nyanZones zone, int x, int y );

		std::vector<int>* saveFile();

		void deadCheck();

		void doPhysics();

		void draw();

};

//
//triggers
//when the player is inside this object (or presses a key if set) it does a event
//todo: have a global save state that saves to file (hint use spTags)

//these enums are only used inside the trigger
//if its been activated
enum triggerStates
{
	triggerWaiting,  //waiting to be active
	triggerInEvent,  //doing something
	triggerNonActive //completed
};

//when its activated what state is the event in
enum triggerEvents
{
	//default value
	trigEvNon,

	//spTestTrigger
	//this event shows a test dialog message
	trigEvTestDialogWait
};

class ObjTrigger : public GameObj
{
	triggerStates state;
	triggerEvents eventState;

	bool keyActivated;

	//temp varables for events
	bool dialogDone;

	public:
		//when keyActivated the user has to press down to activate the trigger
		ObjTrigger( nyanSpTag type, nyanZones zone, int x, int y, int height, int width, bool keyActivated );

		void stepEvent();

		std::vector<int>* saveFile();

		void draw();
};


#endif