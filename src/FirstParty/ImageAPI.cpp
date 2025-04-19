#include "ImageAPI.h"
#include "ImageDB.h"
#include "Helper.h"

void ImageAPI::DrawUI(std::string image_name, float x, float y)
{
	ImageDrawRequest request = { image_name, NULL, x, y, 0, 1.0f, 1.0f, 0.0f, 0.0f, 255, 255, 255, 255, 0 };
	renderer->uiDrawCalls.push_back(request);
}

void ImageAPI::DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, int sorting_order)
{
	ImageDrawRequest request = { image_name, NULL, x, y, 0, 1.0f, 1.0f, 0.0f, 0.0f, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), sorting_order };
	renderer->uiDrawCalls.push_back(request);
}

void ImageAPI::DrawTile(int tile_id, int tile_size, std::string source_image, float x, float y, int sorting_order)
{
	float src_width_pixels;
	float src_height_pixels;
	Helper::SDL_QueryTexture(ImageDB::LoadActorImage(source_image), &src_width_pixels, &src_height_pixels);
	float rect_x = (tile_id % (static_cast<int>(src_width_pixels) / tile_size)) * tile_size;
	float rect_y = (tile_id / (static_cast<int>(src_width_pixels) / tile_size)) * tile_size;
	SDL_FRect* srcrect = new SDL_FRect; //deleted after executing the draw request. TODO: std::move this so that the request "owns" it?
	srcrect->x = rect_x;
	srcrect->y = rect_y;
	srcrect->w = tile_size;
	srcrect->h = tile_size;
	ImageDrawRequest request = { source_image, srcrect, x, y, 0, 1.0f, 1.0f, 0.0f, 0.0f, 255, 255, 255, 255, sorting_order };
	renderer->actorDrawCalls.push_back(request);
}

void ImageAPI::Draw(std::string image_name, float x, float y)
{
	ImageDrawRequest request = { image_name, NULL, x, y, 0, 1.0f, 1.0f, 0.5f, 0.5f, 255, 255, 255, 255, 0 };
	renderer->actorDrawCalls.push_back(request);
}

void ImageAPI::DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order) {
	ImageDrawRequest request = { image_name, NULL, x, y, static_cast<int>(rotation_degrees), scale_x, scale_y, pivot_x, pivot_y, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a), static_cast<int>(sorting_order) };
	renderer->actorDrawCalls.push_back(request);
}

void ImageAPI::DrawPixel(float x, float y, float r, float g, float b, float a)
{
	PixelDrawRequest request = { x, y, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a)};
	renderer->pixelDrawCalls.push_back(request);
}
