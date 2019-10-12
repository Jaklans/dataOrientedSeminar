
#ifndef Asteroid_13389437
#define Asteroid_13389437
#define PI 3.14159265
#include <glm/glm.hpp>
#include "Config.h"


// Asteroid class.
struct Asteroid
{
   Asteroid();
   Asteroid(float x, float y, float z, float r, unsigned char valueR, 
	    	unsigned char valueG, unsigned char valueB);
   void draw();
   float x, y, z, radius;
   unsigned char color[3];
};

#endif