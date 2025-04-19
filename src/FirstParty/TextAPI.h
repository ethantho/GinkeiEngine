#pragma once

#include "Renderer.h"

class TextAPI
{
public:
	static inline Renderer* renderer;

	static void Draw(std::string str_content, float x, float y, std::string font_name, int font_size, int r, int g, int b, int a);
};

