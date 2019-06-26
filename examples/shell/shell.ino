#include "L3D.h"

Cube cube=Cube();
float rad, minRad=0, maxRad=4, inc=0.1;
void setup() {
    cube.begin();

}

void loop() {
    cube.background(black);
    cube.shell(3,3,3,rad,0.01,Color(0,20,0));
    cube.show();
    rad+=inc;
    if(rad>maxRad || rad<minRad)
        inc*=-1;

}