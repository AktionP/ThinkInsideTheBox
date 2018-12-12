// CS370 - Fall 2018
// Final Project
// Written By Kyle M. Eidson

#ifdef OSX
	#include <GLUT/glut.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
#endif
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lighting.h"
#include "materials.h"
#include "ObjectVars.h"

// Shader file utility functions
#include "shaderutils.h"

#define X 0
#define Y 1
#define Z 2
#define NO_TEXTURES 9
#define CARPET 0
#define WALLPAPER 1
#define ROOF 2
#define SODA 3
#define DOOR 4
#define WINDOW 5
#define ART 6
#define WOODTABLE 7
#define ENVIRONMENT 8

// Texture indices
GLuint tex_ids[NO_TEXTURES];

// Texture Files
char texture_files[NO_TEXTURES][40] = {"carpetTexture.jpg", "wallpaper.jpg",
"popcornCieling.jpg", "sodaCan.jpg", "woodDoor.jpg", "window.jpg", "shrek.jpg", "woodTable.jfif", "blank.bmp" };

// Point Light Parameters
GLfloat light0_pos[] = { 0.0f, 1.5f, 0.0f, 1.0 };

// Camera movement variables
GLfloat atX = 0.0f;
GLfloat atZ = 0.0f;
GLfloat atY = 0.0f;

// Global camera vectors
GLfloat eye[3] = { -1.0f,1.0f, 0.0f };
GLfloat at[3] = { 0.0f,1.0f,0.0f };
GLfloat up[3] = { 0.0f,1.0f,0.0f };

// Perspective Camera variables
GLfloat left = -0.1f;
GLfloat right = 0.1f;
GLfloat bottom = -0.1f;
GLfloat top = 0.1f;
GLfloat nearClip = 0.1f;
GLfloat farClip = 8.0f;

// Shadow Matrices
GLfloat M1_s[16]; // Table
GLfloat M2_s[16]; // Chair
GLfloat M3_s[16]; // Bowl
GLfloat M4_s[16]; // Sodacan

// Shadow color
GLfloat shadow_color[] = {0.2f, 0.2f, 0.2f};

// Global screen dimensions
GLint ww, hh;

// Cube vertices
GLfloat cube[][3] ={{ -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, 1.0f },
					{ -1.0f, -1.0f, 1.0f },  { -1.0f, 1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f },
					{ 1.0f, 1.0f, 1.0f },    { -1.0f, 1.0f, 1.0f }};

// Cube Texture Coords
GLfloat cube_tex[][2] = { {0.0f, 0.0f}, {12.0f, 0.0f}, {12.0f, 8.0f}, {0.0f, 8.0f},
						{0.0f, 0.0f}, {15.0f, 0.0f}, {15.0f, 4.0f}, {0.0f, 4.0f},
						{0.0f, 0.0f}, {12.0f, 0.0f}, {12.0f, 8.0f}, {0.0f, 8.0f},
						{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
						{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 2.0f}, {0.0f, 2.0f} };

// Cube texture Mappingd
#define LEFTWALL 1
#define RIGHTWALL 2
#define FRONTWALL 3
#define BACKWALL 4
#define POPCORN 5
#define WOODDOOR 6
#define WOODTABLE 7

// Display Lists
#define TABLE 1
#define CHAIR 2
#define FAN 3

// Shader files
GLchar* defaultVertexFile = "defaultvert.vs";
GLchar* defaultFragmentFile = "defaultfrag.fs";
GLchar* lightVertexFile = "lightvert.vs";
GLchar* lightFragmentFile = "lightfrag.fs";
GLchar* texVertexFile = "texturevert.vs";
GLchar* texFragmentFile = "texturefrag.fs";

// Object Quadrics
GLUquadricObj* quad;

// Shader objects
GLuint defaultShaderProg;
GLuint lightShaderProg;
GLuint textureShaderProg;
GLuint numLights_param;
GLint numLights = 2;
GLint texSampler;

// Animation Variables
GLint time = 0;
GLint lasttime = 0;
GLint fps = 30;
bool lightOn = false;
bool fanOn = false;
bool blindsOpen = false;
GLfloat fanTheta = 0.0f;
GLfloat fanDelta = 0.5f;
GLfloat lightSwitch = -45.0f;
GLfloat fanSwitch = -45.0f;
GLfloat delta = 0.1f;
GLfloat phi = 0.0f;
GLfloat theta = 0.0f;

GLenum lights[4] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3 };

