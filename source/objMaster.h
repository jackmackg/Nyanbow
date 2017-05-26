#ifndef NYAN_OBJMASTER
#define NYAN_OBJMASTER
#include "tools.h"
#include "baseGameObj.h"
#include <vector>
#include <string>
#include <stack>


//holds all the game objects
//provieds search functions also
namespace objMaster 
{
	extern std::vector<GameObj*> *master;
	extern std::vector<GameObj*> *inZone;
	extern std::stack<GameObj*>  *removeList;

	//optimization lists, used to speed up obj searches
	extern std::vector<GameObj*> *damageList; //list that keeps track of inzone items that can interact with damage (does not include player)
	extern std::vector<GameObj*> *solidList;  //items that are solid and in zone, used for collision detection
	
	
	extern int  zone;
	extern bool loadZone; //if true load new zone

	//adds a game object to the lists
	//objects with zoneRemoveOnSwitch will not be added to master list 
	void add( GameObj * obj );

	bool damageListCriteria( GameObj * obj );

	//load/save objects from file, cpp file will have more info on this
	void addFromFile( std::string filePath );
	void saveToFile( std::string filePath  ); //loops though all the objs and runs their save function

	//adds objs to a list to be deleted
	//run removeMidEvent to remove the items (between events in the main loop is the main idea)
	void remove( GameObj * obj ); //only sets up a removal
	void removeMidEvent( ); //run this function between events to remove all the obj that requested it
	
	

	//takes items off the main list and adds them to inZoneList
	//this is slow, use only when switching zones
	void update( int newZone ); //only sets up a new zone switch
	void updateZone(); //run this function between events to load the new zone
}

#endif
