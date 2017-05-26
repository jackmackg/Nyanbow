#include <SFML/Graphics.hpp>
#include <list>
#include <stdlib.h>
#include <math.h>
#include <SFML/System.hpp>
#include <string>
#include <sstream>

#include "tools.h"
#include "graphics.h"
#include "data.h"
#include "io.h"
//#include <iostream>


//makes the window and obj that holds window data
//this will also load the hard coded texters in defined in the data.h

//base items for drawing in SFML
sf::RenderWindow * window;
sf::Texture * textures;
sf::Event events;


//text system
sf::Font font;
sf::Text text;
sf::RenderTexture textDisp;

//hold items to draw
std::list< Draw* > Graphics::drawList;

//changing these values move the camara (it gets set on loadGraphics() as of now)
//int Graphics::camaraSpeed    = 0;
//int Graphics::minDistance    = 0; //weigh point foor minCamaraSPeed
//int Graphics::minCamaraSPeed = 0; //slowest speed outside of MIN_CAMARA_DIST
int Graphics::CamX = 0; //how offset the camara will be from its main location 
int Graphics::CamY = 0;
#define CAMARA_DIVIDER 15 //lower number means faster follow camara
hitbox* Graphics::follow = NULL;
//#define MIN_CAMARA_DIST     6.0f    //if lower than this value go at a speed of one
//#define WARP_SPEED_DIST   150.0f  //if camara is more than this speed up significantly
//#define WARP_SPEED_AMOUNT 12 //lower = faster
//default values that you cant set
int Graphics::offsetX = 0;
int Graphics::offsetY = 0;


//temp debug values
//#ifndef NDEBUG
	//Performance debug timing values
	//DEBUG
	#define ONESECOND 1000000
	int frameTime  = 0;
	int frameCount = 1;
	int frameDisp  = 0;
	sf::Clock debugClock;
	//DEBUG draw call count
	int debugDrawCalls = 0;
	int debugDispDrawCalls = 0;

//#endif

void Graphics::loadGraphics()
{
	window = new sf::RenderWindow( sf::VideoMode( DATA::windowWidth, DATA::windowHeight ), DATA::windowName );
	window->setFramerateLimit( DATA::maxFrameRate );
	window->setVerticalSyncEnabled( DATA::vsync );
	

	//set up camara
	//camaraSpeed    = DATA::camaraSpeed;
	//minDistance    = DATA::minDistance;
	//minCamaraSPeed = DATA::minCamaraSPeed;
	CamX = DATA::CamX; //how offset the camara will be from its main location
	CamY = DATA::CamY;

	//load font
	font.loadFromFile( DATA::getFontFileName() );
	text.setFont( font );
	textDisp.create( DATA::windowWidth, DATA::windowHeight ); //render to this first to make sure it scales well


	//loading in textures ( defaults )
	textures = new sf::Texture[ DATA::textureCount ];  //size will need to be manually textures added
	//these need to be defined in this order according to the way the DATA::sprites are defined
	textures[ DATA::sprite64             ].loadFromFile( DATA::getTextureFileName( nyanFileGraphic::fileSprite64           ) );
	textures[ DATA::spriteBackground     ].loadFromFile( DATA::getTextureFileName( nyanFileGraphic::fileBackgroundTest     ) );
	textures[ DATA::spriteTileBackground ].loadFromFile( DATA::getTextureFileName( nyanFileGraphic::fileTileBackgroundTest ) );
	textures[ DATA::spriteForeground     ].loadFromFile( DATA::getTextureFileName( nyanFileGraphic::fileForegroundTest     ) );
	textures[ DATA::sprite128            ].loadFromFile( DATA::getTextureFileName( nyanFileGraphic::fileSprite128          ) );
}

void Graphics::swapTexture( std::string filePath, int textureId )
{
	textures[ textureId ].loadFromFile( filePath );
}

int Graphics::getTextureWidth( int textureId )
{
	return textures[ textureId ].getSize().x;
}


