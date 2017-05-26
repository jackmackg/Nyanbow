#include "nonLivingObj.h"
#include "data.h"
#include "graphics.h"
#include "objMaster.h"
#include "io.h"
#include "gameObjPlayer.h"

#include <stdlib.h>
#include <math.h>
#include <vector>


//====
//wall
//====
ObjWall::ObjWall( int x, int y, int height, int width, nyanZones zone, bool oneWay, bool canFall, bool slope )
{
	setDefaults();

	this -> box.setX( x );
	this -> box.setY( y );
	this -> box.setHeight( height );
	this -> box.setWidth ( width );
	this -> zone    = zone;
	this -> oneWay  = oneWay;
	this -> canFall = canFall;
	this -> slope	= slope;

	tag     = nyanTag::ground;
	spTag   = nyanSpTag::spFloor;
	solid   = true;
	physics = false;
	

	#ifndef NDEBUG
	//test box so i can see the walls
	sprites = new std::vector<Draw*>;
	sprites->push_back( new Draw( oneWay ? 3 : 1, x, y, height, width, true ) );

	//used so I can toggle this lator on
	visible = false;
	#endif
}

std::vector<int>* ObjWall::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 
	
	items->push_back( spTag );

	items->push_back( box.x );
	items->push_back( box.y );

	items->push_back( box.height );
	items->push_back( box.width );

	items->push_back( zone );
	items->push_back( oneWay );
	items->push_back( canFall );
	items->push_back( slope );

	return items;
}

void ObjWall::draw()
{
	#ifndef NDEBUG
	if ( !visible ) return;

	sprites->at(0)->width  = box.width;
	sprites->at(0)->height = box.height;
	sprites->at(0)->x = box.x;
	sprites->at(0)->y = box.y;
	Graphics::addDraw( sprites->at(0) );
	#endif
}

ObjWall::~ObjWall()
{
		deleteDraw();
}


//==========
//background
//==========
ObjBG::ObjBG( nyanFileGraphic fileId, int ratio, bool lockX, bool lockY, bool noTile, nyanZones zone, int width, int height, int frameRate, int endFrame, int depth, int texture )
{	
	setDefaults();

	fraction = ratio; //ration of bg movement to camara 

	//things to save to file
	saveFileId = fileId;

	//grab camara starting to find delta later
	camXlast = Graphics::offsetX;
	camYlast = Graphics::offsetY;
	
	physics = false;

	this->tag   = nyanTag::meta;
	this->spTag = nyanSpTag::spBg;
	this->zone  = zone;

	this->lockX = lockX;
	this->lockY = lockY;
	
	//get the size of the texture
	this->width  = width; 
	this->height = height;	

	this->noTile = noTile;
	this->fileId = fileId;

	this->staticImage = ( ratio == 1 && noTile ); 

	this->texture = texture;

	sprites = new std::vector<Draw*>;
	sprites->push_back( new Draw( texture, width, height, depth, 0, 0, staticImage ) );

	//settings up starting tile location and animation
	sprites->at(0)->x = 0;
	sprites->at(0)->y = 0;
	sprites->at(0)->drawSetFrames( 0, endFrame, frameRate, 1, false );

	//setup if you do tile backgrounds
	if ( !noTile )
	{
		sprites->push_back( new Draw( texture, width, height, depth, 0, 0, staticImage ) );
		sprites->push_back( new Draw( texture, width, height, depth, 0, 0, staticImage ) );
		sprites->push_back( new Draw( texture, width, height, depth, 0, 0, staticImage ) );

		sprites->at( 1 )->drawSetFrames( 0, endFrame, frameRate, 1, false );
		sprites->at( 2 )->drawSetFrames( 0, endFrame, frameRate, 1, false );
		sprites->at( 3 )->drawSetFrames( 0, endFrame, frameRate, 1, false );
	}
	
	zoneEntered(); //to load in the texture
}

