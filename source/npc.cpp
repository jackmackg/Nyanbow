#include <stdlib.h>

#include "npc.h"
#include "gameObjPlayer.h"
#include "objMaster.h"
#include "graphics.h"
#include "nonLivingObj.h"
#include "io.h"

//movement type
#define MOVEGROUND 0
#define MOVEAIR 1

//
//ObjEnemies
//

ObjEnemies::ObjEnemies( nyanSpTag type, nyanZones zone, int x, int y )
{
	setDefaults();

	startingX = x;
	startingY = y;

	this->zone = zone;
	this->tag  = nyanTag::enemies;
	activate();
	spTag  = type;
	
	//make enemy face wherever
	dirFacing = ( rand() % 2 == 0 ) ? nyanDirection::right : nyanDirection::left;

	box.setX( x );
	box.setY( y );
	

	switch( spTag )
	{
		case nyanSpTag::spMonstTest:
			box.setWidth ( 15 );
			box.setHeight( 30 );
			moveType = MOVEGROUND;
			speed = 1;
			hp = 8;
		break;

		#ifndef NDEBUG
		default:
			//enemy not known
			exit( 1 );
			break;
		#endif
	}

	#ifndef NDEBUG
	//test box so i can see the enemies
	sprites = new std::vector<Draw*>;
	sprites->push_back( new Draw( 2, x, y, box.height, box.width, true ) );

	//used so I can toggle this lator on
	visible = true;
	#endif
}

void ObjEnemies::inactivate()
{
	active = false;
}

void ObjEnemies::activate()
{
	active = true;
}

std::vector<int>* ObjEnemies::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 

	items->push_back( tag );

	items->push_back( spTag );
	items->push_back( zone );
	items->push_back( startingX );
	items->push_back( startingY );

	return items;
}

void ObjEnemies::deadCheck()
{
	if ( hp < 1 ) objMaster::remove( this );
}

void ObjEnemies::doPhysics()
{
	if ( !active ) return;

	//move in the dirFacing
	hspeed = (float)( dirFacing == nyanDirection::left ? -speed : speed );
	box.setXRel( (int)hspeed ); 

	//check collision

	//hspeed
	//if hit a wall move other direction
	int i, size = objMaster::solidList->size();
	GameObj * tempObj;

	for( i = 0; i < size; ++i )
	{
		tempObj = ( *objMaster::solidList )[i];

		if ( this != tempObj && box.intercepts( tempObj->box ) )
		{
			if ( directionMoving( nyanDirection::left ) )
			{
				dirFacing = nyanDirection::right;
				box.rightOf( tempObj->box );
			}
			else //right
			{
				dirFacing = nyanDirection::left;
				box.leftOf( tempObj->box );
			}
		}
	}


	//vspeed
	if ( vspeed < DATA::maxFall )
		vspeed += DATA::gravity;

	box.setYRel( ( int )ceil( vspeed ) );

	for( i = 0; i < size; ++i )
	{
		tempObj = ( *objMaster::solidList )[i];

		if ( this != tempObj && box.intercepts( tempObj->box ) )
		{
			if ( directionMoving( nyanDirection::up ) )
			{
				vspeed = 0;
				box.bottomOf( tempObj->box );
			}
			else //down
			{
				vspeed = 0;
				box.topOf( tempObj->box );

				//if about to fall off edge then turn around
				if ( box.x <= tempObj->box.x )
					dirFacing = nyanDirection::right;
				else
					if ( box.right >= tempObj->box.right )
						dirFacing = nyanDirection::left;
			}
		}
	}



}

void ObjEnemies::draw()
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


//
//ObjTrigger
//
ObjTrigger::ObjTrigger( nyanSpTag type, nyanZones zone, int x, int y, int height, int width, bool keyActivated )
{
	setDefaults();

	box.setX( x );
	box.setY( y );
	box.setWidth ( width  );
	box.setHeight( height );

	this->zone = zone;
	this->keyActivated = keyActivated;
	spTag = type;
	tag   = nyanTag::triggers;

	state      = triggerStates::triggerWaiting;
	eventState = triggerEvents::trigEvNon;

	
	#ifndef NDEBUG
		//test sprite
		sprites = new std::vector<Draw*>;
		sprites->push_back( new Draw( 4, x, y, box.height, box.width, true ) );

		//used so I can toggle this lator on
		visible = true;
	#endif

}

void ObjTrigger::stepEvent()
{
	//activate
	if ( state == triggerStates::triggerWaiting && box.intercepts( ObjPlayer::player->box ) )
	{
		if ( keyActivated )
			if ( IO::keyDown() ) 
				state = triggerStates::triggerInEvent;
		else
			state = triggerStates::triggerInEvent;
	}

	if ( keyActivated && IO::keyDown() )

	//dont move on unless you are in an event
	if ( state != triggerStates::triggerInEvent ) return;


	//decode what to do when active
	if ( spTag == nyanSpTag::spTestTrigger )
	{
		//
		//test dialog start
		//
		switch ( eventState )
		{
			case triggerEvents::trigEvNon:

				//make a dialog and then set event to wait on it
				dialogDone = false;

				//make it so the player cant move
				ObjPlayer::player->canIO = false; 
				ObjPlayer::player->freeze( 1, false, false, true, 0, true, 0 );

				objMaster::add( new ObjDialog( "[I00Please read this first line!\n[E[I01[X01This is line 2 test. end bye!", &dialogDone ) );
				eventState = triggerEvents::trigEvTestDialogWait;
				
				break;

			case triggerEvents::trigEvTestDialogWait:

				//end this trigger when dialog is done
				if ( dialogDone ) 
				{
					ObjPlayer::player->canIO = true;
					state = triggerStates::triggerNonActive; //this might be useless
					objMaster::remove( this );
				}
				

				break;

			default:
				exit( 1 );
				break;
		}
		//
		//test dialog end
		//

	}
	#ifndef NDEBUG
	else
	{
		//Trigger not known
		exit( 1 );
	}
	#endif

}

std::vector<int>* ObjTrigger::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 

	items->push_back( tag );

	items->push_back( spTag );
	items->push_back( zone  );
	items->push_back( box.x );
	items->push_back( box.y );
	items->push_back( box.width  );
	items->push_back( box.height );
	items->push_back( keyActivated );

	return items;
}

void ObjTrigger::draw()
{
	#ifndef NDEBUG
		if ( !visible ) return;
		Graphics::addDraw( sprites->at(0) );
	#endif
}
