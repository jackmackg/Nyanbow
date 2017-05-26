#ifndef NDEBUG
#ifndef NYAN_MAKE_MAP
#define NYAN_MAKE_MAP

//outside tool from the game takes image files and makes level data files that are used in game
//mostly for collision detection making
//NOTE max image size 8000x8000

#include <string>

namespace TOMAP
{
	
	void startConvert( std::string pathToConver );
}

#endif
#endif