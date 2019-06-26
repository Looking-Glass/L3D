#include "L3D.h"
//super simple, hello, world-style program that shows off the setVoxel() function and toggles the color of a single voxel in the cube
Cube cube=Cube();

int blinkRate=500; //ms

void setup()
{
	cube.begin();
	cube.background(black);
}

void loop()
{
	cube.setVoxel(3,3,3,red);
	cube.show();
	delay(blinkRate);
	cube.setVoxel(3,3,3,green);
	cube.show();	
	delay(blinkRate);	
	cube.setVoxel(3,3,3,blue);
	cube.show();	
	delay(blinkRate);	
	cube.setVoxel(3,3,3,black);
	cube.show();	
	delay(blinkRate);	
}