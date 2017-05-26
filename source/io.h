#ifndef NYAN_IO
#define NYAN_IO

//Functions return true when key is down, false if not
//these functions use the SFML library

namespace IO
{
	bool keyJump();
	bool keyShoot();
	bool keyThrust();

	bool keyUp();
	bool keyDown();
	bool keyLeft();
	bool keyRight();

	bool keyStart();
	bool keyExit();

	//0 to 100, 0 is none
	extern int deadZone;
}

#endif
