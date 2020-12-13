#include "Vector.h"
#include "Constants.h"
#include <gl\glew.h>
#include <iostream>

class CubeVertex
{
public:
	double3 position;
	float value;
};

class CubsInGrid
{
public:
	CubeVertex* vertices[8];
};

const int gridSize = 40;
const int numVertices = (gridSize + 1) * (gridSize + 1) * (gridSize + 1);
const int gridSize_1 = (gridSize + 1);
const int numCubes = (gridSize) * (gridSize) * (gridSize);

class Grid4Metaballs
{
public:
	CubeVertex vertices[gridSize_1][gridSize_1][gridSize_1];
	CubsInGrid cubes[numCubes];

	void RenderBalls();

	Grid4Metaballs()
	{
		// сетка
		for (int i = 0; i < gridSize + 1; i++)
		{
			for (int j = 0; j < gridSize + 1; j++)
			{
				for (int k = 0; k < gridSize + 1; k++)
				{
					double3 pos((i*20.0f)/(gridSize)-10.0f, (j*20.0f) / (gridSize)-10.0f, (k*20.0f) / (gridSize)-10.0f);
					vertices[i][j][k].position = pos;
				}
			}
		}

		//кубы
		int currentCube = 0;
		for (int i = 0; i < gridSize; i++)
		{
			for (int j = 0; j < gridSize; j++)
			{
				for (int k = 0; k < gridSize; k++)
				{
					cubes[currentCube].vertices[0] = &vertices[i][j][k];
					cubes[currentCube].vertices[1] = &vertices[i][j][k + 1];
					cubes[currentCube].vertices[2] = &vertices[i][j + 1][k + 1];
					cubes[currentCube].vertices[3] = &vertices[i][j + 1][k];
					cubes[currentCube].vertices[4] = &vertices[i + 1][j][k];
					cubes[currentCube].vertices[5] = &vertices[i + 1][j][k + 1];
					cubes[currentCube].vertices[6] = &vertices[i + 1][j + 1][k + 1];
					cubes[currentCube].vertices[7] = &vertices[i + 1][j + 1][k];
					currentCube++;
				}
			}
		}
	}
};

void Grid4Metaballs::RenderBalls()
{
	float threshold = 1.0f;
	static double3 edgeVertices[12];

	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < numCubes; i++)
		{
			//какие вершины за поверхностью
			unsigned char cubeIndex = 0;

			if (cubes[i].vertices[0]->value < threshold)
				cubeIndex |= 1;
			if (cubes[i].vertices[1]->value < threshold)
				cubeIndex |= 2;
			if (cubes[i].vertices[2]->value < threshold)
				cubeIndex |= 4;
			if (cubes[i].vertices[3]->value < threshold)
				cubeIndex |= 8;
			if (cubes[i].vertices[4]->value < threshold)
				cubeIndex |= 16;
			if (cubes[i].vertices[5]->value < threshold)
				cubeIndex |= 32;
			if (cubes[i].vertices[6]->value < threshold)
				cubeIndex |= 64;
			if (cubes[i].vertices[7]->value < threshold)
				cubeIndex |= 128;
			//std::cout << cubeIndex << std::endl;

			//по каким краям интерполировать
			int usedEdges = edges[cubeIndex];

			// кубы которые внутри или совсем снаружи отбрасываем			
			if (usedEdges == 0 || usedEdges == 255)
				continue;

			//обновляем грани
			for (int currentEdge = 0; currentEdge < 12; currentEdge++)
			{
				if (usedEdges & 1 << currentEdge)
				{
					CubeVertex* v1 = cubes[i].vertices[endsOfEdges[currentEdge * 2]];
					CubeVertex* v2 = cubes[i].vertices[endsOfEdges[currentEdge * 2 + 1]];

					float delta = (threshold - v1->value) / (v2->value - v1->value);

					edgeVertices[currentEdge].x = v1->position.x + delta * (v2->position.x - v1->position.x);
					edgeVertices[currentEdge].y = v1->position.y + delta * (v2->position.y - v1->position.y);
					edgeVertices[currentEdge].z = v1->position.z + delta * (v2->position.z - v1->position.z);
				}
			}

			for (int k = 0; triangles[cubeIndex][k] != -1; k += 3)
			{
				glVertex3f((GLfloat)edgeVertices[triangles[cubeIndex][k + 0]].x, 
					(GLfloat)edgeVertices[triangles[cubeIndex][k + 0]].y,
					(GLfloat)edgeVertices[triangles[cubeIndex][k + 0]].z);

				glVertex3f((GLfloat)edgeVertices[triangles[cubeIndex][k + 2]].x, 
					(GLfloat)edgeVertices[triangles[cubeIndex][k + 2]].y,
					(GLfloat)edgeVertices[triangles[cubeIndex][k + 2]].z);

				glVertex3f((GLfloat)edgeVertices[triangles[cubeIndex][k + 1]].x,
					(GLfloat)edgeVertices[triangles[cubeIndex][k + 1]].y,
					(GLfloat)edgeVertices[triangles[cubeIndex][k + 1]].z);

				//std::cout << edgeVertices[triangles[cubeIndex][k + 1]].x<<std::endl;
				//std::cout << edgeVertices[triangles[cubeIndex][k + 1]].y << std::endl;
				//std::cout << edgeVertices[triangles[cubeIndex][k + 1]].z << std::endl;
			}
		}
	}
	glEnd();
}
