#include <vector>
#include "gameObjPlayer.h"
#include "data.h"
#include "tools.h"
#include "graphics.h"
#include "objMaster.h"
#include "io.h"
#include "nonLivingObj.h"

//sprites names
#define SPIWALK  0
#define SPISTAND 1
#define SPIJUMP  2
#define SPIFALL  3
#define OFFSET_SPR_X 20

//#define SPIDEBUG  0

//dimenmsions
#define HEIGHT 62
#define WIDTH  26

#define SPRITEHEIGHT 64
#define SPRITEWIDTH  64

//hspeed
#define RUNNING    0.5f
#define RUNNINGMAX 2
#define SLOPE_TP_DOWN 5

//vspeed
#define JUMPHEIGHT 10
#define MAXJUMPS   2

//wall jumping
#define WALLJUMPHEIGHT 8
#define WALLJUMPLENGTH 5
#define WALL_JUMP_FREEZE 5

//thrusting
#define THRUSTSPEED 7.0f
#define SIN45 0.707f

#define THRUSTTIMERPICK  10
#define THRUSTTIMERTHRUST 7
#define THRUSTTIMERCOOLDOWN 30

#define THRUSTSTATENON  0
#define THRUSTSTATEPICK 1
#define THRUSTSTATETHRUSTING 2
#define THRUSTSTATERESET 3

//shooting
#define ATTACK_COOLDOWN 5

//reused code to make doPhysics() code more readable
#define SPFLOOR_ONEWAY  dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i])->oneWay
#define SPFLOOR_CANFALL dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i])->canFall
#define SPFLOOR_SLOPE   dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i])->slope

ObjPlayer * ObjPlayer::player = NULL;

ObjPlayer::ObjPlayer( int x, int y )
{
	setDefaults();

	//save who you are for all to see
	player = this;

	//values to save to file
	saveX = x;
	saveY = y;

	//setup
	zone  = nyanZones::zoneAll;
	tag   = nyanTag::player;
	spTag = nyanSpTag::spPlayer;

	meta = false;
	dirFacing = nyanDirection::left;

	box.setX( x );
	box.setY( y );	
	box.setHeight( HEIGHT );
	box.setWidth ( WIDTH  );
	Graphics::follow = &box; //make the camara follow us

	wallTop   = false;
	wallRight = false;
	wallLeft  = false;
	onGround  = false;
	onSlopeLastFrame = false;

	thrustingAngleSpeed = (int)( THRUSTSPEED * SIN45 );
	
	//one way platforms values
	ignoreOneWay     = false;
	downKeyLastFrame = false;

	//physics
	hspeed = 0;

	vspeed    = 0;
	maxHSpeed = RUNNINGMAX;

	physics  = true;
	gravity  = true;
	friction = true;

	onGround = false;
	solid    = false;

	//players abilities
	canTrust      = true;
	canDoubleJump = true;
	canWallJump   = true;

	//jump
	jumps = 0; //how many jumps you have done
	jumpedLastFrame = false;


	//thrusting
	thrustState = THRUSTSTATENON;
	thrustTimer = 0;

	//freezing
	froze = false;
	freezeTimer = 0;

	canIO = true; //turn off user IO


	//shooting values
	shootCoolDown = 0;


	
	#ifndef NDEBUG
	//debug
	debugOnce = true; //safe to del, used for debug
	#endif


	//graphics
	visible = true;

	//make a test sprite
	sprites = new std::vector<Draw*>;

	
	//real sprites
	
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );
	sprites->push_back( new Draw( nyanFileGraphic::fileSprite64, SPRITEHEIGHT, SPRITEWIDTH, DATA::depthMiddleGround, 0, 0 , true ) );

	//setting the range on the sprite sheet of where things are
	sprites->at( SPISTAND) -> drawSetFrames( 8, 9, 15, 1, false );
	sprites->at( SPIWALK ) -> drawSetFrames( 0, 3,  8, 1, false );
	sprites->at( SPIJUMP ) -> drawSetFrames( 6, 7,  6, 1, false );
	sprites->at( SPIFALL ) -> drawSetFrames( 4, 5,  6, 1, false );
	

	//debug square
	//#ifndef NDEBUG
	//sprites->push_back(new Draw(2, x, y, HEIGHT, WIDTH, true));
	//#endif
}

