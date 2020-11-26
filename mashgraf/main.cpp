#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <gl\glew.h>
#include <gl\glut.h>
#include "BSpline.h"
#include"BmpLoad.h"
#include"Formats.h"

GLfloat l_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat l_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat l_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat l_position[] = { -4.0, 4.0, -4.0, 0.0 };

GLint Width = 512, Height = 512;
int q = 0;

double x_angle = 30;
double y_angle = 0, angle = 0;
int mouse_x = 0, mouse_y = 0, mouse_button = -1, mouse_state = GLUT_UP;

const double PI = 3.14159265358979323846;

double3 ControlPoints[] = {
 double3(0,-0.9, 1.0),
 double3(0.0,-0.9, 1.0),
 double3(0.8,-0.8, 1.0),
 double3(0.2,0.3, 1.0),
 double3(0.0,0.5 * 5 , 1.0 * 5),
 double3(0,0.8, 1.0),
 double3(0,0.9, 1.0)

};

int N = 25;

BSpline<double3> bsp(N, OpenBasis, sizeof(ControlPoints) / sizeof(ControlPoints[0]), 3, 0, 1);

const int area = 40;
const double bsize = 20.0;

double billbs[3];

GLuint texture;

inline void glVertex(const double2 v)
{
	glVertex2d(30 * v.x, 30 * v.y);
}

inline void glVertex(const double3 v)
{
	glVertex3d(30 * v.x, 30 * v.y, 30 * v.z);
}

inline double3 RotateY(double angle, const double2 v)
{
	return double3(cos(angle) * v.x, v.y, sin(angle) * v.x);
}

unsigned char* ConstructTexture(int* w, int* h)
{
	int width1, height1;
	unsigned char* tex1 = LoadTrueColorBMPFile("bird.bmp", &width1, &height1);
	if (!tex1)
		return NULL;
	int width2, height2;
	unsigned char* tex2 = LoadTrueColorBMPFile("abird.bmp", &width2, &height2);
	unsigned char* result = new unsigned char[width1 * height1 * 4];
	for (int i = 0; i < width1 * height1; i++) {
		result[4 * i] = tex1[3 * i];
		result[4 * i + 1] = tex1[3 * i + 1];
		result[4 * i + 2] = tex1[3 * i + 2];
		result[4 * i + 3] = (unsigned char)(((int)tex2[3 * i] + (int)tex2[3 * i + 1] + (int)tex2[3 * i + 2]) / 3);
	}
	delete[] tex1;
	delete[] tex2;
	*w = width1;
	*h = height1;
	return result;
}

void Billboards() {
	glPushMatrix();
	glScaled(4, 4, 4);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 45, 0);
	glEnable(GL_TEXTURE_2D);
	glColor4d(1, 1, 1, 1);
	int Count = sizeof(billbs) / sizeof(billbs[0]);
	for (int i = 0; i < Count; i++) {
		int index = i;
		if (y_angle < 180) index = Count - 1 - i;
		double x = -area + bsize + 2 * (area - bsize) * index / (Count - 1);
		//	billbs[i] = -area + bsize + 2*rand()*(area - bsize)/RAND_MAX;
		double z = billbs[index];
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		glLoadIdentity();
		glTranslated(0, 0, -3 * area);
		glRotated(x_angle, 1, 0, 0);
		glRotated(y_angle, 0, 1, 0);
		glTranslated(x, 0, z);
		glRotated(-y_angle, 0, 1, 0);
		glRotated(-x_angle, 1, 0, 0);
		glTranslatef(0, 50, 90);
		glBegin(GL_QUADS);
		glTexCoord2d(1.0, 0);
		glVertex3d(bsize / 2, 0, 0);
		glTexCoord2d(1.0, 1.0);
		glVertex3d(bsize / 2, bsize, 0);
		glTexCoord2d(0.0, 1.0);
		glVertex3d(-bsize / 2, bsize, 0);
		glTexCoord2f(0.0, 0.0);
		glVertex3d(-bsize / 2, 0, 0);
		glEnd();
		glPopMatrix();
	}
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Nurbs()
{
	glPushMatrix();
	glTranslated(-40, 0, 0);
	GLfloat t_emissive[4] = { 0.0f, 0.8f, 0.7f, 0.0f };
	GLfloat t_diffuse[4] = { 0.1f, 0.9f, 0.8f, 0.0f };
	GLfloat t_specular[4] = { 0.1f, 0.6f, 0.6f, 0.0f };
	GLfloat t_ambient[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, t_emissive);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, t_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, t_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, t_ambient);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);

	for (int j = 1; j < bsp.GetTesselation() - 2; j++) {
		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i <= N; i++) {
			double phi = (i < N) ? (2 * PI * i / N) : (0);
			glVertex(RotateY(phi, bsp.GetPoint(j).Perspective()));
			glVertex(RotateY(phi, bsp.GetPoint(j + 1).Perspective()));
		}
		glEnd();
	}
	glPopMatrix();
}

