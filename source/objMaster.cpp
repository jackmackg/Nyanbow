#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "objMaster.h"
#include "baseGameObj.h"
#include "data.h"
#include "gameObjPlayer.h"
#include "nonLivingObj.h"
#include "npc.h"


std::vector<GameObj*> * objMaster::master     = new std::vector<GameObj*>;
std::vector<GameObj*> * objMaster::inZone     = new std::vector<GameObj*>;
std::stack<GameObj*>  * objMaster::removeList = new std::stack<GameObj*>;

//optimization lists
std::vector<GameObj*> * objMaster::damageList = new std::vector<GameObj*>;
std::vector<GameObj*> * objMaster::solidList  = new std::vector<GameObj*>;
int  objMaster::zone;
bool objMaster::loadZone = false;

void objMaster::add( GameObj * obj )
{	//add to current zone only if its for current zone. or if a one zone temp item
	if ( obj->zone == zone || obj->zone == nyanZones::zoneRemoveOnSwitch || obj->zone == nyanZones::zoneAll ) 
	{
		//Note: adding things here also need to be added to the switch zones function below

		inZone->push_back( obj );

		//solid list add
		if ( obj->solid ) solidList->push_back( obj );

		//damage list add
		if ( damageListCriteria( obj ) ) damageList->push_back( obj );
	}
		

	//do not add zone only objects to master list
	if ( obj->zone != nyanZones::zoneRemoveOnSwitch ) master->push_back( obj );
}


bool objMaster::damageListCriteria( GameObj * obj )
{
	//when the player attacks these are all the things they can hit
	return obj->tag == nyanTag::enemies; //|| obj->solid;
}


void objMaster::remove( GameObj * obj )
{
	removeList->push( obj );
}


void objMaster::removeMidEvent()
{
	//run this function between events to remove all the obj that requested it
	//note: for the foor loop i have reversed the way the look at the list to remove items
	//i think this will be faster since new objects are added to the end of the list
	
	GameObj * obj;
	int i, size;

	while( removeList->size() > 0 )
	{
		obj = removeList->top();
		removeList->pop();

		size = inZone->size();

		for( i = size - 1; i >= 0; --i )
		{
			if ( obj == inZone->at(i) )
			{
				inZone->erase( inZone->begin() + i );

				//remove solid list
				if ( obj->solid ) 
				{
					//loop to remove a solid
					size = solidList->size();
					for( i = size - 1; i >= 0; --i )
					{
						if ( obj == solidList->at(i) )
						{
							solidList->erase( solidList->begin() + i );
							break;
						}
					}
				}
				//soild list end

				//loop to remove from damage
				if ( damageListCriteria( obj ) )
				{
					size = damageList->size();
					for( i = size - 1; i >= 0; --i )
					{
						if ( obj == damageList->at(i) )
						{
							damageList->erase( damageList->begin() + i );
							break;
						}
					}
				}
				//damage end

				break;
			}
		}

		if ( obj->zone != nyanZones::zoneRemoveOnSwitch )
		{
			size = master->size();
		
			for(i = size - 1; i >= 0; --i)
				if ( obj == master->at(i) )
				{
					master->erase( master->begin() + i );
					break;
				}
		}			
	
		//remove its graphics
		obj->deleteDraw();
		delete obj;
	}
}


void objMaster::updateZone()
{
	//check if we need to do this
	if ( !loadZone ) return;
	loadZone = false;

	int size, i;
	GameObj * objTmp;

	//delete zone only items and run zone exit code
	size = inZone->size();
	for(i = 0; i < size; ++i)
	{
		objTmp = inZone->at(i);

		if ( objTmp->zone == nyanZones::zoneRemoveOnSwitch ) 
			delete objTmp;
		else
			objTmp->zoneExit();
	}

	inZone->clear();
	solidList->clear();
	damageList->clear();


	//add items to inzone list from main list and run enterzone code
	size = master->size();
	for(i = 0; i < size; ++i) 
	{
		objTmp = master->at( i );

		if ( objTmp->zone == zone || objTmp->zone == nyanZones::zoneAll )
		{
			//inzone list add
			objTmp->zoneEntered();
			inZone->push_back( objTmp );

			//solid list add
			if ( objTmp->solid ) solidList->push_back( objTmp );

			//damage list add
			if ( damageListCriteria( objTmp ) ) damageList->push_back( objTmp );
		}
	}
}

void objMaster::update( int newZone )
{
	zone = newZone;
	loadZone = true;
}

