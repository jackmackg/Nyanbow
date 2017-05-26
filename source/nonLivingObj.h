#ifndef NYAN_OBJECTS_NONLIVING
#define NYAN_OBJECTS_NONLIVING

#include "baseGameObj.h"
#include <vector>
#include <string>

//
//wall obj, this is the main floor/Wall for the player
//
class ObjWall : public GameObj
{
	public:
		bool oneWay, canFall, slope; //oneWay is used for one way platforms and canFall is if the user can press down to pass through it

		ObjWall( int x, int y, int height, int width, nyanZones zone, bool oneWay, bool canFall, bool slope );
	
		std::vector<int>* saveFile();

		void draw();

		~ObjWall();
};


//used for parallax backgrounds
//can be animated
//can be locked on an axis
//can also be set to non tile (if image is larger than window for example)
//this uses data::tileSpriteBackground as its main texture
//
//if ratio is 1 and notile is true then image will be just a static image on screen (used for normal bg) 
class ObjBG : public GameObj
{
	//used to find delta of camara movment
	int camXlast, camYlast, height, width, texture;
	bool lockX, lockY, noTile, staticImage;
	
	nyanFileGraphic fileId;

	//used for saving data to file
	int saveFileId; 

	public:
		int fraction; //ratio
		
		//fileId is the id of the file name to look up to load the texture
		//ratio is how much slower it will move to the middle ground
		//lockX will not let it move on the x axis, lockY for y
		//width and height are the size of the texture (should be same as window size unless your not tiling)
		//frameRate is the speed to play animation if there is one
		//end frame is how many frames there are in the sheet
		//no tile does not warp tile
		//depth is the layer to draw the bg on
		ObjBG( nyanFileGraphic fileId, int ratio, bool lockX, bool lockY, bool noTile, nyanZones zone, int width, int height , int frameRate, int endFrame, int depth, int texture );

		//loads texture into video memory
		void zoneEntered();
		
		std::vector<int>* saveFile();

		void draw();

		~ObjBG();
};


//
//dialog system
//
//Give this obj a string with dialog formating options and it will show it to the player till the end
//simple system, no real built in branching linear only. supports expressions and showing of images
//Note: pressing the start + jump skips dialog
//Note: obj removes itself after its done
//Coomands are defined at the top of the CPP
//to make dev easy I am going to "hard code" the settings in the cpp with macros, I will move it to data file
class ObjDialog : public GameObj
{
	std::string dialog, dialogBuffer;

	int charPosStart, 
		charPosEnd, 
		dialogTimer, 
		expressionOffsetTemp, 
		storyImageOffsetTemp, 
		dialogSize, 
		strLengthTmp;

	bool * completed;
	bool waitForUser;

	void makeDisplayString();

	public:
		//dialog: is the formatted text to display to the user
		//completed: is a value you pass in, we will change it to true once the dialog is done
		ObjDialog( std::string dialog, bool *completed );

		void draw();
};


//
//objDamage
//
//causes damage when it collides with something it can hurt
//can be use for projectiles or melee
class ObjDamage : public GameObj
{
	int aliveTimer, damage;
	bool onlyHitPlayer;

	//ran when damage hits something
	void effectHit( GameObj * obj );

	public:
		//x/y hspeed/vspeed is the location/speed to spawn
		//type are preset types 
		//dir is the direction it will travel
		//onlyHitPlayer is set for a damage that should only hit the player
		ObjDamage( int x, int y, nyanDirection dir, nyanSpTag type, bool onlyHitPlayer );

		void stepEvent();

		void attackEvent();

		void doPhysics();

		void draw();
};

#endif
