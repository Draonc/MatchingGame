// RotatingCube.cpp : Defines the entry point for the console application. Author: Candice Sandefur
//

#include "stdafx.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <GL\glut.h>
#include <iostream>
#include <ctime>
#include <soil.h>
#include <SFML\Window.hpp>

using namespace std;

void myReshape(int w, int h);
void display(void);
void drawCube(int image);
void face(int v1, int v2, int v3, int v4, int c);
void spinCube();
void mouse(int btn, int state, int x, int y);
void setImage();
void checkMatch(int row, int col);
void reset();
int LoadGLTextures();
void displayStats();
void print(int x, int y, int z, char *string);

//Variables
bool spinFront = false;
bool spinBack = false;
bool spinning = false;
bool started = false;
int selected = 0;
int numMatched;
int score, tries;
int n = 1;
int duration = 0;
GLsizei winWidth = 0, winHeight = 0;
time_t start, endtime;
time_t startTime, endingTime;
GLuint textures[10];

GLint locX, locY;
int curRow = -1, curCol = -1, lastRow = -1, lastCol = -1;
GLint image[4][4];
GLboolean flipped[4][4];
GLboolean matched[4][4];
GLfloat angle[4][4];
GLfloat vertices[8][3] = { { -1.2, -1.2, -.05 },{ 1.2, -1.2, -.05 },{ 1.2, 1.2, -.05 },
							{ -1.2, 1.2, -.05 },{ -1.2, -1.2, .05 },{ 1.2, -1.2, .05 },
							{ 1.2, 1.2, .05 },{ -1.2, 1.2, .05 } };

//Main
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Matching Game");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glEnable(GL_TEXTURE_2D);
	glutIdleFunc(spinCube);
	glutMouseFunc(mouse);
	setImage();
	LoadGLTextures();
	glEnable(GL_DEPTH_TEST);


	time(&startTime);
	glutMainLoop();
	return 0;
}


void myReshape(int w, int h) {
	winWidth = glutGet(GLUT_WINDOW_WIDTH);
	winHeight = glutGet(GLUT_WINDOW_HEIGHT);
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 4, 4, 0.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

//Sets up display
void display(void) {
	GLfloat startX = 1.25;
	GLfloat startY = 1.25;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.5, 0.0, 1);
	glMatrixMode(GL_PROJECTION);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(.4, .4, .4);

	//if two tiles selected start timer
	if (selected >= 2 && !started) {
		time(&start);
		started = true;
	}

	//If timer reaches 3 seconds reset the Tiles
	if ((time(&endtime) - start) == 3) {
		spinFront = true;
		start = 0;
	}

	//If matched all matched display the stats
	if (numMatched == 8) {
		displayStats();
	}

	if (numMatched != 8) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				glPushMatrix();
				glTranslatef(startX, startY, 0.0);
				glRotatef(angle[j][i], 0.0, 1.0, 0.0);
				drawCube(image[j][i]);
				glPopMatrix();
				startX += 2.5;
			}
			startX = 1.25;
			startY += 2.5;
		}
	}
	glutSwapBuffers();
}

//Finds end time and displays stats to screen
void displayStats() {
	if (n) {
		duration = time(&endingTime) - startTime;
		n = 0;
	}
	char *printString;
	char temp[12];

	printString = "Seconds taken to complete: \n";
	print(0, 2, -5, printString);
	sprintf_s(temp, "%d", duration);
	print(6, 2, -5, temp);

	printString = "Tries taken to complete\n";
	print(0, 4, -5, printString);
	sprintf_s(temp, "%d", tries);
	print(6, 4, -5, temp);

	printString = "Your score is: \n";
	print(0, 6, -5, printString);
	sprintf_s(temp, "%d", score);
	print(6, 6, -5, temp);

	printString = "RIGHT CLICK TO EXIT\n";
	print(0, 8, -5, printString);
}

//Prints string to screen
void print(int x, int y, int z, char *string)
{
	char *c;
	glPushMatrix();
	glTranslatef(x + .5, y + 1, z);
	glScalef(0.003f, -.003f, z);
	int len = (int)strlen(string);
	for (int i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, string[i]);
	}
	glPopMatrix();
};


