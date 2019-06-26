#include "L3D.h"
#include <math.h>


#define MICROPHONE 12
#define GAIN_CONTROL 11
#define MAX_POINTS 20
#define SPEED 0.22


#define FIREWORKS 0
#define SQUARRAL 1
#define PLASMA 2
#define FFT_JOY 3
#define DEMO_ROUTINES 4


void add(Point& a, Point& b);


/******************************
 * fireworks variables *
 * ****************************/
 int centerX, centerY, centerZ;
int launchX, launchZ;
int brightness=50;
float radius=0;
float speed;
bool showRocket;
bool exploded;
float xInc, yInc, zInc;
float rocketX, rocketY, rocketZ;
float launchTime;
int maxSize;
Color rocketColor, fireworkColor;



/*********************************
 * squarral variables *
 * ******************************/

#define TRAIL_LENGTH 50

int frame=0;
Color voxelColor;
Point position, increment, pixel;
Point trailPoints[TRAIL_LENGTH];
int posX, posY, posZ;
int incX, incY, incZ;
int squarral_zInc=1;
int bound=0;
int boundInc=1;
unsigned char axis=0;
bool rainbow=true;

//maxBrightness is the brightness limit for each pixel.  All color data will be scaled down 
//so that the largest value is maxBrightness
int maxBrightness=50;

/********************************
 * zplasma variables *
 * *****************************/
float phase = 0.0;
float phaseIncrement = 0.035; // Controls the speed of the moving points. Higher == faster
float colorStretch = 0.23; // Higher numbers will produce tighter color bands 
float plasmaBrightness = 0.2;
Color plasmaColor;

/*********************************
 * FFTJoy variables *
 * *******************************/
#define M 4
float real[(int)pow(2,M)];
float imaginary[(int)pow(2,M)];
float maxValue=0;
float sample;

/**********************************
 * flip variables *
 * ********************************/
 //accelerometer pinout
#define X 13
#define Y 14
#define Z 15
#define AUTOCYCLE_TIME 22222
#define FACEPLANT 2300
#define UPSIDE_DOWN 1850
#define RIGHTSIDE_UP 2400
#define LEFT_SIDE 1800
#define RIGHT_SIDE 2400
#define FLIP_TIMEOUT 3000
#define FLIP_DEBOUNCE 250

long lastFaceplant=-1*FLIP_TIMEOUT;
bool upsideDown=false;
bool sideways=false;
bool autoCycle=true;    //start on autocycle by default
int upsideDownTime=-1*FLIP_TIMEOUT;
long lastAutoCycle=0;
int lastLeft=-1*FLIP_TIMEOUT;
int lastRight=-1*FLIP_TIMEOUT;
int accelerometer[3];
long lastChange=0;


int demo=FIREWORKS;

int frameCount=0;

/*******************************
 * fade variables *
 * ****************************/
bool fading=false;
int fadeValue=255;
int fadeSpeed=2;

Cube cube=Cube();

void initSquarral();
void squarral();

void initFireworks();
void prepRocket();
void updateFireworks();
float distance(float x, float y, float z, float x1, float y1, float z1);
void add(Point& a, Point& b);

void fade();
void zPlasma();

void FFTJoy();
short FFT(short int dir,int m,float *x,float *y);

void checkFlipState();
void updateAccelerometer();
void setFadeSpeed();
void incrementDemo();
void decrementDemo();

void setup() {
 cube.begin();
 initSquarral();
 initFireworks();
}

void loop() {
    
  if(fading)
        fade();
    else
    {
        if(demo!=FFT_JOY)
            cube.background(black);
        switch(demo)
        {
            case(FIREWORKS):
                updateFireworks();
            break;
            case(PLASMA):
                zPlasma();
            break;
            case(SQUARRAL):
                squarral();
            break;
            case(FFT_JOY):
                FFTJoy();
            break;
            
        }
        frameCount++;
    }
    //check to see how if the cube has been flipped
    checkFlipState();

    cube.show();
    
    if(fading)
    {
        fadeValue-=fadeSpeed;
        //if we're done fading)
        if(fadeValue<=0)
        {
            fading=false;
            fadeValue=255;
        }
        else
            fade();
    }
}

void fade()
{
    Color voxelColor;
        for(int x=0;x<cube.size;x++)
            for(int y=0;y<cube.size;y++)
                for(int z=0;z<cube.size;z++)
                    {
                        voxelColor=cube.getVoxel(x,y,z);
                        if(voxelColor.red>0)
                            voxelColor.red--;
                        if(voxelColor.green>0)
                             voxelColor.green--;
                        if(voxelColor.blue>0)
                            voxelColor.blue--;
                        cube.setVoxel(x,y,z, voxelColor);    
                    }
}

