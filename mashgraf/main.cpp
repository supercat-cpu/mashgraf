#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <gl\glew.h>
#include <gl\glut.h>
#include "BSpline.h"
#include"BmpLoad.h"
#include"Formats.h"
#include "Grid4Metaballs.h"
#include "Metaball.h"

const double PI = 3.14159265358979323846;

GLfloat l_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat l_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat l_specular[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat l_position[] = { -4.0, 4.0, -4.0, 0.0 };

// Экран
int sceneNumber = 0;
GLint Width = 512, Height = 512;
double x_angle = 30, y_angle = 0; // вращение сцены
double angle = 0; // Для вращающихся кусков куба
int mouse_x = 0, mouse_y = 0, mouse_button = -1, mouse_state = GLUT_UP;

// Сплайн
const int numBspl = 10;
double3 ControlPoints[] = {
	 double3(0,   -1, 1.0),
	 double3(0.0,-0.5, 1.0),
	 double3(0.8,  0, 1.0),
	 double3(0.2, 0.5, 1.0),
	 double3(0.2, 1.0, 1.0),
	 double3(0.2, 1.5, 1.0),
	 double3(0.2, 2.0, 1.0),
	 double3(0.2, 2.5, 1.0),
	 double3(0,   3.0, 1.0),
	 double3(0, 3.5, 1.0)

};
int N = 25; // количество вращений
int degree = 4; // степень сплайна
BSpline<double3> bsp(N, OpenBasis, numBspl, degree, 0, 1);

// Билборды
const int numbBilbs = 3;
double billbs[numbBilbs];
int offset = 0;

unsigned char* ConstructTexture(int* w, int* h)
{
	int width1, height1;
	unsigned char* tex1 = LoadTrueColorBMPFile("bird.bmp", &width1, &height1);
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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 45, 0);
	glEnable(GL_TEXTURE_2D);
	glColor4d(1, 1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < numbBilbs; i++) {
		int index = i;
		if (y_angle < 180) index = numbBilbs - 1 - i;
		double x = 40 * index / (numbBilbs - 1) + (Width / 2 + offset);
		double z = billbs[index];

		glPushMatrix();
		glLoadIdentity();
		glTranslated(0, 0, -3 * 40);
		glRotated(x_angle, 1, 0, 0);
		glRotated(y_angle, 0, 1, 0);
		glTranslated(x, Height / 4, z);
		glRotated(-y_angle, 0, 1, 0);
		glRotated(-x_angle, 1, 0, 0);
		glTranslatef(0, 50, 90);
		glBegin(GL_QUADS);
			glTexCoord2d(1.0, 0);
			glVertex3d(10, 0, 0);
			glTexCoord2d(1.0, 1.0);
			glVertex3d(10, 20, 0);
			glTexCoord2d(0.0, 1.0);
			glVertex3d(-10, 20, 0);
			glTexCoord2f(0.0, 0.0);
			glVertex3d(-10, 0, 0);
		glEnd();
		glPopMatrix();
	}
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

// Для сплайнов
inline void glVertex(const double3 v)
{
	int k = 40;
	glVertex3d(k * v.x, k * v.y, k * v.z);
}

inline double3 RotateY(double angle, const double2 v)
{
	return double3(cos(angle) * v.x, v.y, sin(angle) * v.x);
}

void DrawBSpline()
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

	for (int j = 1; j < bsp.GetNumPoints(); j++) {
		glBegin(GL_QUAD_STRIP);
			for (int i = 0; i <= bsp.GetTesselation(); i++) {
				double phi = (i < N) ? (2 * PI * i / N) : (0);
				glVertex(RotateY(phi, bsp.GetPoint(j).Perspective()));
				glVertex(RotateY(phi, bsp.GetPoint(j + 1).Perspective()));
			}
		glEnd();
	}
	glPopMatrix();
}

// csg
void Cub(int x, int y, int z, int size)
{
	glPushMatrix();
	glTranslated(x, y, z);
	glutSolidCube(size);
	glPopMatrix();
}

void resetDepth(void (*draw)(int, int, int, int), int x, int y, int z, int size)
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_ALWAYS);

	draw(x, y, z, size);

	glDepthFunc(GL_LESS);
}

// face - какие грани отбрасываем
// test GL_NOTEQUAL - то что попало внутрь другого, и наоборот GL_EQUAL то что не попало
void AinB(void (*A)(int, int, int, int), int x1, int y1, int z1, int size1,
	void (*B)(int, int, int, int), int x2, int y2, int z2, int size2,
	int face, int test)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glCullFace(face);

	A(x1, y1, z1, size1);

	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glStencilFunc(GL_ALWAYS, 0, 0xfffffffff);
	glCullFace(GL_BACK); 

	B(x2, y2, z2, size2);

	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);

	B(x2, y2, z2, size2);

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(test, 0, 0xfffffffff);
	glDisable(GL_DEPTH_TEST);
	glCullFace(face);

	A(x1, y1, z1, size1);

	glDisable(GL_STENCIL_TEST);
}

void Minus(int x1, int y1, int z1, int size1, int x2, int y2, int z2, int size2)
{
	AinB(Cub, x2, y2, z2, size2, Cub, x1, y1, z1, size1, GL_FRONT, GL_NOTEQUAL); // нелицевые 2го лежащие в 1ом
	resetDepth(Cub, x1, y1, z1, size1);
	AinB(Cub, x1, y1, z1, size1, Cub, x2, y2, z2, size2, GL_BACK, GL_EQUAL); // лиценвые грани 1го не лежащие во 2м
}

