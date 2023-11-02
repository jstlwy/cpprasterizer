#include "point.h"
#include <cmath>
#include "constants.h"

Point3D project_vertex(const Point3D& p)
{
	const float p_x = p.x * (d / p.z);
	const float c_x = p_x * (SCREEN_WIDTH / viewportSize);
	const float p_y = p.y * (d / p.z);
	const float c_y = p_y * (SCREEN_HEIGHT / viewportSize) * ASPECT_RATIO;
	return {c_x, c_y, p.z, p.h};
}

SDL_Point project_to_2d(const Point3D& p)
{
	const float p_x = p.x * (d / p.z);
	const int c_x = std::round(p_x * (SCREEN_WIDTH / viewportSize));
	const float p_y = p.y * (d / p.z);
	const int c_y = std::round(p_y * (SCREEN_HEIGHT / viewportSize) * ASPECT_RATIO);
	return {X_MID_SCREEN + c_x, Y_MID_SCREEN - c_y};
}

SDL_Point project_special(const Point3D& p)
{
	const int x = X_MID_SCREEN + std::round(p.x);
	const int y = Y_MID_SCREEN - std::round(p.y);
	return {x, y};
}