void ObjBG::zoneEntered()
{
	Graphics::swapTexture( DATA::getTextureFileName( fileId ), texture );
}

std::vector<int>* ObjBG::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 

	items->push_back( spTag );
	
	items->push_back( saveFileId );
	items->push_back( fraction );

	items->push_back( lockX );
	items->push_back( lockY );

	items->push_back( noTile );
	items->push_back( zone );

	items->push_back( sprites->at(0)->width );
	items->push_back( sprites->at(0)->height );

	items->push_back( sprites->at(0)->delay );
	items->push_back( sprites->at(0)->endFrame );

	items->push_back( sprites->at(0)->depth );
	items->push_back( texture );	

	return items;
}

void ObjBG::draw()
{
	if ( !staticImage )
	{
		int deltaX, deltaY;
		
		Draw* tempDraw = sprites->at( 0 );
		int winx = DATA::windowWidth;
		int winy = DATA::windowHeight;

		int deltaX2 = 0;
		int deltaY2 = 0;

		//x and y of the tile we are warping
		int x = sprites->at( 0 )->x;
		int y = sprites->at( 0 )->y; 

		//find the delta in the camara movment
		deltaX = Graphics::offsetX - camXlast;
		deltaY = Graphics::offsetY - camYlast;


		//find out how much to move the bg
		if ( abs( deltaX ) > fraction && !lockX )
		{
			deltaX2 -= (int)copysign( deltaX / fraction, deltaX );	
			camXlast = Graphics::offsetX;	
		}


		if( abs( deltaY ) > fraction && !lockY )
		{
			deltaY2 -= (int)copysign( deltaY / fraction, deltaY );
			camYlast = Graphics::offsetY;	
		}


		//
		//move main tile and warp it
		//
		//move the bg
		x += deltaX2;
		y += deltaY2;
		
		//warp tile if out of view
		if ( !noTile )
		{
			//Here i'm just naming all the items to make it less confusing
			int tileLeft  = x;
			int tileRight = x + width;

			int tileTop    = y;
			int tileBottom = y + height;

			int winLeft  = 0;
			int winRight = DATA::windowWidth;

			int winTop    = 0;
			int winBottom = DATA::windowHeight;

			if ( tileLeft  <= winLeft  ) x = winRight - 1;
			if ( tileRight >= winRight ) x = winLeft  + 1;

			if ( tileTop    <= winTop    ) y = winBottom - 1;
			if ( tileBottom >= winBottom ) y = winTop    + 1;
		}
		//move main tile
		sprites->at( 0 )->x = x;
		sprites->at( 0 )->y = y;

		//
		//move the other tile to the correct locaion based off warp tile
		//
		//x axis
		if ( !noTile )
		{
			if ( tempDraw->x + winx >= winx )
			{
				sprites->at( 1 )->x = tempDraw->x - winx;
				sprites->at( 3 )->x = tempDraw->x - winx;
			}
			else
			{
				sprites->at( 1 )->x = tempDraw->x + winx;
				sprites->at( 3 )->x = tempDraw->x + winx;
			}
			
			sprites->at( 1 )->y = tempDraw->y;


			//y axis
			if ( tempDraw->y + winy >= winy )
			{
				sprites->at( 2 )->y = tempDraw->y - winy;
				sprites->at( 3 )->y = tempDraw->y-winy;
			}
			else
			{
				sprites->at( 2 )->y = tempDraw->y + winy;
				sprites->at( 3 )->y = tempDraw->y + winy;
			}
			
			sprites->at( 2 )->x = tempDraw->x;

			//draw the tiles
			sprites->at( 3 )->drawUpdate();
			sprites->at( 1 )->drawUpdate();
			sprites->at( 2 )->drawUpdate();
			Graphics::addDraw( sprites->at( 3 ) );
			Graphics::addDraw( sprites->at( 1 ) );
			Graphics::addDraw( sprites->at( 2 ) );
		}
	}

	//draw base texture
	sprites->at( 0 )->drawUpdate();
	Graphics::addDraw( sprites->at( 0 ) );
}