void Cub(int x, int y, int z, int size)
{
	glPushMatrix();
	glTranslated(x, y, z);
	glutSolidCube(size);
	glPopMatrix();
}

void depth(void (*draw)(int, int, int, int), int x, int y, int z, int size)
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_ALWAYS);

	draw(x, y, z, size);

	glDepthFunc(GL_LESS);
}
void inter(void (*first)(int, int, int, int), int x1, int y1, int z1, int size1,
	void (*second)(int, int, int, int), int x2, int y2, int z2, int size2,
	int face, int test)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glCullFace(face);

	first(x1, y1, z1, size1);

	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glCullFace(GL_BACK); //не было в inter0

	second(x2, y2, z2, size2);

	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);

	second(x2, y2, z2, size2);

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(test, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glCullFace(face);

	first(x1, y1, z1, size1);

	glDisable(GL_STENCIL_TEST);
}

void Minus(int x1, int y1, int z1, int size1, int x2, int y2, int z2, int size2)
{
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
	inter(Cub, x2, y2, z2, size2, Cub, x1, y1, z1, size1, GL_FRONT, GL_NOTEQUAL);
	depth(Cub, x1, y1, z1, size1);
	inter(Cub, x1, y1, z1, size1, Cub, x2, y2, z2, size2, GL_BACK, GL_EQUAL);
}

void Intersect(int x1, int y1, int z1, int size1, int x2, int y2, int z2, int size2)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	inter(Cub, x2, y2, z2, size2, Cub, x1, y1, z1, size1, GL_BACK, GL_NOTEQUAL); // тут был интер0
	depth(Cub, -40, 15, 0, 25);
	inter(Cub, x1, y1, z1, size1, Cub, x2, y2, z2, size2, GL_BACK, GL_NOTEQUAL); // тут был интер0
}

void DrawScene()
{
	glPushMatrix();
	glTranslatef(-50.0, 50.0, 50.0);
	Nurbs();
	glTranslatef(100.0, 0.0, -20.0);
	GLfloat c_emissive[4] = { 0.0f, 0.4f, 0.0f, 0.0f };
	GLfloat c_diffuse[4] = { 0.2f, 0.8f, 0.6f, 0.0f };
	GLfloat c_specular[4] = { 0.2f, 0.6f, 0.2f, 0.0f };
	GLfloat c_ambient[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c_emissive);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c_ambient);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
	glutSolidCube(29);
	glPopMatrix();
}

void RenderSurface()
{
	//glTranslatef(0, -50, 0);
	glBegin(GL_QUADS);
	glColor4d(125, 249, 255, 0.3);
	glVertex3d(40 * 3, -20, 40 * 3);
	glVertex3d(-40 * 3, -20, 40 * 3);
	glVertex3d(-40 * 3, -20, -40 * 3);
	glVertex3d(40 * 3, -20, -40 * 3);
	glEnd();
}

