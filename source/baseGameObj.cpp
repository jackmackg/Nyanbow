#include "baseGameObj.h"
#include "tools.h"
#include <math.h>
#include "objMaster.h"
#include "data.h"
#include "nonLivingObj.h"

GameObj::GameObj()
{
}

void GameObj::setDefaults()
{
	//
	//values that must be set
	//
	//tag
	//spTag
	//zone


	//
	//values that should be set
	//

	//physics
	physics   = false;
	gravity   = false;
	friction  = false;
	solid     = false;
	hspeed    = 0;
	vspeed    = 0;
	maxHSpeed = 0;

	//obj size
	box.setX( 0 );
	box.setY( 0 );
	box.setHeight( 1 );
	box.setWidth(  1 );

	//drawing and direction facing
	visible   = false;
	dirFacing = nyanDirection::left;

	std::vector<Draw*> *sprites = NULL;
	Draw* mainSprite = NULL;
	
	//hp
	hp = 1;

	//meta
	meta = false;
}

//events
void GameObj::stepEvent()
{
}

void GameObj::attackEvent()
{
}

//void GameObj::metaStep()
//{
//}

void GameObj::deadCheck()
{
}

void GameObj::doPhysics()
{
}

void GameObj::draw()
{
}
//end of events

void GameObj::zoneEntered()
{
}

void GameObj::zoneExit()
{
}

std::vector<int>* GameObj::saveFile()
{
	return NULL;
}

bool GameObj::directionMoving( nyanDirection dirToCheck )
{
	if ( dirToCheck == nyanDirection::up    ) return vspeed < 0;
	if ( dirToCheck == nyanDirection::down  ) return vspeed > 0;
	if ( dirToCheck == nyanDirection::left  ) return hspeed < 0;
	if ( dirToCheck == nyanDirection::right ) return hspeed > 0;
	
	return false;
}

void GameObj::hspeedAdderWithLimter( float acceleration )
{
	//adds acceleration to hspeed but only if under max speed
	float tempHspeed, tempAcc;
	tempHspeed = fabs( hspeed );
	tempAcc    = fabs( acceleration );
	
	hspeed += copysign( ( tempHspeed >= maxHSpeed ) ? 0 : TOOLS::min( tempAcc, maxHSpeed - tempHspeed ), acceleration ); 
}

void GameObj::deleteDraw()
{
	if ( sprites == NULL ) return;

	int size = sprites->size();
	for (int i = 0; i < size; i++)
		delete sprites->at(i);

	sprites->clear();
	delete sprites;
	sprites = NULL;
}

GameObj* GameObj::returnFirstCdTag( hitbox hit, int tag )
{
	GameObj* returnObj;
	int size = objMaster::inZone->size();
	
	for(int i = 0; i < size; ++i )
	{
		returnObj = (*objMaster::inZone)[i];
		
		if ( returnObj->tag == tag && hit.intercepts( returnObj->box ) )
			return returnObj;
	}

	return NULL;
}

GameObj::~GameObj()
{
}
