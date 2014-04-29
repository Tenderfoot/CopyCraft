#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")
#pragma comment(lib, "GLU32")
#pragma comment(lib, "OpenGL32")
#pragma comment(lib, "glew32")
#pragma comment(lib, "SOIL")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gl/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/GL.h>
#include <gl/GLU.h>
#include <SOIL.h>

#include "common.h"

#define WORLD_SIZE 32

t_levelBlock world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];
t_pos camera_pos;

// load in a second texture
// texture the two block types seperately

GLuint texture[2];

void init()
{
	int i, j, k;

	camera_pos.x = 0;
	camera_pos.y = 0;
	camera_pos.z = 5;

	texture[0] =  SOIL_load_OGL_texture
	(
		"data/dirt.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS 
	);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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

int get_number_of_faces(int i, int j, int k)
{
	int number_to_return = 0;
	
	if(check_block_exists(i,j,k+1) == false)
	{
		number_to_return++;
	}

	if(check_block_exists(i,j,k-1) == false)
	{
		number_to_return++;
	}

	if(check_block_exists(i,j-1,k) == false)
	{
		number_to_return++;
	}

	if(check_block_exists(i,j+1,k) == false)
	{
		number_to_return++;
	}

	if(check_block_exists(i-1,j,k) == false)
	{
		number_to_return++;
	}

	if(check_block_exists(i+1,j,k) == false)
	{
		number_to_return++;
	}

	return number_to_return;
}

int get_total_faces()
{
	int i,j,k;
	int total=0;

	for(i=0; i<WORLD_SIZE; i++)
	{
		for(j=0; j<WORLD_SIZE; j++)
		{
			for(k=0; k<WORLD_SIZE; k++)
			{	
				// block is only drawn if it exists
				if(world[i][j][k].type > 0)
				{
					total+=get_number_of_faces(i,j,k);
				}
			}
		}
	}
	return total;
}

int populate_verticies(float *vertex_buffer, float *color_buffer, float *texcoord_buffer)
{
	int i,j,k;
	int total=0;
	int current_face = 0;

	float *current_color;
	float brown[3];
	brown[0] = 1;
	brown[1] = 1;
	brown[2] = 1;
	current_color = brown;

	float green[3];
	green[0] = 1;
	green[1] = 1;
	green[2] = 1;

	float *current_texcoords;
	float dirt_texcoords[8];
	dirt_texcoords[0] = 0.0f;
	dirt_texcoords[1] = 0.0f;
	dirt_texcoords[2] = 0.5f;
	dirt_texcoords[3] = 0.0f;
	dirt_texcoords[4] = 0.5f;
	dirt_texcoords[5] = 1.0f;
	dirt_texcoords[6] = 0.0f;
	dirt_texcoords[7] = 1.0f;
	float grass_texcoords[8];
	grass_texcoords[0] = 0.5f;
	grass_texcoords[1] = 0.0f;
	grass_texcoords[2] = 1.0f;
	grass_texcoords[3] = 0.0f;
	grass_texcoords[4] = 1.0f;
	grass_texcoords[5] = 1.0f;
	grass_texcoords[6] = 0.5f;
	grass_texcoords[7] = 1.0f;


	for(i=0; i<WORLD_SIZE; i++)
	{
		for(j=0; j<WORLD_SIZE; j++)
		{
			for(k=0; k<WORLD_SIZE; k++)
			{
				if(world[i][j][k].type > 0)
				{
					if(world[i][j][k].type == 1)
						current_texcoords = dirt_texcoords;
					else
						current_texcoords = grass_texcoords;

					if(check_block_exists(i,j,k+1) == false)
					{
						texcoord_buffer[current_face*8] = current_texcoords[0];
						texcoord_buffer[current_face*8+1] = current_texcoords[1];
						texcoord_buffer[current_face*8+2] = current_texcoords[2];
						texcoord_buffer[current_face*8+3] = current_texcoords[3];
						texcoord_buffer[current_face*8+4] = current_texcoords[4];
						texcoord_buffer[current_face*8+5] = current_texcoords[5];
						texcoord_buffer[current_face*8+6] = current_texcoords[6];
						texcoord_buffer[current_face*8+7] = current_texcoords[7];

						color_buffer[current_face*12] = current_color[0];
						color_buffer[current_face*12+1] = current_color[1];
						color_buffer[current_face*12+2] = current_color[2];
						color_buffer[current_face*12+3] = current_color[0];
						color_buffer[current_face*12+4] = current_color[1];
						color_buffer[current_face*12+5] = current_color[2];
						color_buffer[current_face*12+6] = current_color[0];
						color_buffer[current_face*12+7] = current_color[1];
						color_buffer[current_face*12+8] = current_color[2];
						color_buffer[current_face*12+9] = current_color[0];
						color_buffer[current_face*12+10] = current_color[1];
						color_buffer[current_face*12+11] = current_color[2];

						vertex_buffer[current_face*12] = 0.5+i;
						vertex_buffer[current_face*12+1] = 0.5+j;
						vertex_buffer[current_face*12+2] = 0.5+k;
						vertex_buffer[current_face*12+3] = -0.5+i;
						vertex_buffer[current_face*12+4] = 0.5+j;
						vertex_buffer[current_face*12+5] = 0.5+k;
						vertex_buffer[current_face*12+6] = -0.5+i;
						vertex_buffer[current_face*12+7] = -0.5+j;
						vertex_buffer[current_face*12+8] = 0.5+k;
						vertex_buffer[current_face*12+9] = 0.5+i;
						vertex_buffer[current_face*12+10] = -0.5+j;
						vertex_buffer[current_face*12+11] = 0.5+k;
						current_face++;
					}

					if(check_block_exists(i,j,k-1) == false)
					{
						texcoord_buffer[current_face*8] = current_texcoords[0];
						texcoord_buffer[current_face*8+1] = current_texcoords[1];
						texcoord_buffer[current_face*8+2] = current_texcoords[2];
						texcoord_buffer[current_face*8+3] = current_texcoords[3];
						texcoord_buffer[current_face*8+4] = current_texcoords[4];
						texcoord_buffer[current_face*8+5] = current_texcoords[5];
						texcoord_buffer[current_face*8+6] = current_texcoords[6];
						texcoord_buffer[current_face*8+7] = current_texcoords[7];

						color_buffer[current_face*12] = current_color[0];
						color_buffer[current_face*12+1] = current_color[1];
						color_buffer[current_face*12+2] = current_color[2];
						color_buffer[current_face*12+3] = current_color[0];
						color_buffer[current_face*12+4] = current_color[1];
						color_buffer[current_face*12+5] = current_color[2];
						color_buffer[current_face*12+6] = current_color[0];
						color_buffer[current_face*12+7] = current_color[1];
						color_buffer[current_face*12+8] = current_color[2];
						color_buffer[current_face*12+9] = current_color[0];
						color_buffer[current_face*12+10] = current_color[1];
						color_buffer[current_face*12+11] = current_color[2];

						vertex_buffer[current_face*12] = 0.5+i;
						vertex_buffer[current_face*12+1] = 0.5+j;
						vertex_buffer[current_face*12+2] = -0.5+k;
						vertex_buffer[current_face*12+3] = -0.5+i;
						vertex_buffer[current_face*12+4] = 0.5+j;
						vertex_buffer[current_face*12+5] = -0.5+k;
						vertex_buffer[current_face*12+6] = -0.5+i;
						vertex_buffer[current_face*12+7] = -0.5+j;
						vertex_buffer[current_face*12+8] = -0.5+k;
						vertex_buffer[current_face*12+9] = 0.5+i;
						vertex_buffer[current_face*12+10] = -0.5+j;
						vertex_buffer[current_face*12+11] = -0.5+k;
						current_face++;
					}

					if(check_block_exists(i,j-1,k) == false)
					{

						texcoord_buffer[current_face*8] = current_texcoords[0];
						texcoord_buffer[current_face*8+1] = current_texcoords[1];
						texcoord_buffer[current_face*8+2] = current_texcoords[2];
						texcoord_buffer[current_face*8+3] = current_texcoords[3];
						texcoord_buffer[current_face*8+4] = current_texcoords[4];
						texcoord_buffer[current_face*8+5] = current_texcoords[5];
						texcoord_buffer[current_face*8+6] = current_texcoords[6];
						texcoord_buffer[current_face*8+7] = current_texcoords[7];

						color_buffer[current_face*12] = current_color[0];
						color_buffer[current_face*12+1] = current_color[1];
						color_buffer[current_face*12+2] = current_color[2];
						color_buffer[current_face*12+3] = current_color[0];
						color_buffer[current_face*12+4] = current_color[1];
						color_buffer[current_face*12+5] = current_color[2];
						color_buffer[current_face*12+6] = current_color[0];
						color_buffer[current_face*12+7] = current_color[1];
						color_buffer[current_face*12+8] = current_color[2];
						color_buffer[current_face*12+9] = current_color[0];
						color_buffer[current_face*12+10] = current_color[1];
						color_buffer[current_face*12+11] = current_color[2];

						vertex_buffer[current_face*12] = 0.5+i;
						vertex_buffer[current_face*12+1] = -0.5+j;
						vertex_buffer[current_face*12+2] = 0.5+k;
						vertex_buffer[current_face*12+3] = -0.5+i;
						vertex_buffer[current_face*12+4] = -0.5+j;
						vertex_buffer[current_face*12+5] = 0.5+k;
						vertex_buffer[current_face*12+6] = -0.5+i;
						vertex_buffer[current_face*12+7] = -0.5+j;
						vertex_buffer[current_face*12+8] = -0.5+k;
						vertex_buffer[current_face*12+9] = 0.5+i;
						vertex_buffer[current_face*12+10] = -0.5+j;
						vertex_buffer[current_face*12+11] = -0.5+k;
						current_face++;
					}

					if(check_block_exists(i,j+1,k) == false)
					{

						texcoord_buffer[current_face*8] = current_texcoords[0];
						texcoord_buffer[current_face*8+1] = current_texcoords[1];
						texcoord_buffer[current_face*8+2] = current_texcoords[2];
						texcoord_buffer[current_face*8+3] = current_texcoords[3];
						texcoord_buffer[current_face*8+4] = current_texcoords[4];
						texcoord_buffer[current_face*8+5] = current_texcoords[5];
						texcoord_buffer[current_face*8+6] = current_texcoords[6];
						texcoord_buffer[current_face*8+7] = current_texcoords[7];

						color_buffer[current_face*12] = current_color[0];
						color_buffer[current_face*12+1] = current_color[1];
						color_buffer[current_face*12+2] = current_color[2];
						color_buffer[current_face*12+3] = current_color[0];
						color_buffer[current_face*12+4] = current_color[1];
						color_buffer[current_face*12+5] = current_color[2];
						color_buffer[current_face*12+6] = current_color[0];
						color_buffer[current_face*12+7] = current_color[1];
						color_buffer[current_face*12+8] = current_color[2];
						color_buffer[current_face*12+9] = current_color[0];
						color_buffer[current_face*12+10] = current_color[1];
						color_buffer[current_face*12+11] = current_color[2];

						vertex_buffer[current_face*12] = 0.5+i;
						vertex_buffer[current_face*12+1] = 0.5+j;
						vertex_buffer[current_face*12+2] = 0.5+k;
						vertex_buffer[current_face*12+3] = -0.5+i;
						vertex_buffer[current_face*12+4] = 0.5+j;
						vertex_buffer[current_face*12+5] = 0.5+k;
						vertex_buffer[current_face*12+6] = -0.5+i;
						vertex_buffer[current_face*12+7] = 0.5+j;
						vertex_buffer[current_face*12+8] = -0.5+k;
						vertex_buffer[current_face*12+9] = 0.5+i;
						vertex_buffer[current_face*12+10] = 0.5+j;
						vertex_buffer[current_face*12+11] = -0.5+k;
						current_face++;
					}

					if(check_block_exists(i-1,j,k) == false)
					{
						texcoord_buffer[current_face*8] = current_texcoords[0];
						texcoord_buffer[current_face*8+1] = current_texcoords[1];
						texcoord_buffer[current_face*8+2] = current_texcoords[2];
						texcoord_buffer[current_face*8+3] = current_texcoords[3];
						texcoord_buffer[current_face*8+4] = current_texcoords[4];
						texcoord_buffer[current_face*8+5] = current_texcoords[5];
						texcoord_buffer[current_face*8+6] = current_texcoords[6];
						texcoord_buffer[current_face*8+7] = current_texcoords[7];

						color_buffer[current_face*12] = current_color[0];
						color_buffer[current_face*12+1] = current_color[1];
						color_buffer[current_face*12+2] = current_color[2];
						color_buffer[current_face*12+3] = current_color[0];
						color_buffer[current_face*12+4] = current_color[1];
						color_buffer[current_face*12+5] = current_color[2];
						color_buffer[current_face*12+6] = current_color[0];
						color_buffer[current_face*12+7] = current_color[1];
						color_buffer[current_face*12+8] = current_color[2];
						color_buffer[current_face*12+9] = current_color[0];
						color_buffer[current_face*12+10] = current_color[1];
						color_buffer[current_face*12+11] = current_color[2];

						vertex_buffer[current_face*12] = -0.5+i;
						vertex_buffer[current_face*12+1] = 0.5+j;
						vertex_buffer[current_face*12+2] = 0.5+k;
						vertex_buffer[current_face*12+3] = -0.5+i;
						vertex_buffer[current_face*12+4] = -0.5+j;
						vertex_buffer[current_face*12+5] = 0.5+k;
						vertex_buffer[current_face*12+6] = -0.5+i;
						vertex_buffer[current_face*12+7] = -0.5+j;
						vertex_buffer[current_face*12+8] = -0.5+k;
						vertex_buffer[current_face*12+9] = -0.5+i;
						vertex_buffer[current_face*12+10] = 0.5+j;
						vertex_buffer[current_face*12+11] = -0.5+k;
						current_face++;
					}

					if(check_block_exists(i+1,j,k) == false)
					{

						texcoord_buffer[current_face*8] = current_texcoords[0];
						texcoord_buffer[current_face*8+1] = current_texcoords[1];
						texcoord_buffer[current_face*8+2] = current_texcoords[2];
						texcoord_buffer[current_face*8+3] = current_texcoords[3];
						texcoord_buffer[current_face*8+4] = current_texcoords[4];
						texcoord_buffer[current_face*8+5] = current_texcoords[5];
						texcoord_buffer[current_face*8+6] = current_texcoords[6];
						texcoord_buffer[current_face*8+7] = current_texcoords[7];

						color_buffer[current_face*12] = current_color[0];
						color_buffer[current_face*12+1] = current_color[1];
						color_buffer[current_face*12+2] = current_color[2];
						color_buffer[current_face*12+3] = current_color[0];
						color_buffer[current_face*12+4] = current_color[1];
						color_buffer[current_face*12+5] = current_color[2];
						color_buffer[current_face*12+6] = current_color[0];
						color_buffer[current_face*12+7] = current_color[1];
						color_buffer[current_face*12+8] = current_color[2];
						color_buffer[current_face*12+9] = current_color[0];
						color_buffer[current_face*12+10] = current_color[1];
						color_buffer[current_face*12+11] = current_color[2];

						vertex_buffer[current_face*12] = 0.5+i;
						vertex_buffer[current_face*12+1] = 0.5+j;
						vertex_buffer[current_face*12+2] = 0.5+k;
						vertex_buffer[current_face*12+3] = 0.5+i;
						vertex_buffer[current_face*12+4] = -0.5+j;
						vertex_buffer[current_face*12+5] = 0.5+k;
						vertex_buffer[current_face*12+6] = 0.5+i;
						vertex_buffer[current_face*12+7] = -0.5+j;
						vertex_buffer[current_face*12+8] = -0.5+k;
						vertex_buffer[current_face*12+9] = 0.5+i;
						vertex_buffer[current_face*12+10] = 0.5+j;
						vertex_buffer[current_face*12+11] = -0.5+k;
						current_face++;
					}
				}
			}
		}
	}
	return current_face;
}


// This isn't used
// drawing in immediate mode
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

	printf("total faces drawn: %d\n", get_total_faces());

	int i,j,k;
	float rot_amount;

	//Initialise VBO - do only once, at start of program
	//Create a variable to hold the VBO identifier
	GLuint triangleVBO[2];

	float *verticies;
	float *colors;
	float *texcoords;
	verticies = new float[get_total_faces()*4*3];
	colors = new float[get_total_faces()*4*3];
	texcoords = new float[get_total_faces()*4*2];

	int num_faces_populated;
	num_faces_populated = populate_verticies(verticies, colors, texcoords);

	printf("Number of faces populated: %d\n", num_faces_populated);

	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers(3, &triangleVBO[0]);

	//Make the new VBO active
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO[0]);
	//Upload vertex data to the video device
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_faces_populated*4*3, verticies, GL_STATIC_DRAW);

	//Make the new VBO active
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO[1]);
	//Upload vertex data to the video device
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_faces_populated*4*3, colors, GL_STATIC_DRAW);

	//Make the new VBO active
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO[2]);
	//Upload vertex data to the video device
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_faces_populated*4*2, texcoords, GL_STATIC_DRAW);

	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO[0]);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO[1]);
	glColorPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO[2]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
 
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT || event.type == SDL_QUIT)
				done = 1;
		}
		
		glLoadIdentity();

		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		gluLookAt(16.0f,40.0f, 20.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);

		rot_amount = SDL_GetTicks()/10;

		glPushMatrix();
			//glRotatef(rot_amount,0,1,0);
			//Actually draw the triangle, giving the number of vertices provided
			glDrawArrays(GL_QUADS, 0, num_faces_populated*4);
		glPopMatrix();

		SDL_GL_SwapWindow(window);
	} 

	SDL_GL_DeleteContext(glcontext);  
  
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}