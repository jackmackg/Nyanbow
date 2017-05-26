#include "io.h"
#include <SFML/Window.hpp>

int IO::deadZone = 25; //range is zero to 100. Zero is center.

bool IO::keyJump()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) || 
		   sf::Joystick::isButtonPressed( 0, 0 );
}

bool IO::keyShoot()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::S ) || 
		   sf::Joystick::isButtonPressed( 0, 1 );
}

bool IO::keyThrust()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::A ) || 
		   sf::Joystick::isButtonPressed( 0, 3 );
}

bool IO::keyUp()
{

	return sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) || 
		 ( sf::Joystick::getAxisPosition( 0, sf::Joystick::Y ) < -deadZone );
}

bool IO:: keyDown()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) || 
		 ( sf::Joystick::getAxisPosition( 0, sf::Joystick::Y ) >  deadZone );
}

bool IO::keyLeft()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) || 
		 ( sf::Joystick::getAxisPosition( 0, sf::Joystick::X ) < -deadZone );
}

bool IO::keyRight()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) || 
		 ( sf::Joystick::getAxisPosition( 0, sf::Joystick::X ) >  deadZone );
}

bool IO::keyStart()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::Return );
}

bool IO::keyExit()
{
	return sf::Keyboard::isKeyPressed( sf::Keyboard::Escape );
}