/***************************************
 * fireworks functions *
 * ***********************************/
 
 
void updateFireworks()
{
if(showRocket)
        cube.sphere(rocketX,rocketY,rocketZ,radius, rocketColor);
    if(exploded)
         cube.shell(rocketX,rocketY,rocketZ,radius,fireworkColor);
        

        if(exploded)
            radius+=speed;  //the sphere gets bigger
        if(showRocket)
        {
            rocketX+=xInc;
            rocketY+=yInc;
            rocketZ+=zInc;
        }
        //if our sphere gets too large, restart the animation in another random spot
        if(radius>maxSize)
            prepRocket();
        if(abs(distance(centerX,centerY,centerZ,rocketX, rocketY, rocketZ)-radius)<2)
            {
                showRocket=false;
                exploded=true;
            }
        
}

float distance(float x, float y, float z, float x1, float y1, float z1)
{
    return(sqrt(pow(x-x1,2)+pow(y-y1,2)+pow(z-z1,2)));
}

void prepRocket()
{
            radius=0;
            centerX=rand()%8;
            centerY=rand()%8;
            centerZ=rand()%8;
            fireworkColor.red=rand()%brightness;
            fireworkColor.green=rand()%brightness;
            fireworkColor.blue=rand()%brightness;
            launchX=rand()%8;
            launchZ=rand()%8;
            rocketX=launchX;
            rocketY=0;
            rocketZ=launchZ;
            launchTime=15+rand()%25;
            xInc=(centerX-rocketX)/launchTime;
            yInc=(centerY-rocketY)/launchTime;
            zInc=(centerZ-rocketZ)/launchTime;
            showRocket=true;
            exploded=false;
            speed=0.15;
            maxSize=2+rand()%6;
            //speed=rand()%5;
            //speed*=0.1;
}

void initFireworks()
{
    rocketColor.red=255;
    rocketColor.green=150;
    rocketColor.blue=100;
    prepRocket();
}

void initSquarral() 
{
  position={0,0,0};
  increment={1,0,0};
}

void squarral() 
{
    add(position, increment);
    if((increment.x==1)&&(position.x==cube.size-1-bound))
        increment={0,1,0};
    if((increment.x==-1)&&(position.x==bound))
        increment={0,-1,0};
    if((increment.y==1)&&(position.y==cube.size-1-bound))
        increment={-1,0,0};
    if((increment.y==-1)&&(position.y==bound))
    {
        increment={1,0,0};
        position.z+=squarral_zInc;
        bound+=boundInc;
        if((position.z==3)&&(squarral_zInc>0))
          boundInc=0;
        if((position.z==4)&&(squarral_zInc>0))
          boundInc=-1;
        if((position.z==3)&&(squarral_zInc<0))
          boundInc=-1;
        if((position.z==4)&&(squarral_zInc<0))
          boundInc=0;
        
        if((position.z==0)||(position.z==cube.size-1))
            boundInc*=-1;
            
        if((position.z==cube.size-1)||(position.z==0))
        {
            squarral_zInc*=-1;
            if(squarral_zInc==1)
            {
                axis=rand()%6;
                if(rand()%5==0)
                    rainbow=true;
                else
                    rainbow=false;
            }
        }
    }
    
    posX=position.x;
    posY=position.y;
    posZ=position.z;
    
    incX=increment.x;
    incY=increment.y;
    incZ=increment.z;
    
    for(int i=TRAIL_LENGTH-1;i>0;i--)
    {
        trailPoints[i].x=trailPoints[i-1].x;
        trailPoints[i].y=trailPoints[i-1].y;
        trailPoints[i].z=trailPoints[i-1].z;
    }
    trailPoints[0].x=pixel.x;
    trailPoints[0].y=pixel.y;
    trailPoints[0].z=pixel.z;
    switch(axis){
        case(0):
            pixel.x=position.x;
            pixel.y=position.y;
            pixel.z=position.z;
            break;
        case(1):
            pixel.x=position.z;
            pixel.y=position.x;
            pixel.z=position.y;
            break;
        case(2):
            pixel.x=position.y;
            pixel.y=position.z;
            pixel.z=position.x;
            break;
        case(3):
            pixel.x=position.z;
            pixel.y=cube.size-1-position.x;
            pixel.z=position.y;
            break;
        case(4):
            pixel.x=position.y;
            pixel.y=position.z;
            pixel.z=cube.size-1-position.x;
            break;
        case(5):
            pixel.x=position.x;
            pixel.y=cube.size-1-position.y;
            pixel.z=position.z;
            break;
    }
        
    voxelColor=cube.colorMap(frame%1000,0,1000);
    cube.setVoxel((int)pixel.x, (int)pixel.y, (int)pixel.z, voxelColor);
    for(int i=0;i<TRAIL_LENGTH;i++)
    {
        Color trailColor;
        if(rainbow)
        {
            trailColor=cube.colorMap((frame+(i*1000/TRAIL_LENGTH))%1000,0,1000);
            //fade the trail to black over the length of the trail
            trailColor.red=trailColor.red*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.green=trailColor.green*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.blue=trailColor.blue*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
        }
        else
        {
            trailColor.red=voxelColor.red*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.green=voxelColor.green*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.blue=voxelColor.blue*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
        }
        cube.setVoxel((int)trailPoints[i].x, (int)trailPoints[i].y, (int)trailPoints[i].z, trailColor);
    }
    frame++;
}