void Intersect(int x1, int y1, int z1, int size1, int x2, int y2, int z2, int size2)
{
	AinB(Cub, x2, y2, z2, size2, Cub, x1, y1, z1, size1, GL_BACK, GL_NOTEQUAL); // то что из одного внутри второго
	resetDepth(Cub, -40, 15, 0, 25);
	AinB(Cub, x1, y1, z1, size1, Cub, x2, y2, z2, size2, GL_BACK, GL_NOTEQUAL); // то что из одного внутри второго
}

// куб и ваза
void DrawScene()
{
	glPushMatrix();
	glTranslatef(-50.0, 50.0, 50.0);
	DrawBSpline();
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
	glBegin(GL_QUADS);
		glColor4d(125, 120, 255, 0.3);
		glVertex3d(240, -20, 120);
		glVertex3d(-240, -20, 120);
		glVertex3d(-240, -20, -120);
		glVertex3d(240, -20, -120);
	glEnd();
}

void Display()
{
	UpdateMetaBalls();
	glClearColor(0.0, 1.0, 0.713, 0.0);
	glClearStencil(GLint(0.0));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0, 0, -9);
	glRotated(x_angle, 1, 0, 0);
	glRotated(y_angle, 0, 1, 0);
	if (sceneNumber == 0) {
		glEnable(GL_LIGHTING);
		glLightfv(GL_LIGHT0, GL_POSITION, l_position);
		DrawScene();

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xfffffffff);
		glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		RenderSurface();
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 0xfffffffff);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_LIGHTING);
		glPushMatrix();
		glLoadIdentity();		
		glTranslatef(0.0f, Height/4, 0.0f);
		glScaled(10, 10, 10);
		// зеленый
		GLfloat c_emissive[4] = { 0.0f, 0.4f, 0.0f, 0.0f };
		GLfloat c_diffuse[4] = { 0.2f, 0.8f, 0.6f, 0.0f };
		GLfloat c_specular[4] = { 0.2f, 0.6f, 0.2f, 0.0f };
		GLfloat c_ambient[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c_emissive);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c_ambient);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
		cubeGrid.RenderBalls();
		glPopMatrix();

		// красно-рыжий
		GLfloat c_emissive2[4] = { 1.0f, 0.4f, 0.0f, 0.0f };
		GLfloat c_diffuse2[4] = { 0.8f, 0.8f, 0.6f, 0.0f };
		GLfloat c_specular2[4] = { 0.8f, 0.6f, 0.2f, 0.0f };
		GLfloat c_ambient2[4] = { 0.9f, 0.1f, 0.1f, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c_emissive2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c_diffuse2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c_specular2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c_ambient2);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);

		glPushMatrix();
		glTranslated(-30, 0, 0);
		glRotated(angle, 0, 1, 0);
		glScaled(2.5, 2.5, 2.5);
		Minus(1, 0, 0, 20, 10, 10, 0, 30);
		glPopMatrix();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPushMatrix();
		glTranslated(-30, 0, 0);
		glRotated(angle, 1, 0, 0);
		glScaled(2.5, 2.5, 2.5);
		Intersect(-40, 15, 0, 25, -30, 10, 0, 20);
		glPopMatrix();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPushMatrix();
		glTranslated(80, 0, 0);
		glRotated(angle, 0, 0, 1);
		glScaled(2.5, 2.5, 2.5);
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
	if (mouse_state == GLUT_DOWN && mouse_button == GLUT_LEFT_BUTTON) {
			y_angle += (x - mouse_x) / 3;
			if (y_angle < 0)
			{
				y_angle += 360;
			}
			if (y_angle >= 360)
			{
				y_angle -= 360;
			}
			x_angle += (y - mouse_y) / 3;
			if (x_angle < 5)
			{
				x_angle = 10;
			}
			if (x_angle > 90)
			{
				x_angle = 90;
			}
			Display();
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
	startTime = (double)timeGetTime();
	glLightfv(GL_LIGHT0, GL_AMBIENT, l_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, l_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	for (int i = 0; i < numBspl; i++)
		bsp.ControlPoint(i) = ControlPoints[i];

	glEnable(GL_DEPTH_TEST);
	GLuint texture;
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

		for (int i = 0; i < numbBilbs; i++)
			billbs[i] = 40 * rand() / RAND_MAX;
	}

	for (int i = 0; i < numMetaballs; i++)
		metaballs[i] = Metaball(double3(0.0f, 0.0f, 0.0f), 5.0f + float(i));

}

void Key(unsigned char key, int x, int y)
{
	if (key == '\033') { // escape
		glutDestroyWindow(glutGetWindow());
		exit(1);
	}
	if (key == ' ' && sceneNumber == 0) { //Другая сцена
		sceneNumber++;
		Display();
	}
}

void timer(int i = 0)
{
	angle += 10; // в градусах
	offset = (offset - 1) % (Width);
	Display();
	glutTimerFunc(40, timer, 0);
}

void Reshape(GLint w, GLint h)
{
	Width = w;
	Height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-Width / 2, Width / 2, -Height / 2, Height / 2, -150, 150);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Funny objects");
	init();
	glutDisplayFunc(Display);
	glutMouseFunc(Click_mouse);
	glutMotionFunc(Motion_mouse);
	glutKeyboardFunc(Key);
	glutReshapeFunc(Reshape);
	glutTimerFunc(40, timer, 0);
	glutMainLoop();
}