//takes a draw object pointer and sorts it in order of depth
//camara should be updated first as to not leave objects out when checking if they are in view
void Graphics::addDraw( Draw * drawObj )
{
	//checking if the obj is in view, culling
	if ( drawObj->offsetEffect && //if effected by camara
	    (drawObj->x > offsetX + DATA::windowWidth  || 
	     drawObj->y > offsetY + DATA::windowHeight || 
	     drawObj->x + drawObj->width  < offsetX    || 
	     drawObj->y + drawObj->height < offsetY     )) return;

	//adding objs to draw list in order of depth
	std::list< Draw* >::iterator i;
	std::list< Draw* >::iterator end = drawList.end();

	for( i = drawList.begin(); i != end; ++i )
		if ( drawObj->depth <= (*i)->depth )
		{
			drawList.insert( i, drawObj );
			return;
		}

	drawList.push_back( drawObj ); //if you make it to the end of the list then add obj to end
}

void Graphics::updateCamara()
{
	//==move camara to hitbox==
	
	//make sure camara is following something
	if ( follow == NULL ) return;

	//temp vaules, get view locaion with camara offset
	int offsetXTemp = offsetX + CamX;
	int offsetYTemp = offsetY + CamY;
	int x = follow->centerX(); //these x and y are used to make it more easy to read and these values are computed
	int y = follow->centerY();

	//make the camara move slower when closer to follow hitbox
	//distance the camara is away from where it should be
	float dist = (float)abs( abs( offsetXTemp ) - abs( x ) ) + abs( abs( offsetYTemp ) - abs( y ) );
	//int tempCamaraSpeed = camaraSpeed;
	//
	////if camara is close to follow then start slowing its speed down as it gets closer
	//if ( dist < minDistance ) 
	//		tempCamaraSpeed = (int)TOOLS::max( (float)dist / (float)minDistance * (float)camaraSpeed, (float)minCamaraSPeed );

	//if ( dist > WARP_SPEED_DIST )
	//	tempCamaraSpeed = (int)( dist / WARP_SPEED_AMOUNT );

	//if ( dist < MIN_CAMARA_DIST )
	//	tempCamaraSpeed = 1;

	//"follow camara"
	int tempCamaraSpeed = (int) ( dist / CAMARA_DIVIDER );

	//move x to follow
	offsetXTemp += ( offsetXTemp < x ) ? tempCamaraSpeed : -tempCamaraSpeed;
	//snap to follow if speed will pass it	
	if ( abs( offsetXTemp - x ) <= abs( tempCamaraSpeed ) ) 
		offsetXTemp = x;

	//move y to follow
	offsetYTemp += ( offsetYTemp < y ) ? tempCamaraSpeed : -tempCamaraSpeed;
	
	if ( abs( offsetYTemp - y ) <= abs( tempCamaraSpeed ) ) 
		offsetYTemp = y;

	
	//move camara to new location
	offsetX = offsetXTemp - CamX;
	offsetY = offsetYTemp - CamY;
}