ObjBG::~ObjBG()
{
	deleteDraw();
}


//======
//dialog
//======

//general settings
#define DIALOGDEPTH DATA::depthUI
#define DIALOGTIMEKEYPAUSE 30
#define MAXCHARSONSCREEN   300

//draw text
#define DIALOGTEXTX 88
#define DIALOGTEXTY 202
#define DIALOGTEXTSPRITEID 0

//expression icon
#define texturetExpressionID DATA::sprite64
#define expressionOffset 10
#define TEXTURESIZEEXP 64
#define DIALOGEXPRX 11
#define DIALOGEXPRY 198
#define DIALOGEXPRESSPRITEID 1

//text background
#define texturetBoxID DATA::sprite64
#define DIALOGBOXW 384
#define DIALOGBOXH 64
#define DIALOGBOXX 82
#define DIALOGBOXY 198
#define DIALOGBOXTEXTUREX 128
#define DIALOGBOXTEXTUREY 64
#define DIALOGBOXSPRITEID 2

//story image
#define texturetStoryImage DATA::sprite128
#define storyImageOffset 0
#define TEXTURESIZESTORY 128
#define STORYIMGX 176
#define STORYIMGY 36
#define STORYIMAGESPRITEID 3
#define HIDESTORYIMAGE -1 //set storyImageOffsetTemp to this to keep it from drawing

//commands
#define COMMANDSTART    '['  //how all commands start
#define COMMANDWAIT     "[E" //stop and wait for user to hit a key to show more text
#define COMMANDEXPRESS  "[X" //[X## (# meaning a number) of the offset for the expression graphic. Must be 2 numbers (01 for 1), 99 is max. graphics stays on screen until changed
#define COMMANDSTORYIMG "[I" //[I## same as above but for story images, story images only last the line they are started on, user input ends it

ObjDialog::ObjDialog( std::string dialog, bool *completed )
{
	setDefaults();

	this->completed = completed;
	this->dialog    = dialog;

	dialogSize = dialog.size();

	tag  = nyanTag::meta;
	spTag  = nyanSpTag::spDialog;
	zone = nyanZones::zoneRemoveOnSwitch;

	//zero out
	charPosStart = 0;
	charPosEnd   = 0;
	dialogTimer  = 0;
	expressionOffsetTemp = 0;
	storyImageOffsetTemp = HIDESTORYIMAGE;

	dialogBuffer = "";
	waitForUser  = false;

	//must have push_back in this order to match their macros ids
	sprites = new std::vector<Draw*>;
	//text id:0
	sprites->push_back( new Draw( "", DIALOGDEPTH, DIALOGTEXTX, DIALOGTEXTY, DATA::fontColor, DATA::fontSize ) );
	//expression id:1
	sprites->push_back( new Draw( texturetExpressionID, TEXTURESIZEEXP, TEXTURESIZEEXP, DIALOGDEPTH, DIALOGEXPRX, DIALOGEXPRY, false ) );
	//box id:2
	sprites->push_back( new Draw( texturetBoxID, DIALOGBOXW, DIALOGBOXH, DIALOGDEPTH, DIALOGBOXX, DIALOGBOXY, false, DIALOGBOXTEXTUREX, DIALOGBOXTEXTUREY ) );
	//story image id:3
	sprites->push_back( new Draw( texturetStoryImage, TEXTURESIZESTORY, TEXTURESIZESTORY, DIALOGDEPTH, STORYIMGX, STORYIMGY, false ) );


}

