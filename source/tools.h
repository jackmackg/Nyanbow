#ifndef NYAN_TOOLS
#define NYAN_TOOLS
#include "data.h"
#include <stdlib.h>
#include <string>

//in here goes all the one off items that I dont find large enough
//for a file to itself


namespace TOOLS
{
	// == Timer tools. Frame based ==
	extern int timer;

	int timerSet( int time );

	//returns true if timer has passed
	int timerPassed( int timer ); 

	//called once perframe to update the timer
	//note to me: 1892160000 is a year
	void timerUpdate(); 


	// == min and max ==
	// returns the smallest value
	float min( float x, float y );
	float max( float x, float y );
}


// == Holds the data that will be used in the graphics object ==
class Draw
{
	int frameTimer;

	public:
		int height, width, startFrame, endFrame, delay, frame, step, depth, x, y, graphic;
		
		//offsetEffect this is on then it will be effected by the camaras offset, if not it will stay static on screen (like UI)
		bool offsetEffect;
		bool isText; //so you can have blank text
		bool flip; // if true will flip sprite on x axis

		//used to calculate where to draw the sprite from the texture
		//used by draw functions
		int TextureWidth;
		int TextureX, TextureY;

		std::string text;
		unsigned int textColor;
		int fontSize;

		#ifndef NDEBUG
		bool debugSQ; //if this is on a square will be draw around the area
		int  debugColor; //color of the debug square, 0.white 1.red 3.green 4.blue 5.yellow
		#endif 

		//set the texture the sprite sheet is on and the grid its divided on
		//width and height are required
		//depth is the draw order, and offsetEffect is if the camara effects placement 
		Draw( int textureUsed, int width, int height , int depth, int x, int y, bool offsetEffect );

		//TextureX/Y are the x and y on the texture to draw from
		Draw( int textureUsed, int width, int height , int depth, int x, int y, bool offsetEffect, int TextureX, int TextureY );
		
		Draw( std::string text, int depth, int x, int y, unsigned int textColor, int fontSize );

		#ifndef NDEBUG
		//used to make a draw obj only for debug
		Draw( int debugColor, int x, int y, int height, int width, bool offsetEffect );
		#endif 


		//set what the current animation should be, don't use this if using whole texture
		//start and end are the frames on the sprite sheet the animation is on
		//delay is the number of frames to wait till the next frame
		//step is the amount to move the timer each frame, 1 for forward, 0 to stop, -1 for reverse
		//animation will loop back to first frame once completed
		void drawSetFrames( int start, int end, int delay, int step, bool flip ); 

		//increments to the next frame based on speed
		//run this once per frame
		void drawUpdate();

		//called by other functions to calculate TextureX, TextureY;
		void calculateTexturePosition();
};

//homemade datatypes

//holds size and location of the object
struct hitbox
{
	//use set to set them, read only
	int x, y, height, width, halfHeight, halfWidth;
	
	//calculated from the others adbove, read only
	//right is x + width and height is y + height please use setters
	int right, bottom;

	//initialize to zero
	hitbox()
	{
		setX( 0 );
		setY( 0 );
		setWidth ( 0 );
		setHeight( 0 );
	}

	//only set with these functions
	void setX( int x ) { this->x = x; right  = x + width;  }
	void setY( int y ) { this->y = y; bottom = y + height; }
	void setRight ( int right  ) { x = right  - width;  this->right  = right;  }
	void setBottom( int bottom ) { y = bottom - height; this->bottom = bottom; }  

	//setting x and y relative
	void setXRel( int delta ) { setX( x + delta ); }
	void setYRel( int delta ) { setY( y + delta ); }

	//setting width and height
	void setWidth(  int width  ) { this->width  = width;  right  = x + width;  halfWidth  = width  >> 1; }
	void setHeight( int height ) { this->height = height; bottom = y + height; halfHeight = height >> 1; }

	void reset() { setX( 0 ); setY( 0 ); setWidth( 0 ); setWidth( 0 ); }

	//copying another box/checking
	void copyBox( hitbox box ) { setX( box.x ); setY( box.y ); setWidth( box.width ); setHeight( box.height ); };
	bool checkSameBox( hitbox box ) { return x == box.x && y == box.y && bottom == box.bottom && right == box.right; };

	//gets
	int centerX() { return x + halfWidth ; }
	int centerY() { return y + halfHeight; }

	//movement based on hitbox, give a hitbox and it will move this hitbox to that location
	void leftOf  ( hitbox box ) { setRight(  box.x ); }
	void topOf   ( hitbox box ) { setBottom( box.y ); }
	void rightOf ( hitbox box ) { x = box.right;      }
	void bottomOf( hitbox box ) { y = box.bottom;     }

	//checks/interactions
	bool onTop     ( hitbox box ) { return bottom > box.y; }
	bool intercepts( hitbox box ) { return !( x >= box.right || right <= box.x || y >= box.bottom || bottom <= box.y ); } //true if overlap
	int  distance  ( hitbox box ) { return ( abs( x ) - abs( box.x ) ) + ( abs( y ) - abs( box.y ) ); }
};


#endif