//Draws the cube
void drawCube(int image) {
	face(1, 0, 3, 2, image);//Back
	face(2, 3, 7, 6, 9);//Top
	face(0, 4, 7, 3, 9);//Left
	face(1, 2, 6, 5, 9);//Right
	face(4, 5, 6, 7, 8);//Front
	face(0, 1, 5, 4, 9);//Bottom
}
//Places the image on a side
void face(int v1, int v2, int v3, int v4, int c) {
	glBindTexture(GL_TEXTURE_2D, textures[c]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vertices[v1]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vertices[v2]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vertices[v3]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vertices[v4]);
	glEnd();
}
//Check to see if there is a match
void checkMatch(int row, int col) {
	if (matched[row][col] || (lastRow == row && lastCol == col)) {
		;
	}
	else if (lastRow == -1 && !matched[row][col]) {
		lastRow = row;
		lastCol = col;
	}
	else if (image[lastRow][lastCol] == image[row][col]) {
		matched[lastRow][lastCol] = true;
		matched[row][col] = true;
		lastRow = -1;
		lastCol = -1;
		selected = 0;
		start = 0;
		score += 10;
		numMatched++;
	}
	else {
		curRow = row;
		curCol = col;
	}
}

GLfloat delta = .05;
//Resets the tiles to face down
void reset() {
	spinning = true;
	flipped[curRow][curCol] = true;
	flipped[lastRow][lastCol] = true;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (flipped[j][i] && !matched[j][i]) {
				if (angle[j][i] >= 0) {
					angle[j][i] -= delta;
				}
				else {
					flipped[j][i] = false;
					spinFront = false;
					spinning = false;
					started = false;
					curRow = -1;
					curCol = -1;
					lastRow = -1;
					lastCol = -1;
					selected = 0;
				}
			}
		}
	}
}


//Rotates the cubes
void spinCube() {

	if (spinFront) {
		reset();
	}
	if (spinBack) {
		spinning = true;
		if (angle[locX][locY] <= 180) {
			angle[locX][locY] += delta;
		}
		else {
			spinBack = false;
			spinning = false;
		}
	}
	glutPostRedisplay();
}


//Mouse Input
void mouse(int btn, int state, int x, int y) {
	GLfloat lengthW = (winWidth / 4) - 5;
	GLfloat lengthH = (winHeight / 4) - 5;
	if (!spinning && (btn == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		locX = x / lengthW;
		locY = y / lengthH;

		if (angle[locX][locY] <= 180) {
			selected++;
			if (selected < 3) {
				spinBack = true;
				tries++;
				score--;
				checkMatch(locX, locY);
			}
		}
	}

	if (!spinning && (btn == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		exit(0);
	}
}

//Randomly assign an int to each Tile to be used later for the image
void setImage() {
	GLint set[8] = { 0 };
	GLint temp;
	GLboolean setting;
	srand(time(NULL));
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			setting = true;
			while (setting) {
				temp = rand() % 8;
				if (set[temp] < 2) {
					set[temp] = set[temp] + 1;
					image[j][i] = temp;
					setting = false;
				}
			}
		}
	}
}

//Load in the images.
//IF THE IMAGES ARE NOT LOADING MAKE SURE THE DEBUG WORKING DIRECTORY IS POINTING TO THE CORRECT PLACE
int LoadGLTextures()
{
	//load an image file directly as a new OpenGL texture
	textures[0] = SOIL_load_OGL_texture("Plane1.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[1] = SOIL_load_OGL_texture("Plane2.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[2] = SOIL_load_OGL_texture("Plane3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[3] = SOIL_load_OGL_texture("Plane4.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[4] = SOIL_load_OGL_texture("Plane5.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[5] = SOIL_load_OGL_texture("Plane6.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[6] = SOIL_load_OGL_texture("Plane7.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[7] = SOIL_load_OGL_texture("Plane8.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[8] = SOIL_load_OGL_texture("Prop.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[9] = SOIL_load_OGL_texture("Side.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	//Tests if the image loaded used for error checking.
	//if (0 == textures[0])
	//{
	//	printf("SOIL loading error: '%s'\n", SOIL_last_result());
	//}
	return true;                                     
}