#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")
#pragma comment(lib, "GLU32")
#pragma comment(lib, "OpenGL32")
#pragma comment(lib, "glew32")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gl/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/GL.h>
#include <gl/GLU.h>

#include "common.h"

#define WORLD_SIZE 32

t_levelBlock world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];

// first get the size of the vertex array
// then create and populate it

void init()
{
	int i, j, k;

	srand (time(NULL));
	int block_type = 0;

	for(i=0; i<WORLD_SIZE; i++)
	{
		for(j=0; j<WORLD_SIZE; j++)
		{
			for(k=0; k<WORLD_SIZE; k++)
			{
				block_type =  rand() % 50;
				if(block_type>10)	
					world[i][j][k].type = DIRT;
				else
					world[i][j][k].type = NO_BLOCK;
			}
		}
	}

	int current_depth = WORLD_SIZE-1;
	bool found=false;
	// next set topsoil.
	for(i=0; i<WORLD_SIZE; i++)
	{
		for(k=0; k<WORLD_SIZE; k++)
		{
			while(found == false)
			{
				if(world[i][current_depth][k].type == 0 && current_depth<WORLD_SIZE)
					current_depth--;
				else
				{
					found = true;
					world[i][current_depth][k].type = TOPSOIL;
				}
			}
			found = false;
			current_depth=WORLD_SIZE-1;
		}
	}
}

bool check_block_exists(int i, int j, int k)
{
	if(i>WORLD_SIZE-1 || i<0 || j>WORLD_SIZE-1 || j<0 || k>WORLD_SIZE-1 || k<0) 
		return false;
	else
	{
		return  world[i][j][k].type > 0;
	}
}

void draw_cube(int i, int j, int k)
{
	glBindTexture(GL_TEXTURE_2D, NULL);

	// draw different faces of the cube
	// only if there isn't another cube
	// adjacent on that side

	glBegin(GL_QUADS);
		
		if(world[i][j][k].type == DIRT)
		{
			glColor3f(1.0f,0.5f,0.1f);
		}
		else if(world[i][j][k].type == TOPSOIL)
		{
			glColor3f(0.0f,0.8f,0.0f);
		}		

		if(check_block_exists(i,j,k+1) == false)
		{
			//front
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
		}

		if(check_block_exists(i,j,k-1) == false)
		{
			//back
			glVertex3f(0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
		}

		if(check_block_exists(i,j-1,k) == false)
		{
			//top
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
		}

		if(check_block_exists(i,j+1,k) == false)
		{
			//bottom
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
		}

		if(check_block_exists(i-1,j,k) == false)
		{
			//left
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
		}

		if(check_block_exists(i+1,j,k) == false)
		{
			//right
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

	init_opengl();

	glewInit();

	init();

	float x = 0.0, y = 30.0;

	SDL_Event event; 
	bool done = 0;

	int i,j,k;
	float rot_amount;
	
	GLuint world_display_list = glGenLists (1);
	glNewList(world_display_list, GL_COMPILE);
	for(i=0; i<WORLD_SIZE; i++)
	{
		for(j=0; j<WORLD_SIZE; j++)
		{
			for(k=0; k<WORLD_SIZE; k++)
			{
				if(world[i][j][k].type > 0)
				{
					glPushMatrix();
						//glTranslatef(i*1.5,j*1.5,k*1.5);
						glTranslatef(i-16,j-16,k-16);
						draw_cube(i,j,k);
					glPopMatrix();
				}
			}
		}
	}

	glEndList();

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
		
		gluLookAt(0.0f,5.0f, 5.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);

		rot_amount = SDL_GetTicks()/10;

		//Initialise VBO - do only once, at start of program
		//Create a variable to hold the VBO identifier
		GLuint triangleVBO;
 
		//Vertices of a triangle (counter-clockwise winding)
		float data[] = {1.0, 0.0, 1.0, 0.0, 0.0, -1.0, -1.0, 0.0, 1.0};
		//try float data[] = {0.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, -1.0, 0.0}; if the above doesn't work.
 
		//Create a new VBO and use the variable id to store the VBO id
		glGenBuffers(1, &triangleVBO);
 
		//Make the new VBO active
		glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
 
		//Upload vertex data to the video device
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
 
		//Make the new VBO active. Repeat here incase changed since initialisation
		glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
 
		//Draw Triangle from VBO - do each time window, view point or data changes
		//Establish its 3 coordinates per vertex with zero stride in this array; necessary here
		glVertexPointer(3, GL_FLOAT, 0, NULL);
 
		//Establish array contains vertices (not normals, colours, texture coords etc)
		glEnableClientState(GL_VERTEX_ARRAY);
 
		//Actually draw the triangle, giving the number of vertices provided
		glDrawArrays(GL_TRIANGLES, 0, sizeof(data) / sizeof(float) / 3);
 
		//Force display to be drawn now
		glFlush();

		SDL_GL_SwapWindow(window);
    
	} 

	SDL_GL_DeleteContext(glcontext);  
  
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}