void ObjDialog::makeDisplayString()
{
	//makes dialogBuffer the string we wish to show and waits for user inputs

	//make sure we have text to process
	if ( dialogSize == 0 )
	{
		*completed = true;
		return;
	}

	//wait for user input
	if ( waitForUser && TOOLS::timerPassed( dialogTimer ) )
	{
		if ( IO::keyShoot() )
		{
			if ( charPosEnd >= dialogSize )
			{
				*completed = true;
				return;
			}
			else
			{
				//move start to end and start all over again
				charPosStart = charPosEnd;
				waitForUser = false;
				dialogTimer = TOOLS::timerSet( DIALOGTIMEKEYPAUSE );
				storyImageOffsetTemp = HIDESTORYIMAGE; //hide story image
			}
				
		}
		else //do nothing until user presses a key
			return;
	}

	//copy the part of the string we wish to display
	strLengthTmp = ( charPosEnd - charPosStart );
	dialogBuffer = dialog.substr( charPosStart,  strLengthTmp + 1 );

	//filter tags out of display
	int index;
	index = dialogBuffer.find( COMMANDEXPRESS );
	if ( index != std::string::npos )
		dialogBuffer.replace( index, 4, "" );

	index = dialogBuffer.find( COMMANDSTORYIMG );
	if ( index != std::string::npos )
		dialogBuffer.replace( index, 4, "" );


	//check display length position
	if ( charPosEnd >= dialogSize || strLengthTmp >= MAXCHARSONSCREEN )
		waitForUser = true;
	else
	{
		//main tag parsing
		std::string tempCmd = dialog.substr( charPosEnd, 2 );

		//parse all commands before moving on
		while( dialog.at( charPosEnd ) == COMMANDSTART )
		{
			//=set expression offset==
			if ( tempCmd == COMMANDEXPRESS )
			{
				//grab next 2 digits for the offset from the string
				expressionOffsetTemp = atoi( dialog.substr( charPosEnd + 2, 2 ).c_str() );
				charPosEnd += 4;
			}
			else //=set story image offset==
			if ( tempCmd == COMMANDSTORYIMG )
			{
				storyImageOffsetTemp = atoi( dialog.substr( charPosEnd + 2, 2 ).c_str() );
				charPosEnd += 4;
			}
		}
		//end of tag parsing

		//move tail out to give the one character at a time look checking for overflow, end if overflow
		if ( ++charPosEnd >= dialogSize )
		{
			waitForUser = true;
			return;
		}

		//tag parsing end
		//used for wait user tag
		if ( dialog.at(charPosEnd) == COMMANDSTART )
		{
			tempCmd = dialog.substr( charPosEnd, 2 );

			//==stop printing text and wait for the user==
			if ( tempCmd == COMMANDWAIT )
			{
				waitForUser = true;
				charPosEnd += 2; //skip over tag
			}
		}
		//end of tag parsing
	}

}

void ObjDialog::draw()
{
	makeDisplayString();

	//exit if you are done or skip dialog keys are pressed
	if ( *completed || ( IO::keyStart() && IO::keyJump() ) )
	{
		//deleteDraw(); this is now done in the live below
		objMaster::remove( this ); //remove yourself from the list
		return;
	}


	//draw story image
	if ( storyImageOffsetTemp != HIDESTORYIMAGE )
	{
		sprites->at( STORYIMAGESPRITEID )->drawSetFrames( storyImageOffset + storyImageOffsetTemp, storyImageOffset + storyImageOffsetTemp, 0, 0, false );
		Graphics::addDraw( sprites->at( STORYIMAGESPRITEID ) );
	}
	
	//draw expression
	sprites->at( DIALOGEXPRESSPRITEID )->drawSetFrames( expressionOffset + expressionOffsetTemp, expressionOffset + expressionOffsetTemp, 0, 0, false );
	Graphics::addDraw( sprites->at( DIALOGEXPRESSPRITEID ) );

	//draw text ( must be before dialog box )
	sprites->at( DIALOGTEXTSPRITEID )->text = dialogBuffer;
	Graphics::addDraw( sprites->at( DIALOGTEXTSPRITEID ) );

	//draw dialog box
	Graphics::addDraw( sprites->at( DIALOGBOXSPRITEID ) );

}


