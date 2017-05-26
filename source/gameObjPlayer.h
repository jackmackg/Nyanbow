#ifndef NYAN_OBJECTS_PLAYER
#define NYAN_OBJECTS_PLAYER


#include "baseGameObj.h"

class ObjPlayer : public GameObj
{
	//jumping tools
	bool jumpedLastFrame;
	int  jumps;

	//trust
	int thrustState, thrustTimer, thrustingAngleSpeed;

	//these are the values that are given back when saving to file
	int saveX, saveY;

	//falling through platforms
	bool downKeyLastFrame;

	//holds where you where last frame, use for physics
	hitbox boxLastFramePhysics;

	//holds if you next to a wall ( onGround is for down )
	bool wallRight, wallLeft, wallTop; 

	//physics will set this value since its the only things that can move the player
	bool onGround;
	bool onSlopeLastFrame; //used to teleport you down as you go down a slope

	//one way platform code
	hitbox oneWayOn;   //holds what one way platform you where on if you where on it, dont need to set this value
	bool ignoreOneWay; //set true to ignore all oneWay platforms

	//freeze vaules
	//Note:	unFreezeOnce should be set to false at the start, timer needs to be set to zero also
	//Note: unFreezeOnce also lets your code know if freeze is on
	bool setGravity, setFriction, setHspeed, setVspeed, froze;
	float newHspeed, newVspeed;
	int freezeTimer;


	//attacking values
	int shootCoolDown;


	public:
		int debugOnce;
		bool canIO; //turn off user IO
		bool canTrust, canDoubleJump, canWallJump; //set player's abilities

		//since this is a one player game we will save it so other obj can reference it
		static ObjPlayer * player;

		ObjPlayer( int x, int y );
		
		void stepEvent();
		std::vector<int>* saveFile();
		void doPhysics();
		void draw();

		//stops the obj with many settings, setting gravity/friction will set these vaules to false till the timer
		//goes off. Same with hspeed and vspeed. setting hspeed/vspeed setting values to false will keep what value
		//they had.
		void freeze( int freezeTimer, bool setGravityFalse, bool setFrictionFalse, bool setHspeed, float newHspeed, bool setVspeed, float newVspeed );
		void unFreeze( bool now ); //used with freeze. It checks the timer set by freeze. You may override timer by settings now to true

		~ObjPlayer();

};

#endif
