#ifndef NDEBUG
/*
by brandon and andrew, makes level objs out of blocks

warning this code is cuppled
*/

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <stdio.h>

#include "imgToMap.h"
#define EMPTY 0
#define OCCUPIED 1
#define DEFSIZE 10000

#define NORMALPLAT 0
#define ONEWAYPLAT 1
#define ONEWAYONLYPLAT 2
#define SLOPEPLAT 3

//colors the represent platforms types
#define NORMALPLATCOLOR 0x000000FF //remove the ff at the end to get the RGB in hex
#define ONEWAYPLATCOLOR 0x0026FFFF
#define ONEWAYONLYPLATCOLOR 0x21007FFF //can go up but now down
#define SLOPEPLATCOLOR 0x7F0000FF

//main image, 0 is empty, 1 is a wall
bool img[DEFSIZE][DEFSIZE];
bool imgMirrored[DEFSIZE][DEFSIZE]; //used to hold a mirrored image when checking for better blocks


//holds squares to compare
struct rec
{
	int x, y, h, w;
};

std::vector<rec> *answers = new std::vector<rec>();          //holds normal image blocks
std::vector<rec> *answersDone4real = new std::vector<rec>(); //filter out rectangles that dont add anything

int HEIGHT, WIDTH;

//looks at the array and gets the basic blocks with no overlapping
//flipped adds a offset to the mirrored array to make the lists compararble
void rectangleFinder( bool img[][DEFSIZE], std::vector<rec>* list, bool flipped )
{
	int x, y;

	for ( y = 0; y < HEIGHT; y++ )
	{
		for ( x = 0; x < WIDTH; x++ )
			if ( img[ y ][ x ] == OCCUPIED )
			{

				rec tempRec;
				tempRec.x = x;
				tempRec.y = y;

				int superX = x;
				int superY = y;

				//Finding the width
				while ( ( img[superY][superX] == OCCUPIED ) && ( superX < WIDTH ) )
				{
					superX++;
				}

				tempRec.w = superX - x; //set width


				//Finding the height of the rectangle
				superX = x;
				superY++;
				int i = 0;

				while (superY < HEIGHT)
				{
					for (; i < tempRec.w; i++)
					{
						if (img[superY][superX] == EMPTY) break;
						superX++;
					}

					if (i != tempRec.w) 	//if you hit a empty space before making it to the end 
					{
						superY--;
						break;
					}
					else //move down a row if made it to the end with not hitting spaces
					{
						superX = x;
						superY++;
						i = 0;
					}
				}

				//if hit end of image move back up
				if (superY == HEIGHT) superY--;

				tempRec.h = superY - (y - 1); //set height

				//clean out the section you just sqaured out to not read it again
				for (int y2 = 0; y2 < tempRec.h; y2++)
					for (int x2 = 0; x2 < tempRec.w; x2++)
						img[y + y2][x + x2] = EMPTY;

				//set where to start searching for the next square
				x += tempRec.w;

				//Saving new rectangle
				if (flipped)
				{ //offsets for reversed image 
					tempRec.x = (WIDTH - 1) - ((tempRec.x + tempRec.w) - 1);
					tempRec.y = (HEIGHT - 1) - ((tempRec.y + tempRec.h) - 1);
				}

				list->push_back(tempRec); //save to the list
			}
	}
}


//sorting by largest rec
bool sort(rec a, rec b)
{
	return (a.w * a.h) > (b.h * b.w);
}

