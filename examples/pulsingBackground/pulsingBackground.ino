#include "L3D.h"
#include "math.h"

Cube cube=Cube();
float t=0;
int maxBrightness=20;  //if you set the background super bright, it will gobble up lots of current.  Too large a number will make the LEDs flicker, especially if you're not running on the provided 2A power supply

void setup() {
    cube.begin();
}

void loop() {
    cube.background(Color(maxBrightness*pow(sin(t/3),2), maxBrightness*pow(sin(t/2),2), maxBrightness*pow(cos(t/5),2)));
    cube.show();
    t=millis()/1000;

}