void objMaster::saveToFile( std::string filePath )
{
	std::ofstream file ( filePath.c_str() );
	std::vector<int>* tempData;
	int size, size2, i, i2;

	size = master->size();
	for( i = 0; i < size; ++i ) 
	{
		tempData = master->at(i)->saveFile();
		
		if ( tempData == NULL ) continue; //make sure there is data

		size2 = tempData->size();	
		for( i2 = 0; i2 < size2; ++i2 )
		{
			file << tempData->at(i2);
			file << "\n";
		}

		delete tempData;
	}

	file.close();
}


//loading from file
//objects constructors need to be added/updateded below
void objMaster::addFromFile( std::string filePath )
{
	//load basic object data from a file
	std::ifstream file;
	std::string tempStr;
	int objId;

	file.open( filePath.c_str() );
	
	while( std::getline( file, tempStr )  )
	{
		//decode the id of obj to spawn
		if ( tempStr.c_str()[0] == '#' ) continue; //ignore lines that start with # (can not be used in the middle of an obj def)
		objId = atoi( tempStr.c_str() ); 

		//walls
		if ( nyanSpTag::spFloor == objId )
		{
			std::string x, y, h, w, zone, oneWay, canFall, slope;
			
			std::getline( file, x );
			std::getline( file, y );
			std::getline( file, h );
			std::getline( file, w );
			std::getline( file, zone   );
			std::getline( file, oneWay );
			std::getline( file, canFall );
			std::getline( file, slope );

			add( new ObjWall( 
				atoi( x      .c_str() ), 
				atoi( y      .c_str() ), 
				atoi( h      .c_str() ), 
				atoi( w      .c_str() ), 
   ( nyanZones )atoi( zone   .c_str() ),
				atoi( oneWay .c_str() ),
				atoi( canFall.c_str() ),
				atoi( slope  .c_str() ) 
			) );

			continue;
		}

		//player
		if ( nyanSpTag::spPlayer == objId )
		{
			std::string x, y;

			std::getline( file, x );
			std::getline( file, y );

			add( new ObjPlayer( 
				atoi( x.c_str() ), 
				atoi( y.c_str() ) 
			) );
			continue;
		}
	
		//parallax bg
		if ( nyanSpTag::spBg == objId )
		{
			std::string fileId, ratio, lockX, lockY, noTile, zone, width, height, frameRate, endFrame, depth, texture;

			std::getline( file, fileId );
			std::getline( file, ratio  );
			std::getline( file, lockX  );
			std::getline( file, lockY  );
			std::getline( file, noTile );
			std::getline( file, zone   );
			std::getline( file, width  );
			std::getline( file, height );
			std::getline( file, frameRate );
			std::getline( file, endFrame  );
			std::getline( file, depth   );
			std::getline( file, texture );
			
			add( new ObjBG( 
	 ( nyanFileGraphic )atoi( fileId.c_str() ), 
						atoi( ratio    .c_str() ), 
						atoi( lockX    .c_str() ), 
						atoi( lockY    .c_str() ), 
						atoi( noTile   .c_str() ), 
		   ( nyanZones )atoi( zone     .c_str() ),
						atoi( width    .c_str() ), 
						atoi( height   .c_str() ), 
						atoi( frameRate.c_str() ), 
						atoi( endFrame .c_str() ), 
						atoi( depth    .c_str() ),
						atoi( texture  .c_str() )
						) );
			continue;
		}

		//ObjEnemies
		if ( nyanTag::enemies == objId )
		{
			std::string type, zone, x, y;

			std::getline( file, type  );
			std::getline( file, zone  );
			std::getline( file, x  );
			std::getline( file, y  );

			add( new ObjEnemies( 
   ( nyanSpTag )atoi( type .c_str() ), 
   ( nyanZones )atoi( zone .c_str() ), 
				atoi( x    .c_str() ), 
				atoi( y    .c_str() )
			) );
			continue;
		}

		//ObjTrigger
		if ( nyanTag::triggers == objId )
		{
			std::string type, zone, x, y, height, width, keyActivated;

			std::getline( file, type  );
			std::getline( file, zone  );
			std::getline( file, x  );
			std::getline( file, y  );
			std::getline( file, height );
			std::getline( file, width  );
			std::getline( file, keyActivated );

			add( new ObjTrigger( 
   ( nyanSpTag )atoi( type  .c_str() ), 
   ( nyanZones )atoi( zone  .c_str() ), 
				atoi( x     .c_str() ), 
				atoi( y     .c_str() ),
				atoi( height.c_str() ), 
				atoi( width .c_str() ),
				atoi( keyActivated.c_str() )
			) );
			continue;
		}
	
	}
	file.close();
}