void display();
void render_Scene(bool shadow);
void createMirror();
void renderMirror();
void keyfunc(unsigned char key, int x, int y);
void mousefunc(int x, int y);
void idlefunc();
void reshape(int w, int h);
void createWalls();
void createCeiling();
void createFloor();
void createTable();
void createTableShadow();
void createChair();
void createCeilingFan();
void createBowl();
void createFruit();
void createSodaCan();
void createSodaCanShadow();
void createGlassWithSoda();
void createSwitch();
void createDoor();
void createWindow();
void createBlinds();
void createArt();
bool load_textures();
void texturecube(int cubeVec);
void texquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat t1[], GLfloat t2[], GLfloat t3[], GLfloat t4[]);


int main(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc,argv);

	// Initialize the window with double buffering and RGB colors
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Set the window size to image size
	glutInitWindowSize(800,800);

	// Create window
	glutCreateWindow("House");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);
	glutReshapeFunc(reshape);
//	glutMouseFunc(mousefunc);

	// Create Display Lists
	createChair();
	createCeilingFan();

	// Set background color to white
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Initialize Shadow Matrices
	for (int i = 0; i < 16; i++)
	{
		M1_s[i] = 0.0f;
		M2_s[i] = 0.0f;
		M3_s[i] = 0.0f;
		M4_s[i] = 0.0f;
	}
	M1_s[0] = M1_s[5] = M1_s[10] = 1.0f;
	M2_s[0] = M2_s[5] = M2_s[10] = 1.0f;
	M3_s[0] = M3_s[5] = M3_s[10] = 1.0f;
	M4_s[0] = M4_s[5] = M4_s[10] = 1.0f;
	M1_s[7] = M2_s[7] = M3_s[7] = M4_s[7] =  -1.0f / light0_pos[1];


	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create quadrics
	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);
	
	// Load shader programs
	defaultShaderProg = load_shaders(defaultVertexFile,defaultFragmentFile);
	lightShaderProg = load_shaders(lightVertexFile, lightFragmentFile);
	textureShaderProg = load_shaders(texVertexFile, texFragmentFile);

	numLights_param = glGetUniformLocation(lightShaderProg, "numLights");
	texSampler = glGetUniformLocation(textureShaderProg, "texMap");

	if (!load_textures())
	{
		exit(0);
	}

	// Begin event loop
	glutMainLoop();
	return 0;
}

// Display callback
void display()
{
	// Create Mirror Texture
	createMirror();

	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Adjust viewing volume (orthographic)
	GLfloat xratio = 1.0f;
	GLfloat yratio = 1.0f;
	// If taller than wide adjust y
	if (ww <= hh)
	{
		yratio = (GLfloat)hh / (GLfloat)ww;
	}
	// If wider than tall adjust x
	else if (hh <= ww)
	{
		xratio = (GLfloat)ww / (GLfloat)hh;
	}
	glFrustum(left, right, bottom, top, nearClip, farClip);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], at[X] + atX, at[Y], at[Z] + atZ, up[X], up[Y], up[Z]);

	// Render scene without shadows
	render_Scene(false);

	// Render scene with shadows
	render_Scene(true);

	// Render Mirror
	renderMirror();

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