//input file is the .png to blocout
//output is the text file output
//type is the format the text file will be saved in
//wi and hi are WIDTH and HEIGHT
void convert( std::string inputFile, std::string outputFile, int type  )
{
	//clear out old data
	answers->clear();
	answersDone4real->clear();

	//loading in test image
	sf::Image tempTxt;
	tempTxt.loadFromFile(inputFile);

	HEIGHT = tempTxt.getSize().y;
	WIDTH = tempTxt.getSize().x;

	//place test image on starting grid
	//black pixels mean wall, all other ignored

	unsigned int lookForColor;
	switch ( type )
	{
		case NORMALPLAT:
			lookForColor = NORMALPLATCOLOR;
			break;

		case ONEWAYPLAT:
			lookForColor = ONEWAYPLATCOLOR;
			break;

		case ONEWAYONLYPLAT:
			lookForColor = ONEWAYONLYPLATCOLOR;
			break;

		case SLOPEPLAT:
			lookForColor = SLOPEPLATCOLOR;
			break;
	}

	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
			img[y][x] = tempTxt.getPixel(x, y) == sf::Color( lookForColor ) ? OCCUPIED : EMPTY;

			


	//180 flip
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
			imgMirrored[y][x] = img[HEIGHT - 1 - y][WIDTH - 1 - x];

	rectangleFinder(imgMirrored, answers, true);

	//check main upright image last
	rectangleFinder(img, answers, false);


	//first sort so largest area rectangles draw first
	std::sort(answers->begin(), answers->end(), sort);


	//draw to make sure they done overlap
	bool didSomething = false;
	for (unsigned int i = 0; i < answers->size(); i++)
	{
		rec tempRec = (answers->at(i));

		for (int y = 0; y < tempRec.h; y++)
			for (int x = 0; x < tempRec.w; x++)
			{
				if (img[tempRec.y + y][tempRec.x + x] == EMPTY)
					didSomething = true;

				img[tempRec.y + y][tempRec.x + x] = OCCUPIED;
			}

		if (didSomething)
			answersDone4real->push_back(tempRec);

		didSomething = false;
	}


	//save resualts to file
	std::fstream myfile;
	myfile.open(outputFile, std::ios::app);


	//make the test player, but only do it once (so im only doing it doing it for normal plat)
	if ( type == NORMALPLAT )
	{
		myfile << 0 << "\n"; //id(sp tag) of obj
		myfile << 50 << "\n";
		myfile << 50 << "\n";
	}






	for (unsigned int i = 0; i < answersDone4real->size(); i++)
	{
		rec tempRec = (answersDone4real->at(i));

		switch( type )
		{
			case NORMALPLAT: //solid plats
				myfile << 1 << "\n"; //id(sp tag) of obj
				myfile << tempRec.x << "\n";
				myfile << tempRec.y << "\n";
				myfile << tempRec.h << "\n";
				myfile << tempRec.w << "\n";
				myfile << 1 << "\n"; //zone 
				myfile << 0 << "\n"; //oneWay
				myfile << 1 << "\n"; //canFall
				myfile << 0 << "\n"; //slope
				break;

			case ONEWAYPLAT: //oneWay plats
				myfile << 1 << "\n"; //id(sp tag) of obj
				myfile << tempRec.x << "\n";
				myfile << tempRec.y << "\n";
				myfile << tempRec.h << "\n";
				myfile << tempRec.w << "\n";
				myfile << 1 << "\n"; //zone 
				myfile << 1 << "\n"; //oneWay
				myfile << 1 << "\n"; //canFall
				myfile << 0 << "\n"; //slope
				break;

			case ONEWAYONLYPLAT: //oneWay plats
				myfile << 1 << "\n"; //id(sp tag) of obj
				myfile << tempRec.x << "\n";
				myfile << tempRec.y << "\n";
				myfile << tempRec.h << "\n";
				myfile << tempRec.w << "\n";
				myfile << 1 << "\n"; //zone 
				myfile << 1 << "\n"; //oneWay
				myfile << 0 << "\n"; //canFall
				myfile << 0 << "\n"; //slope
				break;

			case SLOPEPLAT: //oneWay plats
				myfile << 1 << "\n"; //id(sp tag) of obj
				myfile << tempRec.x << "\n";
				myfile << tempRec.y << "\n";
				myfile << tempRec.h << "\n";
				myfile << tempRec.w << "\n";
				myfile << 1 << "\n"; //zone 
				myfile << 1 << "\n"; //oneWay
				myfile << 1 << "\n"; //canFall
				myfile << 1 << "\n"; //slope
				break;
		}

		
	}


	myfile.close();
}

void TOMAP::startConvert( std::string pathToConver )
{
	//remove old data
	#ifdef _WIN32
		std::string tempStrData = pathToConver + "data\\levelData";
		std::string tempStrImg = pathToConver + "data\\levelToConvert.png";
	#elif __linux
		std::string tempStrData = pathToConver + "/data/levelData";
		std::string tempStrImg = pathToConver + "/data/levelToConvert.png";
	#endif

	//remove old data
	remove( tempStrData.c_str() );

	//do 3 passes grabbing the 3 diffrent types of plats
	convert( tempStrImg, tempStrData, NORMALPLAT );
	convert( tempStrImg, tempStrData, ONEWAYPLAT );
	convert( tempStrImg, tempStrData, SLOPEPLAT );
	//convert( tempStrImg, tempStrData, ONEWAYONLYPLAT );


}
#endif