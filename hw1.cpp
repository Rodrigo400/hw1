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
#include "fonts.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

const int MAX_PARTICLES = 5000;
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

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
                Shape box[5];					// need 5 boxes for the steps
                Particle particle[MAX_PARTICLES];
                int n;
                int mouse[2];
                int bubbler; 					// use to turn on and off waterfall
                Game()
                {
                        n = 0;
                        bubbler = 0;
                }
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);


int main(void)
{
        int done=0;
        srand(time(NULL));
        initXWindows();
        init_opengl();
        //declare game object
        Game game;
        game.n = 0;

        //declare a box shape
        for (int i = 0; i < 5; i++)
        {
                game.box[i].width = 100;
                game.box[i].height = 15;
                game.box[i].center.x = 200 + i*75;              // display boxes in stair format
                game.box[i].center.y = 500 - i*70;
        }

        //start animation
        while (!done) {
                while (XPending(dpy)) {
                        XEvent e;
                        XNextEvent(dpy, &e);
                        check_mouse(&e, &game);
                        done = check_keys(&e, &game);
                }
                movement(&game);
                render(&game);
                glXSwapBuffers(dpy, win);
        }
        cleanupXWindows();
        cleanup_fonts();
        return 0;
}

void set_title(void)
{
        //Set the window title bar.
        XMapWindow(dpy, win);
        XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void)
{
        //do not change
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
}

void initXWindows(void)
{
        //do not change
        GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
                std::cout << "\n\tcannot connect to X server\n" << std::endl;
                exit(EXIT_FAILURE);
        }
        Window root = DefaultRootWindow(dpy);
        XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
        if (vi == NULL) {
                std::cout << "\n\tno appropriate visual found\n" << std::endl;
                exit(EXIT_FAILURE);
        } 
        Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        XSetWindowAttributes swa;
        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                ButtonPress | ButtonReleaseMask | PointerMotionMask |
                StructureNotifyMask | SubstructureNotifyMask;
        win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
                        InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
        set_title();
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
        //OpenGL initialization
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        //Initialize matrices
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        //Set 2D mode (no perspective)
        glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
        //Set the screen background color
        glClearColor(1.0, 0.4117, 0.7058, 1.0);					// pink
        glEnable(GL_TEXTURE_2D);
        initialize_fonts();
}

#define rnd() (float)rand() / (float)RAND_MAX

void makeParticle(Game *game, int x, int y)
{
        if (game->n >= MAX_PARTICLES)
                return;
        //std::cout << "makeParticle() " << x << " " << y << std::endl;
        //position of particle
        Particle *p = &game->particle[game->n];
        p->s.center.x = x;
        p->s.center.y = y;
        p->velocity.y = rnd() * 2.0 - 1.0;                  // initial particle velocities when created
        p->velocity.x = rnd() * 2.0 - 1.0;
        game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
        static int savex = 0;
        static int savey = 0;
        //static int n = 0;				// used in lab1.cpp, unused after changes for hw1.cpp

        if (e->type == ButtonRelease) {
                return;
        }
        if (e->type == ButtonPress) {
                if (e->xbutton.button==1) {
                        //Left button was pressed
                        int y = WINDOW_HEIGHT - e->xbutton.y;	

                        for (int i = 0; i < 25; i++)
                        {
                                makeParticle(game, e->xbutton.x, y);
                        }
                        return;
                }
                if (e->xbutton.button==3) {
                        //Right button was pressed
                        return;
                }
        }
        //Did the mouse move?
        if (savex != e->xbutton.x || savey != e->xbutton.y) {
                savex = e->xbutton.x;
                savey = e->xbutton.y;
                //if (++n < 10)
                //	return;
                int y = WINDOW_HEIGHT - e->xbutton.y;

                if (game->bubbler == 0)
                {
                        game->mouse[0] = savex;
                        game->mouse[1] = y;
                }

                for (int i = 0; i < 25; i++)
                {
                        makeParticle(game, e->xbutton.x, y);
                }
        }
}

extern int check_b(XEvent *e, Game *game);
/*{
    if (e->type == KeyPress)
    {
	int key = XLookupKeysym(&e->xkey, 0);

	if (key == XK_b)
	{
	    game->bubbler ^= 1;
	}

	return 0;
    }
}
*/

int check_keys(XEvent *e, Game *game)
{
        //Was there input from the keyboard?
        if (e->type == KeyPress) {
                int key = XLookupKeysym(&e->xkey, 0);
                if (key == XK_Escape) {
                        return 1;
                }
                //You may check other keys here.
                /*if (key == XK_b)                            // check if 'b' key was pressed for bubbler
                {
                        game->bubbler ^= 1;
                }
		*/

		check_b(e, game);


        }
        return 0;
}