// Scene render function
void render_Scene(bool shadow)
{
	// Lights on or off
	set_PointLight(GL_LIGHT0, (lightOn ? &white_light : &grey_light), light0_pos);

	// Create Objects that won't have shadows
	if (!shadow) {
		//Ceiling
		createCeiling();
		// Walls
		createWalls();
		// Floor
		createFloor();
		// Table
		createTable();
		// Door
		createDoor();
		// Window
		createWindow();
		// Piece of Art
		createArt();
		//Light anf Fan switch
		createSwitch();
		// Glass with soda
		glPushMatrix();
			glScalef(SODACANSCALEX, SODACANSCALEY, SODACANSCALEZ);
			glTranslatef(SODACANLOCATX, SODACANLOCATY, SODACANLOCATZ);
			createSodaCan();
		glPopMatrix();
		createGlassWithSoda();

		// Blinds
		glPushMatrix();
		for (int i = 0; i < NUMBLINDS; i++) {
			if (blindsOpen)
				glTranslatef(0.0f, 0.0f, BLINDSHFTVAL);
			createBlinds();
		}
		glPopMatrix();

	}

	//Fan
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &brass);
		glTranslatef(FANLOCATX, FANLOCATY, FANLOCATZ);
		glRotatef(fanTheta, 0.0f, 1.0f, 0.0f);
		glCallList(FAN);
	glPopMatrix();

	// Creating objects that will either cast shadows or create the shadows for non-shadowable objects
	// Object that will create shadow for table
	glPushMatrix();
		if(shadow && lightOn)
		{
			glTranslatef(light0_pos[0], light0_pos[1] + 0.01f, light0_pos[2]);
			glMultMatrixf(M1_s);
			glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

			glUseProgram(defaultShaderProg);

			glColor3fv(shadow_color);
		}
		else
		{
			glUseProgram(lightShaderProg);
			glUniform1i(numLights_param, numLights);
			set_material(GL_FRONT_AND_BACK, &silver);
		}
		createTableShadow();
	glPopMatrix();

	// Bowl
	glPushMatrix();
	if (shadow && lightOn)
	{
		glTranslatef(light0_pos[0], light0_pos[1] + 0.64f, light0_pos[2]);
		glMultMatrixf(M1_s);
		glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

		glUseProgram(defaultShaderProg);

		glColor3fv(shadow_color);
		glScalef(0.5f, 1.0f, 0.5f);
	}
	else 
	{
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &brass);
	}
	createBowl();
	glPopMatrix();
	
	// Spherical fruit
	glPushMatrix();
	if (shadow && lightOn)
	{
		glTranslatef(light0_pos[0], light0_pos[1] + 0.66f, light0_pos[2]);
		glMultMatrixf(M1_s);
		glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

		glUseProgram(defaultShaderProg);

		glColor3fv(shadow_color);
		glScalef(0.5f, 1.0f, 0.5f);
	}
	else
	{
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &copper);
	}
	createFruit();
	glPopMatrix();

	// Object that will cast shadow for soda can
	glPushMatrix();
	if (shadow && lightOn)
	{
		glTranslatef(light0_pos[0], light0_pos[1] + 0.601f, light0_pos[2]);
		glMultMatrixf(M1_s);
		glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

		glUseProgram(defaultShaderProg);

		glColor3fv(shadow_color);
		glScalef(0.5f, 1.0f, 0.5f);
	}
	else
	{
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &silver);
	}
	glScalef(SODACANSCALEX, SODACANSCALEY, SODACANSCALEZ);
	glTranslatef(SODACANLOCATX, SODACANLOCATY, SODACANLOCATZ);
	createSodaCanShadow();
	glPopMatrix();

	// Chairs
	glPushMatrix();
	if (shadow && lightOn) 
	{
		glTranslatef(light0_pos[0], light0_pos[1] + 0.01f, light0_pos[2]);
		glMultMatrixf(M1_s);
		glTranslatef(-light0_pos[0], -light0_pos[1], -light0_pos[2]);

		glUseProgram(defaultShaderProg);

		glColor3fv(shadow_color);

	}
	else
	{
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &darkWood);
	}
		glPushMatrix();
			glTranslatef(CHAIR1LOCATX, CHAIR1LOCATY, CHAIR1LOCATZ);
			glCallList(CHAIR);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(CHAIR2LOCATX, CHAIR2LOCATY, CHAIR2LOCATZ);
			glRotatef(90, 0.0f, 1.0f, 0.0f);
			glCallList(CHAIR);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(CHAIR3LOCATX, CHAIR3LOCATY, CHAIR3LOCATZ);
			glRotatef(180, 0.0f, 1.0f, 0.0f);
			glCallList(CHAIR);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(CHAIR4LOCATX, CHAIR4LOCATY, CHAIR4LOCATZ);
			glRotatef(-90, 0.0f, 1.0f, 0.0f);
			glCallList(CHAIR);
		glPopMatrix();
	glPopMatrix();
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// Move Forward
	if (key == 'w' || key == 'W')
	{
		eye[X] += (at[X] - eye[X]) * delta;
		eye[Z] += (at[Z] - eye[Z]) * delta;
	}
	// Move Backward
	else if (key == 's' || key == 'S')
	{
		eye[X] -= (at[X] - eye[X]) * delta;
		eye[Z] -= (at[Z] - eye[Z]) * delta;
	}
	// Turn Left
	else if (key == 'a' || key == 'A')
	{
		theta -= delta;
	}
	// Turn Right
	else if (key == 'd' || key == 'D')
	{
		theta += delta;
	}

	if (key == 'z' || key == 'Z')
	{
		phi += delta;
	}
	else if (key == 'x' || key == 'X')
	{
		phi -= delta;
	}

	if (key == 'f' || key == 'F')
	{
		if (fanOn) {
			fanOn = false;
			fanSwitch = -FANSWITCHTHETA;
		}
		else {
			fanOn = true;
			fanSwitch = FANSWITCHTHETA;
		}
	}

	if (key == 'o' || key == 'O')
	{
		blindsOpen = !blindsOpen;
	}

	if (key == 'l' || key == 'L')
	{
		if (lightOn) {
			lightOn = false;
			lightSwitch = -LIGHTSWITCHTHETA;
		}
		else {
			lightOn = true;
			lightSwitch = LIGHTSWITCHTHETA;
		}		
	}

	// <esc> quits
	if (key == 27)
	{
		exit(0);
	}

	if (eye[X] > POSXBOUNDS) {
		eye[X] = POSXBOUNDS;
	}
	else if (eye[X] < NEGXBOUNDS) {
		eye[X] = NEGXBOUNDS;
	} 
	if (eye[Z] > POSZBOUNDS) {
		eye[Z] = POSZBOUNDS;
	}
	else if (eye[Z] < NEGZBOUNDS) {
		eye[Z] = NEGZBOUNDS;
	}



	at[X] = eye[X] + GLfloat(cos(theta));
	at[Z] = eye[Z] + GLfloat(sin(theta));
	at[Y] = eye[Y] + phi;

	glutPostRedisplay();
}