void Graphics::draw()
{
	//#ifndef NDEBUG
		//debug find frame time and draw counter
		frameTime += (int)debugClock.restart().asMicroseconds(); 
	
		if ( frameTime >= ONESECOND )
		{
			frameDisp = frameTime / frameCount;
			debugDispDrawCalls = debugDrawCalls / frameCount;

			frameTime  = 0;
			frameCount = 0;
			debugDrawCalls = 0;

			//drawing debug frame data
			std::ostringstream toStr, toStr2;
			toStr  << frameDisp;
			toStr2 << debugDispDrawCalls;
			window->setTitle( DATA::windowName + " frameTime: " + toStr.str() + " DrawCalls: " + toStr2.str()  );
		}
	
		++frameCount;
	
		//DEBUG
		debugDrawCalls += drawList.size();
	//#endif

	//getting the window set up
	(*window).pollEvent( events );
	(*window).clear( sf::Color::Black );

	//temp vaules for rendering the texture
	sf::Sprite sprite;
	sf::Texture * textureTemp;
	Draw * tempDraw;
	int textureCache = -2;	
	int size = drawList.size();

	for (int i = 0; i < size; i++)
	{
		//getting drawing object from the list and removing it
		tempDraw = drawList.front();
		drawList.pop_front();
		
		#ifndef NDEBUG
		if ( tempDraw->debugSQ ) goto debugDraw;
		#endif

		//if not drawing text
		if ( !tempDraw->isText )
		{
			//dont rebind the texture if it was used last loop
			if ( textureCache != tempDraw->graphic )
			{
				textureTemp = &textures[ tempDraw->graphic ]; 

				//setting up drawing obj
				sprite.setTexture( *textureTemp  );
			}

			//what sprite to draw	
			sprite.setTextureRect( sf::IntRect( 
						tempDraw->TextureX,
						tempDraw->TextureY,
						tempDraw->width , tempDraw->height ) );
			
			
			//set offset based off camara, if the sprite is flipped, and location of the sprite in world space
			if ( tempDraw->offsetEffect )
				sprite.setPosition( sf::Vector2f( (float)tempDraw->x - (float)offsetX, (float)tempDraw->y - (float)offsetY) );
			else
				sprite.setPosition( sf::Vector2f( (float)tempDraw->x, (float)tempDraw->y ) );
		
			//flip sprite on 
			if ( tempDraw->flip )
			{
				//flip the sprite
				sprite.setPosition( sf::Vector2f( sprite.getPosition().x + tempDraw->width , sprite.getPosition().y ) );
				sprite.setScale( -1.0f, 1.0f );
				
				//draw
				window->draw( sprite );
				
				//flip back to not effect other things drawing ontop
				sprite.setScale( 1.0f, 1.0f );
				sprite.setPosition( sf::Vector2f( 0, 0 ) );
			}
			else	
				window->draw( sprite );
		}
		else
		{
			//set text settings
			text.setPosition(  sf::Vector2f( (float)tempDraw->x, (float)tempDraw->y ) );
			text.setFillColor( sf::Color( tempDraw->textColor ) );

			text.setCharacterSize( tempDraw->fontSize );
			text.setString( tempDraw->text );
			
			//render text
			//im rendering it to to a surface first to fix text scaling problems in fullscreen
			textDisp.clear( (sf::Color)0x00000000 );
			textDisp.draw( text );
			textDisp.display();

			sf::Sprite spriteText( textDisp.getTexture() );

			window->draw( spriteText );
		}


		#ifndef NDEBUG
		debugDraw:
		//draw debug rectangle
		if ( tempDraw->debugSQ )//DEBUG
		{
			//make a square outline 1px thick of the color of your choosing
			sf::RectangleShape rec( sf::Vector2f( (float)tempDraw->width, (float)tempDraw->height ) );
			
			rec.setOutlineThickness( -1 );
			rec.setFillColor( sf::Color::Transparent );	
			
			if ( tempDraw->offsetEffect )
				rec.setPosition( (float)tempDraw->x - (float)offsetX, (float)tempDraw->y - (float)offsetY );
			else
				rec.setPosition( (float)tempDraw->x, (float)tempDraw->y );

			
			switch( tempDraw->debugColor )
			{
				case 0:
					rec.setOutlineColor( sf::Color::White );
					break;
				case 1:
					rec.setOutlineColor( sf::Color::Red );
					break;	
				case 2:
					rec.setOutlineColor( sf::Color::Green );
					break;	
				case 3:
					rec.setOutlineColor( sf::Color::Blue );
					break;	
				case 4:
					rec.setOutlineColor( sf::Color::Yellow );
					break;
			}

			window->draw( rec );
		}
		#endif 
	}
	

	//display graphics to window
	window->display();
}

//void Graphics::deleteGraphics()
//{
//	//TODO
//	//delete window;
//	//delete textures;
//}
