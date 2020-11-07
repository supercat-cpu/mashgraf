#include<GL/glew.h>
#include<GL/freeglut.h>

#include<iostream>
#include<fstream>
#include<string>

using namespace std;

#pragma pack(1) // говорим что проьтно упакуем
struct Vertex // определяем что такое вершина
{
	float x, y, z;
};
#pragma pack()
// координаты вершин
Vertex cube_vertices[] = {
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, 0.5f, -0.5f },
	{ 0.5f, 0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{ -0.5f, 0.5f, 0.5f },
	{ 0.5f, 0.5f, 0.5f },
	{ 0.5f, -0.5f, 0.5f }
};

// грани
GLuint cube_indices[] = {
	0,1,2,3,
	4,5,6,7,
	0,1,5,4,
	1,2,6,5,
	2,3,7,6,
	3,0,4,7
};

GLuint vertexBuffer;
GLuint vertexArray;

void reshape(int w, int h)
{
	glViewport(0, 0, w, h); // Область рисования -- все окно
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	//рисуем прямоугльники с вершинами бла-бла
	glDrawElements(GL_QUADS, sizeof(cube_indices) / sizeof(cube_indices[0]), GL_UNSIGNED_INT, cube_indices);
	glFlush(); // Гарантируем выполнение всех операций: попробуйте закомментировать :)
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutCreateWindow("OpenGL cube");

	glewInit();

	glGenBuffers(1, &vertexBuffer); // формируй буфер и запиши его id в верт буфер
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // делаем его текущим
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW); // запихиваем туда массив вершин

	glGenVertexArrays(1, &vertexArray); // массив вершин  уже не буфер
	glBindVertexArray(vertexArray); // делаем его активным
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // говорим что тут начиная с 0  хранится 3 числа бробных итп
	glEnableVertexAttribArray(0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // рисуем только ребра

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}