void mousefunc(int x, int y)
{
	int xpos, ypos;
	
}

// Idle callback
void idlefunc()
{
	if (fanOn) 
	{
		time = glutGet(GLUT_ELAPSED_TIME);

		if (time - lasttime > 1000.0 / fps)
		{
			fanTheta += fanDelta;
			if (fanTheta > 360.0f)
				fanTheta = 0.0f;
		}

		glutPostRedisplay();

	}
}

// Reshape callback
void reshape(int w, int h)
{
	// Set new screen extents
	glViewport(0, 0, w, h);

	// Store new extents
	ww = w;
	hh = h;
}

// Creates the Walls
void createWalls() 
{
	//Left Wall
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER]);
		glScalef(LEFTWALLSCALEX, LEFTWALLSCALEY, LEFTWALLSCALEZ);
		glTranslatef(LEFTWALLLOCATX, LEFTWALLLOCATY, LEFTWALLLOCATZ);
		texturecube(LEFTWALL);
	glPopMatrix();


	// Right Wall
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER]);
		glScalef(RIGHTWALLSCALEX, RIGHTWALLSCALEY, RIGHTWALLSCALEZ);
		glTranslatef(RIGHTWALLLOCATX, RIGHTWALLLOCATY, RIGHTWALLLOCATZ);
		texturecube(RIGHTWALL);
	glPopMatrix();

	// Front Wall
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER]);
		glScalef(FRONTWALLSCALEX, FRONTWALLSCALEY, FRONTWALLSCALEZ);
		glTranslatef(FRONTWALLLOCATX, FRONTWALLLOCATY, FRONTWALLLOCATZ);
		texturecube(FRONTWALL);
	glPopMatrix();

	// Back Wall
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER]);
		glScalef(BACKWALLSCALEX, BACKWALLSCALEY, BACKWALLSCALEZ);
		glTranslatef(BACKWALLLOCATX, BACKWALLLOCATY, BACKWALLLOCATZ);
		texturecube(BACKWALL);
	glPopMatrix();
}

// Creates the Ceiling
void createCeiling() 
{
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[ROOF]);
		glScalef(CEILINGSCALEX, CEILINGSCALEY, CEILINGSCALEZ);
		glTranslatef(CEILINGLOCATX, CEILINGLOCATY, CEILINGLOCATZ);
		texturecube(POPCORN);
	glPopMatrix();
}

// Creates the Floor
void createFloor()
{
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[CARPET]);
		glScalef(FLOORSCALEX, FLOORSCALEY, FLOORSCALEZ);
		texturecube(CARPET);
	glPopMatrix();
}