void movement(Game *game)
{
        Particle *p;

        if (game->n <= 0)
                return;

        if (game->bubbler != 0)
        {
                for (int i = 0; i < 30; i++)                        // make more while bubbling
                {
                        makeParticle(game, game->mouse[0], game->mouse[1]);
                }
        }
        

        for (int i = 0; i < game->n; i++)
        {
                p = &game->particle[i];
                p->velocity.y -= GRAVITY;
                p->s.center.x += p->velocity.x;
                p->s.center.y += p->velocity.y;


                //check for collision with shapes...
                Shape *s;

                // collision with the 5 boxes
                for (int j = 0; j < 5; j++)
                {
                        s = &game->box[j];

                        if ( p->s.center.y < s->center.y + s->height &&
                                        p->s.center.y > s->center.y - s->height && 
                                        p->s.center.x > s->center.x - s->width &&
                                        p->s.center.x < s->center.x + s->width)
                        {	
                                p->s.center.y = s->center.y + s->height;
                                p->velocity.y = -p->velocity.y * 0.3;            // dont make it bounce too high
                                p->velocity.x += 0.015;                            // when hitting box, move right
                        }	

                }

                //collision with the circle


                //check for off-screen
                if (p->s.center.y < 0.0 || p->s.center.y > WINDOW_HEIGHT) {
                        //std::cout << "off screen" << std::endl;
                        game->particle[i] = game->particle[game->n-1];
                        game->n--;
                }
        }
}

void render(Game *game)
{
        Rect r;
        float w, h;
        glClear(GL_COLOR_BUFFER_BIT);
        //Draw shapes...

        //draw box
        Shape *s;
        for (int i = 0; i < 5; i++)
        {
                glColor3ub(10,0,250);
                s = &game->box[i];

                //game->box[i].center.x = WINDOW_HEIGHT - 100;
                //game->box[i].center.y = 100;

                glPushMatrix();
                glTranslatef(s->center.x, s->center.y, s->center.z);
                w = s->width;
                h = s->height;
                glBegin(GL_QUADS);

                glColor3ub(30, 144, 255);
                glVertex2i(-w,-h);
                glColor3ub(255, 255, 255);
                glVertex2i(-w, h);
                glColor3ub(255, 255, 255);
                glVertex2i( w, h);
                glColor3ub(30, 144, 255);
                glVertex2i( w,-h);
                
                glEnd();
                glPopMatrix();
        }

        //draw all particles here
        for(int i = 0; i <game->n; i++)
        {	    
                glPushMatrix();

                /*int aliceblue[3] = {240, 248, 255};
                int powderblue[] = {176, 224, 230};
                int lightskyblue[] = {135, 206, 250};
                int deepskyblue[] = {0, 191, 255};
                int dodgerblue[] = {30, 144, 255};
                */

                Vec *c = &game->particle[i].s.center;
                w = 3;
                h = 3;
                glBegin(GL_QUADS);

                glColor3ub(240, 248, 255);
                glVertex2i(c->x-w, c->y-h);
                glColor3ub(176, 224, 230);
                glVertex2i(c->x-w, c->y+h);
                glColor3ub(0, 191, 255);
                glVertex2i(c->x+w, c->y+h);
                glColor3ub(0, 0, 255);
                glVertex2i(c->x+w, c->y-h);

                glEnd();
                glPopMatrix();
        }


        r.bot = WINDOW_HEIGHT - 50;
        r.left = 100;
        r.center = 0;

        
        r.bot = game->box[0].center.y - 10;
        r.left = game->box[0].center.x - 60;
        ggprint16(&r, 16, 0xff0000, "Requirements");
        
        r.bot = game->box[1].center.y - 10;
        r.left = game->box[1].center.x - 60;
        ggprint16(&r, 16, 0xff4500, "Design");

        r.bot = game->box[2].center.y - 10;
        r.left = game->box[2].center.x - 60;
        ggprint16(&r, 16, 0xffff00, "Coding");

        r.bot = game->box[3].center.y - 10;
        r.left = game->box[3].center.x - 60;
        ggprint16(&r, 16, 0x006400, "Testing");

        r.bot = game->box[4].center.y - 10;
        r.left = game->box[4].center.x - 60;
        ggprint16(&r, 16, 0x0000ff, "Maintenance");
}