void ObjPlayer::stepEvent()
{

	#ifndef NDEBUG
	//reset debug
	if ( IO::keyStart() )
	{
		box.setX( saveX );
		box.setY( saveY );
	}
	#endif

	//
	//key attack
	//
	if ( IO::keyShoot() && TOOLS::timerPassed( shootCoolDown ) )
	{
		if ( IO::keyDown())
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::down,  nyanSpTag::spDmgFishMelee, false ) );
		else 
		if ( IO::keyUp() )
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::up,    nyanSpTag::spDmgFishMelee, false ) );
		else
		if ( dirFacing == nyanDirection::left )
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::left,  nyanSpTag::spDmgFishMelee, false ) );
		else
		if ( dirFacing == nyanDirection::right )
			objMaster::add( new ObjDamage( box.x, box.y, nyanDirection::right, nyanSpTag::spDmgFishMelee, false ) );

		shootCoolDown = TOOLS::timerSet( ATTACK_COOLDOWN );
	}


	//
	//jumping
	//
	//disable double jump
	if ( !canDoubleJump && jumps == 0 )
		jumps = 1;

	if ( 
			( IO::keyJump() && canIO && !froze ) && 
			( onGround || jumps < MAXJUMPS ) && 
			!jumpedLastFrame 
	   )
	{
		vspeed = -JUMPHEIGHT;
		++jumps;
		jumpedLastFrame = true;
	}
	
	//reset jump key to allow you to have to tap jump twice
	if ( !IO::keyJump() )
		jumpedLastFrame = false;

	//reset jumps when you land
	if ( onGround )
	{
		jumps = 0;
		jumpedLastFrame = false;
	}
	else //only get one jump if you fall (without jumping) then jump
		if ( jumps == 0 )
			++jumps;


	//moving left/right
	if ( canIO && !froze )
	{
		//left
		if ( IO::keyLeft() && !IO::keyRight() )
		{
			//wall jumping
			if( wallRight && !onGround && canWallJump )
			{
				vspeed = -WALLJUMPHEIGHT;
				hspeed = -WALLJUMPLENGTH;

				freeze( WALL_JUMP_FREEZE, true, true, true, hspeed, true, vspeed );
				dirFacing = nyanDirection::right;
			}
			else
			{
				//running
				dirFacing = nyanDirection::left;
				hspeedAdderWithLimter( -RUNNING );
			}
		}

		//right
		if ( IO::keyRight() && !IO::keyLeft() )
		{
			//wall jumping
			if( wallLeft && !onGround && canWallJump )
			{
				vspeed = -WALLJUMPHEIGHT;
				hspeed =  WALLJUMPLENGTH;

				freeze( WALL_JUMP_FREEZE, true, true, true, hspeed, true, vspeed );
				dirFacing = nyanDirection::left;
			}
			else
			{
				//running
				dirFacing = nyanDirection::right;
				hspeedAdderWithLimter( RUNNING );
			}
		}

	}
	//end of left/right
	

	//down falling through platforms
	if ( ignoreOneWay ) ignoreOneWay = false;

	if ( IO::keyDown() && !downKeyLastFrame && !froze && canIO )
	{
		downKeyLastFrame = true;
		ignoreOneWay     = true;
	}

	if ( !IO::keyDown() )
		downKeyLastFrame = false;


	//thrusting
	//resetting thrust
	if ( thrustState == THRUSTSTATERESET && onGround ) 
		thrustState = THRUSTSTATENON;

	//thrusting start
	if ( IO::keyThrust() && !froze && canIO && canTrust &&
		thrustState == THRUSTSTATENON && TOOLS::timerPassed( thrustTimer ) )
	{
		//freeze player in place and set timer
		canIO   = false;
		physics = false;
		hspeed  = 0;
		vspeed  = 0;
		thrustState = THRUSTSTATEPICK;
		thrustTimer = TOOLS::timerSet( THRUSTTIMERPICK );
	}
	
	//let player choose thrust direction
	if ( thrustState == THRUSTSTATEPICK )
	{
		//no angle x axis
		if ( IO::keyDown() )
			vspeed =  THRUSTSPEED;

		if ( IO::keyUp()  ) 
			vspeed = -THRUSTSPEED;

		if ( IO::keyRight() )
			hspeed =  THRUSTSPEED;

		if ( IO::keyLeft()  ) 
			hspeed = -THRUSTSPEED;

		//angled
		if ( IO::keyDown() )
		{
			if( IO::keyRight() )
			{
				hspeed = (float)thrustingAngleSpeed;
				vspeed = (float)thrustingAngleSpeed;
			}

			if( IO::keyLeft() )
			{
				hspeed = (float)-thrustingAngleSpeed;
				vspeed = (float) thrustingAngleSpeed;
			}
		}

		if ( IO::keyUp() )
		{
			if( IO::keyRight() )
			{
				hspeed = (float) thrustingAngleSpeed;
				vspeed = (float)-thrustingAngleSpeed;
			}

			if( IO::keyLeft() )
			{
				hspeed = (float)-thrustingAngleSpeed;
				vspeed = (float)-thrustingAngleSpeed;
			}

		}

		//thrust player when timer goes out
		if ( TOOLS::timerPassed( thrustTimer ) )
		{
			//if no direction was picked, thrust backwards
			if ( vspeed == 0 && hspeed == 0 )
				hspeed = ( dirFacing == nyanDirection::left ) ? THRUSTSPEED : -THRUSTSPEED;

			//set state up for the thrusting
			thrustState = THRUSTSTATETHRUSTING;
			thrustTimer = TOOLS::timerSet( THRUSTTIMERTHRUST );

			canIO    = true;
			physics  = true;
			gravity  = directionMoving( nyanDirection::down ); //if going down let gravity help
			friction = false;
		}
				
	}
	
	//thrusting
	if ( thrustState == THRUSTSTATETHRUSTING && TOOLS::timerPassed( thrustTimer ) )
	{
		gravity     = true;
		friction    = true;
		thrustState = THRUSTSTATERESET;
		thrustTimer = TOOLS::timerSet( THRUSTTIMERCOOLDOWN );
	}


	//im not sure but I think this needs to be at the end
	unFreeze( false );
}

