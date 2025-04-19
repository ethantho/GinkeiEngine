#pragma once

#include "glm/glm.hpp"
#include "SDL_ttf.h"
#include <queue>
#include <string>

struct TextDrawRequest {
	std::string str_content;
	std::string font_name;
	float x;
	float y;
	int font_size;
	int r;
	int g;
	int b;
	int a;
};

struct ImageDrawRequest {
	std::string image_name;
	SDL_FRect* srcrect;
	float x;
	float y;
	int rotation_degrees;
	float scale_x;
	float scale_y;
	float pivot_x;
	float pivot_y;
	int r;
	int g;
	int b;
	int a;
	int sort_order;
};

struct PixelDrawRequest {
	float x;
	float y;
	int r;
	int g;
	int b;
	int a;
};

class Renderer
{
public:
	void CreateWindow(const char* title, int w, int h);
	void CreateRenderer(Uint8 r, Uint8 g, Uint8 b);
	void PresentRender();
	void DrawImage(SDL_Texture* img);
	void Clear();
	void DrawText(const std::string& text_content, std::string font_name, int font_size, SDL_Color font_color, int x, int y);
	void DrawAllText();
	void DrawUI();
	void DrawActorImages();
	void DrawAllPixels();
	void DoUIDrawRequest(ImageDrawRequest& request);
	void DoActorDrawRequest(ImageDrawRequest& request);
	SDL_Renderer* sdl_renderer;

	glm::vec2 cam_position = glm::vec2(0.0f, 0.0f);
	glm::vec2 cam_offset = glm::vec2(0.0f, 0.0f);
	float zoom_factor = 1.0f;
	float cam_ease_factor = 1.0f;

	int clear_r = 255;
	int clear_g = 255;
	int clear_b = 255;
	int clear_a = 255;

	std::queue<TextDrawRequest> textDrawCalls;
	std::vector<ImageDrawRequest> uiDrawCalls;
	std::vector<ImageDrawRequest> actorDrawCalls;
	std::vector<PixelDrawRequest> pixelDrawCalls;

private:
	int window_width;
	int window_height;
	SDL_Window* sdl_window;
	int frame_counter = 0;
};