// Creates the table
void createTable() 
{
	// Table Top
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[WOODTABLE]);
		glScalef(TABLETOPSCALEX, TABLETOPSCALEY, TABLETOPSCALEZ);
		glTranslatef(TABLETOPLOCATX, TABLETOPLOCATY, TABLETOPLOCATZ);
		texturecube(WOODTABLE);
	glPopMatrix();

	// Table Legs
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &silver);
		glScalef(TABLELEGSCALEX, TABLELEGSCALEY, TABLELEGSCALEZ);
		glTranslatef(TABLELEGLOCATX, TABLELEGLOCATY, TABLELEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &silver);
		glScalef(TABLELEGSCALEX, TABLELEGSCALEY, TABLELEGSCALEZ);
		glTranslatef(-TABLELEGLOCATX, TABLELEGLOCATY, TABLELEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &silver);
		glScalef(TABLELEGSCALEX, TABLELEGSCALEY, TABLELEGSCALEZ);
		glTranslatef(-TABLELEGLOCATX, TABLELEGLOCATY, -TABLELEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &silver);
		glScalef(TABLELEGSCALEX, TABLELEGSCALEY, TABLELEGSCALEZ);
		glTranslatef(TABLELEGLOCATX, TABLELEGLOCATY, -TABLELEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();
}