void add(Point& a, Point& b)
{
    a.x+=b.x;
    a.y+=b.y;
    a.z+=b.z;
}




/********************************
 * zplasma functions *
 * *****************************/
 
void zPlasma()
{
  
phase += phaseIncrement;
// The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
// We want values that fit the LED grid: x values between 0..8, y values between 0..8, z values between 0...8
// The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
// The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
Point p1 = { (sin(phase*1.000)+1.0) * 4, (sin(phase*1.310)+1.0) * 4.0,  (sin(phase*1.380)+1.0) * 4.0};
Point p2 = { (sin(phase*1.770)+1.0) * 4, (sin(phase*2.865)+1.0) * 4.0,  (sin(phase*1.410)+1.0) * 4.0};
Point p3 = { (sin(phase*0.250)+1.0) * 4, (sin(phase*0.750)+1.0) * 4.0,  (sin(phase*0.380)+1.0) * 4.0};

byte row, col, dep;

// For each row
for(row=0; row<cube.size; row++) {
float row_f = float(row); // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.

// For each column
for(col=0; col<cube.size; col++) {
float col_f = float(col); // Optimization.

// For each depth
for(dep=0; dep<cube.size; dep++) {
float dep_f = float(dep); // Optimization.

// Calculate the distance between this LED, and p1.
Point dist1 = { col_f - p1.x, row_f - p1.y,  dep_f - p1.z }; // The vector from p1 to this LED.
float distance1 = sqrt( dist1.x*dist1.x + dist1.y*dist1.y + dist1.z*dist1.z);

// Calculate the distance between this LED, and p2.
Point dist2 = { col_f - p2.x, row_f - p2.y,  dep_f - p2.z}; // The vector from p2 to this LED.
float distance2 = sqrt( dist2.x*dist2.x + dist2.y*dist2.y + dist2.z*dist2.z);

// Calculate the distance between this LED, and p3.
Point dist3 = { col_f - p3.x, row_f - p3.y,  dep_f - p3.z}; // The vector from p3 to this LED.
float distance3 = sqrt( dist3.x*dist3.x + dist3.y*dist3.y + dist3.z*dist3.z);

// Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
// You can use a cos() for slightly different shading, or experiment with other functions.
float color_1 = distance1; // range: 0.0...1.0
float color_2 = distance2;
float color_3 = distance3;
float color_4 = (sin( distance1 * distance2 * colorStretch )) + 2.0 * 0.5;
// Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
color_1 *= color_1 * color_4;
color_2 *= color_2 * color_4;
color_3 *= color_3 * color_4;
color_4 *= color_4;
// Scale the color up to 0..7 . Max brightness is 7.
//strip.setPixelColor(col + (8 * row), strip.Color(color_4, 0, 0) );
plasmaColor.red=color_1*plasmaBrightness;
plasmaColor.green=color_2*plasmaBrightness;
plasmaColor.blue=color_3*plasmaBrightness;

cube.setVoxel(row,col,dep,plasmaColor);       
}
}
}
}