std::vector<int>* ObjPlayer::saveFile()
{
	std::vector<int>* items = new std::vector<int>(); 

	items->push_back( spTag );
	items->push_back( saveX );
	items->push_back( saveY );

	return items;
}

void ObjPlayer::doPhysics()
{
	if ( !physics ) return;

	//collision detection and movement
	int size, i, tempHspeed;
	float tempMaxFallSpeed;
	GameObj tempObj;
	bool    tempDir;	   //used to hold which direction you are going even after speed reset (fix bug in teleporting to wrong side of object when you hit a wall)
	hitbox  tempLeftRight; //used for left right wall detection

	//apply gravity if not over max fall speed, and only add up to max speed
	if ( gravity && ( vspeed < DATA::maxFall ) ) 
	{
		vspeed += DATA::gravity;

		//"hang" onto walls
		tempMaxFallSpeed = ( wallRight || wallLeft ) ? DATA::maxFallWall : DATA::maxFall;

		if ( vspeed > tempMaxFallSpeed ) vspeed = tempMaxFallSpeed;
	}

	//
	//check if you have moved from last frame for vertical
	//
	if ( boxLastFramePhysics.y != box.y ) 
	{
		wallTop  = false;
		onGround = false;
	}

	boxLastFramePhysics.copyBox( box );

	//
	//1 way wall reset
	//
	if ( box.x >= oneWayOn.right || box.right <= oneWayOn.x ) 
		oneWayOn.reset();

	//
	//slop animation fix
	//
	if ( vspeed <= 0 )
		onSlopeLastFrame = false;


	//
	//find vspeed cd first
	//
	box.setYRel( (int)ceil( vspeed ) ); //moves the player based on vpseed

	size = objMaster::solidList->size();
	tempDir = GameObj::directionMoving( nyanDirection::down );

	for( i = 0; i < size; ++i )
	{
		tempObj = *(*objMaster::solidList)[i];

		//
		//check collision on solid objects
		//
		if ( this != &tempObj && box.intercepts( tempObj.box ) )
		{

			//if its a slope jump to the top and skip the rest
			//this is really simple, it will push the player though solid objects going up
			if ( tempObj.spTag == nyanSpTag::spFloor && dynamic_cast <ObjWall*> ( (*objMaster::solidList)[i] )->slope )
			{
				box.topOf( tempObj.box );
				onGround = true;
				vspeed   = 0;
				onSlopeLastFrame = true;
				continue;
			}
			else
				onSlopeLastFrame = false; //fix slop animation

			//wall bottom
			if ( tempDir ) //going down
			{

				//if you find a oneway plat while going down
				if ( tempObj.spTag == nyanSpTag::spFloor && SPFLOOR_ONEWAY )
				{
					//ignore plat if set to ignore
					if ( SPFLOOR_CANFALL && ignoreOneWay )
					{
						oneWayOn.reset();
						continue;
					}

					//check if we have see this plat before
					if( !tempObj.box.checkSameBox( oneWayOn ) )
					{
						//if not see if we approached it while falling from the top
						if ( ( box.bottom - ceil(vspeed) <= tempObj.box.y ) )
							oneWayOn.copyBox( tempObj.box ); //add this to the see plat and move onto normal movement code below
						else //if not skip this box and keep falling
							continue;
					}
					else //if we have see this box before treat it like a nomral box if we only moved at much as gravity is pushing us, if not fall
						if ( !( vspeed = DATA::gravity ) )
							continue;
				}
				else
					oneWayOn.reset(); //if we dont see it anymore froget it



				//move obj ontop of obj you fell on and set that you are on ground
				box.topOf( tempObj.box );
				onGround = true;
			}
			else //wall top
			{
				oneWayOn.reset(); //reset oneWay plat

				//ignore 1 way platforms 
				if ( tempObj.spTag == nyanSpTag::spFloor && SPFLOOR_ONEWAY ) 
					continue;

				box.bottomOf(tempObj.box);
				wallTop = true;
			}

			vspeed = 0;
		} //end of checking walls

	}//end of for loop

	//hspeed cd
	tempHspeed = (int)ceil( hspeed );

	box.setXRel( tempHspeed );
	tempDir = GameObj::directionMoving( nyanDirection::right );

	//assume no walls till you see one
	wallRight = false;
	wallLeft  = false;

	for( i = 0; i < size; ++i )
	{
		tempObj = *(*objMaster::solidList)[i];

		//ignore 1 way platforms and slopes
		if ( !( tempObj.spTag == nyanSpTag::spFloor && ( SPFLOOR_ONEWAY || SPFLOOR_SLOPE ) ) )
		{
			if
			(	//check collision on solid objects
				tempHspeed != 0  && //only do these checks if moving
				this != &tempObj && 
				box.intercepts( tempObj.box )
			)
			{
				
				if( tempDir ) //if going right
					box.leftOf( tempObj.box );  //wall right
				else
					box.rightOf( tempObj.box ); //wall left

				hspeed = 0;
			}
		
			//
			//check if there is a box to the left or right of you.
			//
			tempLeftRight.copyBox( box );

			tempLeftRight.setXRel( 1 );  //check right
			if ( tempLeftRight.intercepts( tempObj.box ) ) wallRight = true;

			tempLeftRight.setXRel( -2 ); //check left
			if ( tempLeftRight.intercepts( tempObj.box ) ) wallLeft  = true;
		}

	}


	//hspeed friction
	if ( friction )
	{
		float tempHspeed = fabs( hspeed );

		//make friction a % of your speed
		float tempFriction = TOOLS::max( 
			tempHspeed * ( onGround ? DATA::frictionGround : DATA::frictionAir ),  
			DATA::frictionMin );

		if ( tempHspeed - tempFriction < 0 )
			hspeed = 0;
		else
			hspeed = copysign( tempHspeed - tempFriction, hspeed );
	}
}

