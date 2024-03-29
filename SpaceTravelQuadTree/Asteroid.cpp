
#include <cstdlib>
#include <cmath>
#include <list>
#include <iostream>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include "Asteroid.h"

using namespace std;

// Asteroid default constructor.
Asteroid::Asteroid()
{
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
	radius = 0.0; // Indicates no asteroid exists in the position.
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
}

// Asteroid constructor.
Asteroid::Asteroid(float x, float y, float z, float r, unsigned char valueR,
	unsigned char valueG, unsigned char valueB)
{
	this->x = x;
	this->y = y;
	this->z = z;
	radius = r;
	color[0] = valueR;
	color[1] = valueG;
	color[2] = valueB;
}

// Function to draw asteroid.
void Asteroid::draw()
{
	if (radius > 0.0) // If asteroid exists.
	{
		glPushMatrix();
		glTranslatef(x, y, z);
		//glScalef(0.0125 * radius, 0.0125 * radius, 0.0125 * radius);
		glColor3ubv(color);

		// Turn on wireframe mode
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

		// lul wut
		// create the sphere and place it in a global array
		// CreateSphere(SPHERE_SIZE, 0, 0, 0, index);

		// draw sphere
		glDrawArrays(GL_TRIANGLE_FAN, LINE_VERTEX_COUNT + CONE_VERTEX_COUNT, SPHERE_VERTEX_COUNT);

		// Turn off wireframe mode
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);

		glPopMatrix();
	}
}
