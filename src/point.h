#ifndef POINT_H
#define POINT_H

#include <SDL2/SDL.h>

struct Point3D {
	float x;
	float y;
	float z;
	float h;
};

struct ScreenPoint3D {
	int x;
	int y;
	float z;
	float h;
};

Point3D project_vertex(const Point3D& p);
SDL_Point project_to_2d(const Point3D& p);
SDL_Point project_special(const Point3D& p);

#endif