void ObjPlayer::draw()
{
	
	//if standing
	if ( ceil( hspeed ) == 0 && ceil( vspeed ) == 0 )
		mainSprite = sprites->at( SPISTAND );
	else
		if ( ( ceil( vspeed ) != 0 || !onGround ) && !onSlopeLastFrame ) //drawing in air sprites
			if ( directionMoving( nyanDirection::up ) )
				mainSprite = sprites->at( SPIJUMP );
			else
				mainSprite = sprites->at( SPIFALL );
		else //draw walking sprite
			mainSprite = sprites->at( SPIWALK );
			
	mainSprite->drawUpdate();
	mainSprite->x = box.x - OFFSET_SPR_X;
	mainSprite->y = box.y;
	mainSprite->flip = ( dirFacing == nyanDirection::left ) ? true : false; //flip the sprite to dirFacing
	
	if ( visible ) Graphics::addDraw( mainSprite );
	

	//test sprite thats just a box
	//#ifndef NDEBUG
	//	mainSprite = sprites->at(SPIDEBUG);
	//	mainSprite->x = box.x;
	//	mainSprite->y = box.y;
	//	Graphics::addDraw(mainSprite);
	//#endif
}


void ObjPlayer::freeze( int freezeTimer, bool setGravityFalse, bool setFrictionFalse, bool setHspeed, float newHspeed, bool setVspeed, float newVspeed )
{
	//saving settings for the unFreeze
	setGravity  = setGravityFalse;
	setFriction = setFrictionFalse;
	
	this->setHspeed = setHspeed;
	this->newHspeed = newHspeed;

	//freeze the obj
	if ( setHspeed ) hspeed = 0;
	if ( setVspeed ) vspeed = 0;

    this->setVspeed = setVspeed;
	this->newVspeed = newVspeed;
	
	this->freezeTimer = TOOLS::timerSet( freezeTimer );
	
	//Change these vaules if requested 
	if ( setGravity  ) this->gravity  = false;
	if ( setFriction ) this->friction = false;

	froze = true; 
}

void ObjPlayer::unFreeze( bool now )
{
	if ( !( now || TOOLS::timerPassed( freezeTimer ) ) ) return;
	if ( !froze ) return;

	if ( setGravity  ) this->gravity  = true;
	if ( setFriction ) this->friction = true;

	if ( setHspeed ) this->hspeed = newHspeed;
	if ( setVspeed ) this->vspeed = newVspeed;

	froze = false; 
}

ObjPlayer::~ObjPlayer()
{
	deleteDraw();
}