//
//Damage
//
//this has hard coded values in it but they are only used inside the object

ObjDamage::ObjDamage( int x, int y, nyanDirection dir, nyanSpTag type, bool onlyHitPlayer )
{
	//I left out default function as i want this class to be lean
	this->onlyHitPlayer = onlyHitPlayer;
	this->dirFacing = dir;

	box.setX( x );
	box.setY( y );

	//some defaults
	zone  = nyanZones::zoneRemoveOnSwitch;
	tag   = nyanTag::damage;
	spTag = type;
	solid = false;

	//just incase these things done get set
	aliveTimer = 0;
	damage	   = 0;

	//used for constant speed projectiles, must be positive
	float speed = 0;

	//graphics
	sprites = new std::vector<Draw*>;

	//get attack data
	if ( type == nyanSpTag::spDmgFishMelee )
	{
		speed  = 1;
		damage = 2;
		aliveTimer = TOOLS::timerSet(90);
		box.setWidth (10);
		box.setHeight(10);
	}
	#ifndef NDEBUG
	else
	{
		//type not known
		exit(1);
	}
	#endif

	//setup direction
	switch ( dir )
	{
		case nyanDirection::up:
			hspeed = 0;
			vspeed = -speed;
			box.setYRel( -box.height ); //move box up for a better offset (same with x below) ( assuming you want the back of the object to face coordinates given )
			break;

		case nyanDirection::down:
			hspeed = 0;
			vspeed = speed;
			break;

		case nyanDirection::left:
			hspeed = -speed;
			vspeed = 0;
			box.setXRel( -box.width );
			break;

		case nyanDirection::right:
			hspeed = speed;
			vspeed = 0;
			break;

		#ifndef NDEBUG
		default:
			//direction not know
			exit( 1 );
			break;
		#endif
	}


	//test graphics
	#ifndef NDEBUG
		visible = true;
		sprites->push_back( new Draw( 1, x, y, box.height, box.width, true ) );
	#endif
	
}

void ObjDamage::stepEvent()
{
}

void ObjDamage::attackEvent()
{
	//check if you hit anything
	GameObj * tempObj;
	int size;

	//this is for damage directed at the enemies
	if ( !onlyHitPlayer )
	{
		//loop though all the things the player can hit
		size = objMaster::damageList->size();

		for ( int i = 0; i < size; ++i )
		{
			tempObj = (* objMaster::damageList)[i];

			//check collision
			if ( box.intercepts( tempObj->box ) )
			{
				effectHit( tempObj );      //take damage
				objMaster::remove( this ); //remove self
				return;
			}
		}

	}
	else //this is for a damage thats directed at the player
	{
		if ( box.intercepts( ObjPlayer::player->box ) )
		{
			effectHit( ObjPlayer::player );
			objMaster::remove( this );
			return;
		}
	}

	//remove if you hit a wall but miss everything else
	size = objMaster::solidList->size();

	for ( int i = 0; i < size; ++i )
	{
		tempObj = ( *objMaster::solidList )[ i ];

		//check collision
		if ( box.intercepts( tempObj->box ) )
		{
			objMaster::remove( this ); //remove self
			return;
		}
	}


	//remove self if timer passes
	if ( TOOLS::timerPassed( aliveTimer ) )
	{ 
		objMaster::remove( this );
		return;
	}
		
}

void ObjDamage::effectHit( GameObj * obj )
{
	obj->hp -= damage;
}

void ObjDamage::doPhysics()
{
	//simplified physics
	box.setXRel( (int)hspeed );
	box.setYRel( (int)vspeed );
}

void ObjDamage::draw()
{
	//draw a rec around hitbox
	#ifndef NDEBUG
		if (!visible) return;

		sprites->at(0)->width = box.width;
		sprites->at(0)->height = box.height;
		sprites->at(0)->x = box.x;
		sprites->at(0)->y = box.y;
		Graphics::addDraw(sprites->at(0));
	#endif
}