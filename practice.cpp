//modified by: Rodrigo Garcia-Novoa
//date: 5/31/17
//purpose: Display the Waterfall SDLC with water particles flowing from the top down 
//
//cs3350 Spring 2017 Lab-1
//author: Gordon Griesel
//date: 2014 to present
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

const int MAX_PARTICLES = 5000;
#define GRAVITY 0.1

//X Windows variables
//Display *dpy;
//Window win;
//GLXContext glc;


struct Vec {
        float x, y, z;
};

struct Shape {
        float width, height;
        float radius;
        Vec center;
};

struct Particle {
        Shape s;
        Vec velocity;
};


class Game {
    public:
	Shape box[5];                                   // need 5 boxes for the steps
	Particle particle[MAX_PARTICLES];
	int n;
	int mouse[2];
	int bubbler;                                    // use to turn on and off waterfall
	Game()
	{
	    n = 0;
	    bubbler = 0;
	}
};



void check_b(XEvent *e, Game *game)
{
    if (e->type == KeyPress)
    {
	int key = XLookupKeysym(&e->xkey, 0);

	if (key == XK_b)
	{
	    game->bubbler ^= 1;
	}

    }
}