/********************************************
 *   FFT JOY functions
 * *****************************************/
 void FFTJoy(){
    for(int i=0;i<pow(2,M);i++)
    {
        real[i]=analogRead(MICROPHONE)-2048;
        delayMicroseconds(212);  //this sets our 'sample rate'.  I went through a bunch of trial and error to 
                                //find a good sample rate to put a soprano's vocal range in the spectrum of the cube
      //  Serial.print(real[i]);
        imaginary[i]=0;
    }
    FFT(1, M, real, imaginary);
    for(int i=0;i<pow(2,M);i++)
    {
        imaginary[i]=sqrt(pow(imaginary[i],2)+pow(real[i],2));
//        Serial.print(imaginary[i]);
        if(imaginary[i]>maxValue)
            maxValue=imaginary[i];
    }
    if(maxValue>100)
        maxValue--;
//    Serial.println();
    for(int i=0;i<pow(2,M)/2;i++)
    {
        imaginary[i]=cube.size*imaginary[i]/maxValue;
        int y;
        for(y=0;y<=imaginary[i];y++)
            cube.setVoxel(i,y,cube.size-1,cube.colorMap(y,0,cube.size));
        for(;y<cube.size;y++)
            cube.setVoxel(i,y,cube.size-1,black);
    }
    for(int z=0;z<cube.size-1;z++)
        for(int x=0;x<cube.size;x++)
            for(int y=0;y<cube.size;y++)
            {
                Color col=cube.getVoxel(x,y,z+1);
                cube.setVoxel(x,y,z,col);
//                char output[50];
//                sprintf(output, "%d %d %d:  %d %d %d", x,y,z+1, col.red, col.green, col.blue);
//                Serial.println(output);
            }

    sample++;
    if(sample>=pow(2,M))
        sample-=pow(2,M);
}


short FFT(short int dir,int m,float *x,float *y)
{
   int n,i,i1,j,k,i2,l,l1,l2;
   float c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
   n = 1;
   for (i=0;i<m;i++) 
      n *= 2;

   /* Do the bit reversal */
   i2 = n >> 1;
   j = 0;
   for (i=0;i<n-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0; 
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0; 
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<n;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1; 
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1) 
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      for (i=0;i<n;i++) {
         x[i] /= n;
         y[i] /= n;
      }
   }

   return(0);
}


/****************************************
 * flip functions *
 * **************************************/
 
 void checkFlipState()
 {
    updateAccelerometer();
    /*
    if(accelerometer[2]<UPSIDE_DOWN)  //if the cube is upside-down, set the upside-down flag and mark the time when it was flipped
    {
        upsideDownTime=millis();
  //      Serial.println("I'm upside-down!");
    }
    */
    if(accelerometer[0]>FACEPLANT)  //if the cube is upside-down, set the upside-down flag and mark the time when it was flipped
    {
        lastFaceplant=millis();
  //      Serial.println("I'm upside-down!");
    }
    if(accelerometer[1]<LEFT_SIDE)  //if the cube is flipped to either side
    {
        lastLeft=millis();
//        Serial.println("I'm on my left side");
    }
    if(accelerometer[1]>RIGHT_SIDE)
    {
        lastRight=millis();
  //      Serial.println("I'm on my right side");
    }

    if(accelerometer[2]>RIGHTSIDE_UP)
    {
        if(((millis()-lastFaceplant)<FLIP_TIMEOUT)&&(millis()-lastFaceplant>FLIP_DEBOUNCE))
        {
            autoCycle=false;
            lastFaceplant=millis()-FLIP_TIMEOUT;
            Color flash;
            flash.red=maxBrightness;
            flash.green=maxBrightness;
            flash.blue=maxBrightness;
            cube.background(flash);
        }
        if(((millis()-lastLeft)<FLIP_TIMEOUT)&&(millis()-lastChange>FLIP_DEBOUNCE))
        {
    //        Serial.println("turned to the left and back");
            autoCycle=false;
            lastChange=millis();
            decrementDemo();
            lastLeft=millis()-FLIP_TIMEOUT;
        }
        if(((millis()-lastRight)<FLIP_TIMEOUT)&&(millis()-lastChange>FLIP_DEBOUNCE))
        {
       //     Serial.println("turned to the right and back");
            autoCycle=false;
            lastChange=millis();
            incrementDemo();
            lastRight=millis()-FLIP_TIMEOUT;
        }
    }
    
    if(autoCycle)
        if(millis()-lastAutoCycle>AUTOCYCLE_TIME)   //in autocycle, change demos every 15 seconds
        {
            incrementDemo();
//            Serial.print("autocycling...Demo is ");
//            Serial.println(demo);
            lastAutoCycle=millis();
        }
        
 }
 
void updateAccelerometer()
{
    for(int i=0;i<3;i++)
        accelerometer[i]=analogRead(X+i);
}

void setFadeSpeed()
{
    if(autoCycle)
        fadeSpeed=2;
    else
        fadeSpeed=20;
}
 
 void incrementDemo()
 {
     demo++;
     setFadeSpeed();
     fading=true;
     if(demo>=DEMO_ROUTINES)
        demo=0;
 }
 
  void decrementDemo()
 {
     demo--;
     setFadeSpeed();
     fading=true;
     if(demo<0)
        demo=DEMO_ROUTINES-1;
 }