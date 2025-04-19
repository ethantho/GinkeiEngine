#pragma once

#include "Renderer.h"
#include <string>

class ImageAPI
{
public:
	static inline Renderer* renderer;
	static void DrawUI(std::string image_name, float x, float y);
	static void DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, int sorting_order);
	static void DrawTile(int tile_id, int tile_size, std::string source_image, float x, float y, int sorting_order);
	static void Draw(std::string image_name, float x, float y);
	static void DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);
	static void DrawPixel(float x, float y, float r, float g, float b, float a);
};