// Creates a table to be used for shadows
void createTableShadow()
{
	glPushMatrix();
		glScalef(TABLETOPSHADOWSCALEX, TABLETOPSHADOWSCALEY, TABLETOPSHADOWSCALEZ);
		glTranslatef(TABLETOPLOCATX, TABLETOPLOCATY, TABLETOPLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();
}

// Creates a chair
void createChair() 
{
	glNewList(CHAIR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	// Chair Bottom
	glPushMatrix();
		glScalef(CHAIRBOTTOMSCALEX, CHAIRBOTTOMSCALEY, CHAIRBOTTOMSCALEZ);
		glTranslatef(CHAIRBOTTOMLOCATX, CHAIRBOTTOMLOCATY, CHAIRBOTTOMLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	// Chair legs
	glPushMatrix();
		glScalef(CHAIRLEGSCALEX , CHAIRLEGSCALEY, CHAIRLEGSCALEZ);
		glTranslatef(CHAIRLEGLOCATX, CHAIRLEGLOCATY, CHAIRLEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glScalef(CHAIRLEGSCALEX, CHAIRLEGSCALEY, CHAIRLEGSCALEZ);
		glTranslatef(-CHAIRLEGLOCATX, CHAIRLEGLOCATY, CHAIRLEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glScalef(CHAIRLEGSCALEX, CHAIRLEGSCALEY, CHAIRLEGSCALEZ);
		glTranslatef(-CHAIRLEGLOCATX, CHAIRLEGLOCATY, -CHAIRLEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glScalef(CHAIRLEGSCALEX, CHAIRLEGSCALEY, CHAIRLEGSCALEZ);
		glTranslatef(CHAIRLEGLOCATX, CHAIRLEGLOCATY, -CHAIRLEGLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	// Chair back
	glPushMatrix();
		glScalef(CHAIRBACKSCALEX, CHAIRBACKSCALEY, CHAIRBACKSCALEZ);
		glTranslatef(CHAIRBACKLOCATX, CHAIRBACKLOCATY, CHAIRBACKLOCATZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPopAttrib();
	glEndList();
}

// Creates the ceiling fan
void createCeilingFan() 
{
	glNewList(FAN, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	// Ceiling Light
	glPushMatrix();
		glScalef(0.1f, 0.1f, 0.1f);
		glutSolidSphere(1.0, 32, 32);
	glPopMatrix();

	// Ceiling Fans
	glPushMatrix();
		glTranslatef(CEILINGFANBLADE1LOCATX, CEILINGFANBLADE1LOCATY, CEILINGFANBLADE1LOCATZ);
		glRotatef(90, 0.0f, 0.0f, 1.0f);
		glScalef(CEILINGFANBLADESCALE1X, CEILINGFANBLADESCALE1Y, CEILINGFANBLADESCALE1Z);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(CEILINGFANBLADE2LOCATX, CEILINGFANBLADE2LOCATY, CEILINGFANBLADE2LOCATZ);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		glScalef(CEILINGFANBLADESCALE2X, CEILINGFANBLADESCALE2Y, CEILINGFANBLADESCALE2Z);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(CEILINGFANBLADE3LOCATX, CEILINGFANBLADE3LOCATY, CEILINGFANBLADE3LOCATZ);
		glRotatef(-90, 0.0f, 0.0f, 1.0f);
		glScalef(CEILINGFANBLADESCALE1X, CEILINGFANBLADESCALE1Y, CEILINGFANBLADESCALE1Z);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(CEILINGFANBLADE4LOCATX, CEILINGFANBLADE4LOCATY, CEILINGFANBLADE4LOCATZ);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		glScalef(CEILINGFANBLADESCALE2X, CEILINGFANBLADESCALE2Y, CEILINGFANBLADESCALE2Z);
		glutSolidCube(1.0);
	glPopMatrix();

	// Fan Connector
	glPushMatrix();
		glTranslatef(CEILINGFANCONNECTLOCATX, CEILINGFANCONNECTLOCATY, CEILINGFANCONNECTLOCATZ);
		glScalef(CEILINGFANCONNECTSCALEX, CEILINGFANCONNECTSCALEY, CEILINGFANCONNECTSCALEZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPopAttrib();
	glEndList();
}

// Creates the Bowl that holds the fruit
void createBowl() 
{
	// Base
	glPushMatrix();
		glTranslatef(BOWLLOCATX, BOWLLOCATY, BOWLLOCATZ);
		glRotatef(180, 0.0f, 0.0f, 1.0f);
		glScalef(BOWLBASESCALEX, BOWLBASESCALEY, BOWLBASESCALEZ);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		gluDisk(quad, 0.0f, 1.0f, 32, 1.0);
	glPopMatrix();

	// Side
	glPushMatrix();
		glTranslatef(BOWLLOCATX, BOWLLOCATY, BOWLLOCATZ);
		glRotatef(180, 0.0f, 0.0f, 1.0f);
		glScalef(BOWLSIDESCALEX, BOWLSIDESCALEY, BOWLSIDESCALEZ);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		gluCylinder(quad, 1.0f, 1.5f, 0.5f, 32, 32);
	glPopMatrix();
}

// Creates the fruit
void createFruit()
{
	glPushMatrix();
		glTranslatef(FRUITLOCATX -0.1f, FRUITLOCATY, FRUITLOCATZ);
		glScalef(FRUITSCALEX, FRUITSCALEY, FRUITSCALEZ);
		glutSolidSphere(1.0f, 32, 32);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(FRUITLOCATX + 0.1f, FRUITLOCATY, FRUITLOCATZ);
		glScalef(FRUITSCALEX, FRUITSCALEY, FRUITSCALEZ);
		glutSolidSphere(1.0f, 32, 32);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(FRUITLOCATX, FRUITLOCATY, FRUITLOCATZ - 0.1f);
		glScalef(FRUITSCALEX, FRUITSCALEY, FRUITSCALEZ);
		glutSolidSphere(1.0f, 32, 32);
	glPopMatrix();
}

// Creates the Soda Can
void createSodaCan() 
{
	// Can
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[SODA]);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		gluCylinder(quad, 0.25, 0.25, 0.75, 32, 32);
	glPopMatrix();

	// Top
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &silver);
		glTranslatef(0.0f, 0.75f, 0.0f);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		gluPartialDisk(quad, 0.0, 0.25, 32, 1, 0, 360);
	glPopMatrix();
}

// Creates object that will cast soda can shadow
void createSodaCanShadow()
{
	// Can
	glPushMatrix();
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		gluCylinder(quad, 0.25, 0.25, 0.75, 32, 32);
	glPopMatrix();

	// Top
	glPushMatrix();
		glTranslatef(0.0f, 0.75f, 0.0f);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		gluPartialDisk(quad, 0.0, 0.25, 32, 1, 0, 360);
	glPopMatrix();

}

// Creates the transluscient glass with soda inside it
void createGlassWithSoda() 
{
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &white_acrylic);
		glDepthMask(GL_FALSE);
		glScalef(GLASSWITHSODASCALEX, GLASSWITHSODASCALEY, GLASSWITHSODASCALEZ);
		glTranslatef(GLASSWITHSODALOCATX, GLASSWITHSODALOCATY, GLASSWITHSODALOCATZ);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quad, 0.25, 0.25, 0.75, 90, 90);
		glDepthMask(GL_TRUE);
	glPopMatrix();

	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &redRubber);
		glScalef(GLASSWITHSODASCALEX, GLASSWITHSODASCALEY, GLASSWITHSODASCALEZ);
		glTranslatef(GLASSWITHSODALOCATX, GLASSWITHSODALOCATY, GLASSWITHSODALOCATZ);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quad, 0.2, 0.2, 0.7, 90, 90);
	glPopMatrix();
}

// Creates the light switch
void createSwitch()
{
	glPushMatrix();
	glTranslatef(SWITCHLOCATX, SWITCHLOCATY, SWITCHLOCATZ);

	// Create the block
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &perl);
		glScalef(0.2, 0.2, 0.1);
		glTranslatef(0.0f, 4.0f, 0.0f);
		glutSolidCube(1.0);
	glPopMatrix();

	// Light Switch
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &perl);
		glTranslatef(LIGHTSWITCHLOCATX, LIGHTSWITCHLOCATY, LIGHTSWITCHLOCATZ);
		glRotatef(lightSwitch, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, -0.1f);
		glScalef(SWITCHSCALEX, SWITCHSCALEY, SWITCHSCALEZ);
		glutSolidCube(1.0);
	glPopMatrix();

	// Fan Switch
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &perl);
		glTranslatef(FANSWITCHLOCATX, FANSWITCHLOCATY, FANSWITCHLOCATZ);
		glRotatef(fanSwitch, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, -0.1f);
		glScalef(SWITCHSCALEX, SWITCHSCALEY, SWITCHSCALEZ);
		glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

// Creates the wooden door
void createDoor()
{
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[DOOR]);
		glTranslatef(DOORLOCATX, DOORLOCATY, DOORLOCATZ);
		glScalef(DOORSCALEX, DOORSCALEY, DOORSCALEZ);
		glRotatef(90, 0.0f, 0.0f, 1.0f);
		texturecube(WOODDOOR);
	glPopMatrix();
}

// Creates the window
void createWindow()
{
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[WINDOW]);
		glTranslatef(WINDOWLOCATX, WINDOWLOCATY, WINDOWLOCATZ);
		glRotatef(90, 0.0f, 1.0f, 0.0f);
		glScalef(WINDOWSCALEX, WINDOWSCALEY, WINDOWSCALEZ);
		glRotatef(90, 0.0f, 0.0f, 1.0f);
		texturecube(WOODDOOR);
	glPopMatrix();
}

