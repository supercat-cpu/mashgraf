#pragma once
#include "Vector.h"
// для таймера
#include <mmsystem.h>

// таймер
double startTime;
inline double GetTime()
{
	return ((double)timeGetTime()) - startTime;
}

class Metaball
{
public:
	double3 position;
	float squaredRadius;

	Metaball(double3 _position = double3(0, 0, 0), float _radius = 0)
	{
		position = _position;
		squaredRadius = _radius;
	}
};

// Меташары
Grid4Metaballs cubeGrid;
const int numMetaballs = 3;
Metaball metaballs[numMetaballs];

static void UpdateMetaBalls()
{
	float c = 2.0f * (float)cos(GetTime() / 600);

	metaballs[0].position.x = -4.0f * (float)cos(GetTime() / 700) - c;
	metaballs[0].position.y = 4.0f * (float)sin(GetTime() / 600) - c;
	metaballs[1].position.x = 5.0f * (float)sin(GetTime() / 400) + c;
	metaballs[1].position.y = 5.0f * (float)cos(GetTime() / 400) - c;
	metaballs[2].position.x = -5.0f * (float)cos(GetTime() / 400) - 0.2f * (float)sin(GetTime() / 600);
	metaballs[2].position.y = 5.0f * (float)sin(GetTime() / 500) - 0.2f * (float)sin(GetTime() / 400);

	//очистка
	for (int i = 0; i < gridSize + 1; i++)
		for (int j = 0; j < gridSize + 1; j++)
			for (int k = 0; k < gridSize + 1; k++)
				cubeGrid.vertices[i][j][k].value = 0.0f;

	double3 ballToPoint;
	float squaredRadius;
	double3 ballPosition;
	for (int i_ball = 0; i_ball < numMetaballs; i_ball++)
	{
		squaredRadius = metaballs[i_ball].squaredRadius;
		ballPosition = metaballs[i_ball].position;
		for (int i = 0; i < gridSize + 1; i++)
			for (int j = 0; j < gridSize + 1; j++)
				for (int k = 0; k < gridSize + 1; k++)
				{
					ballToPoint.x = cubeGrid.vertices[i][j][k].position.x - ballPosition.x;
					ballToPoint.y = cubeGrid.vertices[i][j][k].position.y - ballPosition.y;
					ballToPoint.z = cubeGrid.vertices[i][j][k].position.z - ballPosition.z;

					//от точки до шарика
					float squaredDistance = ballToPoint.x * ballToPoint.x +
						ballToPoint.y * ballToPoint.y +
						ballToPoint.z * ballToPoint.z;

					cubeGrid.vertices[i][j][k].value += squaredRadius / squaredDistance; // r^2/d^2
					if (cubeGrid.vertices[i][j][k].value != 0) 
					{
						int f = 0;
					}
				}
	}
}
