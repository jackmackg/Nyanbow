#include <string>
#include "tools.h"
#include "data.h"
#include "graphics.h"

// == Timer tools ==
int TOOLS::timer = 0;

int TOOLS::timerSet( int time ) { return time + TOOLS::timer; }

int TOOLS::timerPassed( int setTimer ) { return TOOLS::timer > setTimer; } 

void TOOLS::timerUpdate() { TOOLS::timer++; }

// ==min and max==
float TOOLS::min( float x, float y ) { return x < y ? x : y; }
float TOOLS::max( float x, float y ) { return x > y ? x : y; }

// == Draw class ==
Draw::Draw( int textureUsed, int width, int height, int depth, int x, int y, bool offsetEffect )
{
	this->height  = height;
	this->width   = width;
	this->graphic = textureUsed;
	this->depth   = depth;
	
	this->x = x;
	this->y = y;

	this->offsetEffect = offsetEffect;

	TextureWidth = Graphics::getTextureWidth( this->graphic ) / width; //get how many textures wide it is

	//zero out all values
	frame = 0;
	step  = 0;
	delay = 0;
	flip  = false;
	text   = "";
	isText = false;
	startFrame = 0;
	endFrame   = 0;
	frameTimer = 0;
	

	#ifndef NDEBUG
	debugSQ = false;
	#endif 

	calculateTexturePosition();
}

Draw::Draw( int textureUsed, int width, int height , int depth, int x, int y, bool offsetEffect, int TextureX, int TextureY )
{
	this->height  = height;
	this->width   = width;
	this->graphic = textureUsed;
	this->depth   = depth;

	this->x = x;
	this->y = y;

	this->offsetEffect = offsetEffect;

	TextureWidth = Graphics::getTextureWidth( this->graphic ) / width; //get how many textures wide it is

	//zero out all values
	frame = 0;
	step  = 0;
	delay = 0;
	flip  = false;
	text   = "";
	isText = false;
	startFrame = 0;
	endFrame   = 0;
	frameTimer = 0;


	#ifndef NDEBUG
	debugSQ = false;
	#endif 

	this->TextureX = TextureX;
	this->TextureY = TextureY;
}

//debug draw, only draws square around settings
#ifndef NDEBUG
Draw::Draw( int debugColor, int x, int y, int height, int width, bool offsetEffect )
{
	//default assumed settings
	debugSQ = true;
	graphic = DATA::spriteNon;
	depth   = DATA::depthspriteDebug;

	this->debugColor   = debugColor;
	this->offsetEffect = offsetEffect;
	this->x = x;
	this->y = y;
	this->height = height;
	this->width  = width;

	text   = "";
	isText = false;
}
#endif 

Draw::Draw( std::string text, int depth, int x, int y, unsigned int textColor, int fontSize )
{
	this->x = x;
	this->y = y;

	this->text      = text;
	this->depth     = depth;
	this->textColor = textColor;
	this->fontSize  = fontSize;

	//text always draws bases off window not world space
	offsetEffect = false;
	isText = true;

	#ifndef NDEBUG
	debugSQ = false;
	#endif
}

void Draw::drawSetFrames( int start, int end, int delay, int step, bool flip )
{
	this->startFrame = start;
	this->frame      = start;
	this->step       = step;
	this->delay      = delay;
	this->endFrame   = end;
	this->flip       = flip;

	frameTimer = 0;

	calculateTexturePosition();
}

//moves the frameTimer to increment/decrment the current frame.
//loops if hits the end or start of the frame limits
void Draw::drawUpdate()
{
	frameTimer += step;

	if ( frameTimer > delay )
	{
		frameTimer = 0;
		if ( ++frame > endFrame ) frame = startFrame;

		calculateTexturePosition();
	}
	else 
	if ( frameTimer < 0 )
	{
		frameTimer = delay;
		if ( --frame < startFrame ) frame = endFrame;

		calculateTexturePosition();
	}	
}

//math of where to get sprite from texture
void Draw::calculateTexturePosition()
{
	TextureX = frame % TextureWidth * width;
	TextureY = frame / TextureWidth * height;
}