// Creates the blinds
void createBlinds()
{
	glPushMatrix();
		glUseProgram(lightShaderProg);
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);
		set_material(GL_FRONT_AND_BACK, &perl);
		glTranslatef(BLINDLOCATX, BLINDLOCATY, BLINDLOCATZ);
		glScalef(BLINDSCALEX, BLINDSCALEY, BLINDSCALEZ);
		glutSolidCube(1.0);
	glPopMatrix();
}

// Creates the Mirror
void createMirror()
{
	// PASS 1 - Render reflected scene
	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set projection matrix for flat "mirror" camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 8);

	// Set modelview matrix positioning "mirror" camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-1.7, 1.0, 0.0, 0.0, 1.5, 0.0, up[X], up[Y], up[Z]);

	// Render scene from mirror
	glPushMatrix();
		render_Scene(false);
		render_Scene(true);
	glPopMatrix();

	glFinish();

	// Copy scene to texture
	glBindTexture(GL_TEXTURE_2D, tex_ids[ENVIRONMENT]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 512, 512, 0);
}

// Renders the Mirror
void renderMirror()
{
	glPushMatrix();
	glUseProgram(textureShaderProg);
	glUniform1i(texSampler, 0);
	// Draw mirror surface
	glBindTexture(GL_TEXTURE_2D, tex_ids[ENVIRONMENT]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.7f, 0.5f, -0.5f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.7f, 1.5f, -0.5f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-1.7f, 1.5f, 0.5f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-1.7f, 0.5f, 0.5f);
	glEnd();

	glUseProgram(defaultShaderProg);
	glColor3f(0.0f, 0.0f, 0.0f);
	// Draw mirror frame
	glBegin(GL_LINE_LOOP);
		glVertex3f(-1.7f, 0.49f, 0.51f);
		glVertex3f(-1.7f, 0.49f, -0.51f);
		glVertex3f(-1.7f, 1.51f, -0.51f);
		glVertex3f(-1.7f, 1.51f, 0.51f);
	glEnd();

	glPopMatrix();
}

// Creates the Art
void createArt()
{
	glPushMatrix();
		glUseProgram(textureShaderProg);
		glUniform1i(texSampler, 0);
		glBindTexture(GL_TEXTURE_2D, tex_ids[ART]);
		glTranslatef(ARTLOCATX, ARTLOCATY, ARTLOCATZ);
		glScalef(ARTSCALEX, ARTSCALEY, ARTSCALEZ);
		glRotatef(-90, 0.0f, 0.0f, 1.0f);
		texturecube(WOODDOOR);
	glPopMatrix();
}