void Display()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glClearStencil(GLint(0.0));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0, 0, -3 * 3);
	glRotated(x_angle, 1, 0, 0);
	glRotated(y_angle, 0, 1, 0);
	if (q == 0) {
		glEnable(GL_LIGHTING);
		glLightfv(GL_LIGHT0, GL_POSITION, l_position);
		glDisable(GL_LIGHTING);
		glEnable(GL_LIGHTING);
		DrawScene();

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		RenderSurface();
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 1);
		glPushMatrix();
		glScaled(1, -1, 1);

		glLightfv(GL_LIGHT0, GL_POSITION, l_position);
		DrawScene();
		glPopMatrix();

		glDisable(GL_STENCIL_TEST);

		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		RenderSurface();
		glDisable(GL_BLEND);

		Billboards();
	}
	else {
		//	glClearColor(0.7,0.0,0.0,0.5);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glPushMatrix();
		glTranslated(-30, 20, 0);
		glRotated(angle, 0, 1, 1);
		Nurbs();
		glPopMatrix();

		glPushMatrix();
		glTranslated(-30, 0, 0);
		glRotated(angle, 0, 1, 0);
		Minus(1, 0, 0, 20, 10, 10, 0, 30);
		glPopMatrix();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPushMatrix();
		glTranslated(30, 0, 0);
		glRotated(angle, 1, 0, 0);
		Intersect(-40, 15, 0, 25, -30, 10, 0, 20);
		glPopMatrix();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPushMatrix();
		glTranslated(80, 0, 0);
		glRotated(angle, 0, 0, 1);
		Minus(-40, 10, 0, 20, -30, 10, 0, 30);
		glPopMatrix();
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPushMatrix();
		glLoadIdentity();

		glPopMatrix();
	}
	glFlush();
	glutSwapBuffers();
}

void Motion_mouse(int x, int y)
{
	int dx = x - mouse_x;
	int dy = y - mouse_y;
	if (mouse_state == GLUT_DOWN) {
		if (mouse_button == GLUT_LEFT_BUTTON) {
			y_angle += dx / 3;
			if (y_angle < 0) y_angle += 360;
			if (y_angle >= 360) y_angle -= 360;
			x_angle += dy / 3;
			if (x_angle < 5) x_angle = 10;
			if (x_angle > 90) x_angle = 90;
			Display();
		}
	}
	mouse_x = x;
	mouse_y = y;
}

void Click_mouse(int button, int state, int x, int y)
{
	mouse_button = button;
	mouse_state = state;
	mouse_x = x;
	mouse_y = y;
}

void init() {

	glLightfv(GL_LIGHT0, GL_AMBIENT, l_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, l_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	if (q == 0) {
		for (int i = 0; i < sizeof(ControlPoints) / sizeof(ControlPoints[0]); i++)
			bsp.ControlPoint(i) = ControlPoints[i];
		glEnable(GL_DEPTH_TEST);

		unsigned char* tex_bits = NULL;
		int tex_width, tex_height;
		if ((tex_bits = ConstructTexture(&tex_width, &tex_height)) != NULL) {
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, 4, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bits);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			for (int i = 0; i < sizeof(billbs) / sizeof(billbs[0]); i++)
				billbs[i] = -area + bsize + 2 * rand() * (area - bsize) / RAND_MAX;
		}
	}
}

void Key(unsigned char key, int x, int y)
{
	if (key == '\033') { // escape
		glutDestroyWindow(glutGetWindow());
		exit(1);
	}
	if (key == ' ' && q == 0) {
		q++;
		init();
		Display();
	}
}

void timer(int i = 0)
{
	angle += 10;
	Display();
	glutTimerFunc(40, timer, 0);
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("My Prog");
	init();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-150, 150, -150, 150, -150, 150);
	glutDisplayFunc(Display);
	glutMouseFunc(Click_mouse);
	glutMotionFunc(Motion_mouse);
	glutKeyboardFunc(Key);
	glutTimerFunc(40, timer, 0);
	glutMainLoop();
}