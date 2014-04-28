#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")
#pragma comment(lib, "GLU32")
#pragma comment(lib, "OpenGL32")

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>

#include "common.h"

t_levelBlock world[32][32][32];

void init()
{
	int i, j, k;

	for(i=0; i<32; i++)
	{
		for(j=0; j<32; j++)
		{
			for(k=0; k<32; k++)
			{
				world[i][j][k].type = 1;
			}
		}
	}
}

void draw_cube(int i, int j, int k)
{
	glBindTexture(GL_TEXTURE_2D, NULL);

	// draw different faces of the cube
	// only if there isn't another cube
	// adjacent on that side

	glBegin(GL_QUADS);
			
		glColor3f(1.0f,1.0f,1.0f);

		if(k==31)
		{
			//front
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
		}

		if(k==0)
		{
			//back
			glColor3f(1.0f,0.0f,1.0f);
			glVertex3f(0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
		}

		if(j==0)
		{
			//bottom
			glColor3f(0.0f,0.0f,1.0f);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
		}

		if(j==31)
		{
			//top
			glColor3f(0.0f,1.0f,1.0f);
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
		}

		if(i==0)
		{
			//left
			glColor3f(1.0f,0.0f,1.0f);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
		}

		if(i==31)
		{
			//right
			glColor3f(0.0f,1.0f,1.0f);
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
		}
	glEnd();

	glColor3f(1.0f,1.0f,1.0f);
}

void init_opengl()
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);	

	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable ( GL_COLOR_MATERIAL );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport     ( 0, 0, 1024, 768 );
	glMatrixMode   ( GL_PROJECTION );  // Select The Projection Matrix
	glLoadIdentity ( );                // Reset The Projection Matrix
	gluPerspective ( 80, ( float )1024 / ( float ) 768, 1.0, 15000.0 );
	glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
    glLoadIdentity ( );    // Reset The Model View Matrix
	glClearColor(0.5f, 0.5f, 1.0f, 0.5f);			
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
  
	SDL_Window *window = SDL_CreateWindow("CopyCraft", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	glMatrixMode(GL_PROJECTION|GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-320,320,240,-240,0,1);

	init_opengl();

	float x = 0.0, y = 30.0;

	SDL_Event event; 
	bool done = 0;
	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT || event.type == SDL_QUIT)
				done = 1;
		}
		
		// render scene
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective ( 80, ( float )1024 / ( float ) 768, 1.0, 100000.0 );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		gluLookAt(0.0f,0.0f, 50.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
		
		int i,j,k;

		glTranslatef(-16,-16,-16);
		for(i=0; i<32; i++)
		{
			for(j=0; j<32; j++)
			{
				for(k=0; k<32; k++)
				{
					glPushMatrix();
						glTranslatef(i,j,k);
						draw_cube(i,j,k);
					glPopMatrix();
				}
			}
		}

		SDL_GL_SwapWindow(window);
    
	} 

	SDL_GL_DeleteContext(glcontext);  
  
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}