#include "utils.h"
#include <cmath>
#include <cstdio>

/*
template <typename T>
void print_hex(std::ostream& stream, const T value, const int width)
{
	std::ios original_formatting = std::ios(nullptr);
	original_formatting.copyfmt(stream);
	if (width <= 0)
		stream << std::hex << value;
	else
		stream << std::setfill('0') << std::setw(width) << std::hex << value;
	stream.copyfmt(original_formatting);
}
*/

void print_argb8888(const std::uint32_t color)
{
	/*
	const unsigned int a = (color & 0xFF000000) >> 24;
	const unsigned int r = (color & 0x00FF0000) >> 16;
	const unsigned int g = (color & 0x0000FF00) >> 8;
	const unsigned int b = color & 0x000000FF;
	std::cout << " A R G B" << std::endl;
	print_hex(std::cout, a, 2);
	print_hex(std::cout, r, 2);
	print_hex(std::cout, g, 2);
	print_hex(std::cout, b, 2);
	std::cout << std::endl;
	*/
	std::printf(" A R G B\n%" PRIX32 "\n", color);
}

// i = independent variable
// d = dependent variable
std::vector<float> interpolate(const float i0, const float d0, const float i1, const float d1)
{
	const int num_vals = std::round(i1 - i0) + 1;
	std::vector<float> values(num_vals);
	const float slope = (d1 - d0) / (i1 - i0);
	float d = d0;
	for (float& f : values) {
		f = d;
		d += slope;
	}
	return values;
}