// Texture Loading Functions
// Routine to load textures using SOIL
bool load_textures()
{
	tex_ids[ENVIRONMENT] = SOIL_load_OGL_texture(texture_files[8], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	// TODO: Set environment map properties if successfully loaded
	if (tex_ids[ENVIRONMENT] != 0)
	{
		// Set scaling filters (no mipmap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Set wrapping modes (clamped)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	}
	// Otherwise texture failed to load
	else
	{
		return false;
	}

	for (int i = 0; i < NO_TEXTURES-1; i++)
	{
		tex_ids[i] = SOIL_load_OGL_texture(texture_files[i], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

		// Set texture properties if successfully loaded
		if (tex_ids[i] != 0 || tex_ids[i] != 12)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		}
		// Otherwise texture failed to load
		else
		{
			return false;
		}
	}

	return true;
}

// Routine to draw textured cube
void texturecube(int cubeVec)
{
	if (cubeVec == CARPET) {

		// Top face
		texquad(cube[4], cube[7], cube[6], cube[5], cube_tex[0], cube_tex[1], cube_tex[2], cube_tex[3]);
	}
	else if (cubeVec > 0 && cubeVec < 5) {
		if (cubeVec == RIGHTWALL) {
			glRotatef(90, 1.0f, 0.0f, 0.0f);
		}
		else if (cubeVec == FRONTWALL) {
			glRotatef(90, 0.0f, 0.0f, 1.0f);
		}
		else if (cubeVec == BACKWALL) {
			glRotatef(-90, 0.0f, 0.0f, 1.0f);
		}
		// Left face
		texquad(cube[2], cube[6], cube[7], cube[3], cube_tex[4], cube_tex[5], cube_tex[6], cube_tex[7]);

		// Right face
		texquad(cube[0], cube[4], cube[5], cube[1], cube_tex[4], cube_tex[5], cube_tex[6], cube_tex[7]);

		// Back face
		texquad(cube[1], cube[5], cube[6], cube[2], cube_tex[4], cube_tex[5], cube_tex[6], cube_tex[7]);

		// Front face
		texquad(cube[0], cube[3], cube[7], cube[4], cube_tex[4], cube_tex[5], cube_tex[6], cube_tex[7]);
	}
	else if (cubeVec == POPCORN) {
		// Bottom face
		texquad(cube[4], cube[7], cube[6], cube[5], cube_tex[8], cube_tex[9], cube_tex[10], cube_tex[11]);
	}
	else if (cubeVec == WOODDOOR) {
		// Left face
		texquad(cube[2], cube[6], cube[7], cube[3], cube_tex[12], cube_tex[13], cube_tex[14], cube_tex[15]);

		// Right face
		texquad(cube[0], cube[4], cube[5], cube[1], cube_tex[12], cube_tex[13], cube_tex[14], cube_tex[15]);

		// Back face
		texquad(cube[1], cube[5], cube[6], cube[2], cube_tex[12], cube_tex[13], cube_tex[14], cube_tex[15]);

		// Front face
		texquad(cube[0], cube[3], cube[7], cube[4], cube_tex[12], cube_tex[13], cube_tex[14], cube_tex[15]);
	}
	else if (cubeVec == WOODTABLE) {
		// Top face
		texquad(cube[4], cube[7], cube[6], cube[5], cube_tex[16], cube_tex[17], cube_tex[18], cube_tex[19]);

		// Left face
		texquad(cube[2], cube[6], cube[7], cube[3], cube_tex[16], cube_tex[17], cube_tex[18], cube_tex[19]);

		// Right face
		texquad(cube[0], cube[4], cube[5], cube[1], cube_tex[16], cube_tex[17], cube_tex[18], cube_tex[19]);

		// Back face
		texquad(cube[1], cube[5], cube[6], cube[2], cube_tex[16], cube_tex[17], cube_tex[18], cube_tex[19]);

		// Front face
		texquad(cube[0], cube[3], cube[7], cube[4], cube_tex[16], cube_tex[17], cube_tex[18], cube_tex[19]);

	}
}

// Routine to draw quadrilateral face
void texquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat t1[], GLfloat t2[], GLfloat t3[], GLfloat t4[])
{
	// Draw face 
	glBegin(GL_POLYGON);
	glTexCoord2fv(t1);
	glVertex3fv(v1);
	glTexCoord2fv(t2);
	glVertex3fv(v2);
	glTexCoord2fv(t3);
	glVertex3fv(v3);
	glTexCoord2fv(t4);
	glVertex3fv(v4);
	glEnd();
}