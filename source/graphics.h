#ifndef NYAN_GRAPHICS
#define NYAN_GRAPHICS

#include <list>
#include <SFML/Graphics.hpp>
#include "tools.h"
#include <SFML/System.hpp>
#include <string>

//handles all graphics, makes the window and takes in a list of sprites
//and draws them to that window. This uses SFML library
//must manually add textures in constructor

namespace Graphics
{
	//master list of items to draw
	extern std::list< Draw* > drawList;

	//carmara settings
	//set these values to use the camara
	extern int offsetX, offsetY; //set these to move the camara
	extern int CamX, CamY;	     //offset of the camara from the view offset, i will default this to middle of the view 
	//extern int camaraSpeed;      //how fast to move to the obj
	//extern int minDistance;      //how far until camara slowdown happens
	//extern int minCamaraSPeed;   //the slowest speed the camara can move	
	extern hitbox* follow;       //hitbox the camara will follow

	//dialog box
	//this is going to be not that flexible, this can turn into a lot of work if i'm not careful so im keeping it simple
	//TODO: maybe add animations
	//much more documentation inside the cpp
	//these items are settable
	//extern int expressionOffset; //start location in spritePlayer texture (the icon is 64x64 in size) offset will be set in text
	//extern int dialogBoxOffset;  //same as top for the box graphic (64 x 384 is size)
	//extern int storyImageOffset; //same as other two but used 128 tile set (128x128 is size)
	//extern std::string dialog;   //this string will hold the dialog and formatting for the other options above, when this is size is zero dialog is done, when you set it, it will start displaying
	//extern int texturetExpressionID; //texture ids
	//extern int texturetBoxID;
	//extern int texturetStoryImage;


	//must be ran first to load all the graphics
	void loadGraphics();
	void swapTexture( std::string filePath, int textureId ); //swaps that texture with a new file
	int  getTextureWidth( int textureId );

	//used to add draw objects to be drawn
	//this list is cleared after draw is called
	//camara should be updated first as to not leave objects out when checking if they are in view
	void addDraw( Draw * drawObj );

	//this will update the camara location, should be called before the draw() but after the hitbix
	//your following has moved
	//this centers the hitbox it follows on screen
	//Note: Only call this once you have a hitbox set, no error checking
	void updateCamara();

	//takes a vector of draw objects and draws
	//call this once perframe at the end
	void draw();

	//void deleteGraphics();
}

#endif
