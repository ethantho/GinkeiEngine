#include "Helper.h"
#include "ImageDB.h"
#include "Renderer.h"
#include "TextDB.h"

void Renderer::CreateWindow(const char* title, int w, int h)
{
	window_height = h;
	window_width = w;
	sdl_window = Helper::SDL_CreateWindow(title, 100, 100, w, h, 0);

}

void Renderer::CreateRenderer(Uint8 r, Uint8 g, Uint8 b)
{
	sdl_renderer = Helper::SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	Uint8 a = 255;
	SDL_SetRenderDrawColor(sdl_renderer, r, g, b, a);
	SDL_RenderClear(sdl_renderer);
	//std::string s = SDL_GetError();
	
}

void Renderer::PresentRender()
{

	Helper::SDL_RenderPresent(sdl_renderer);
	frame_counter++;
}

void Renderer::DrawImage(SDL_Texture* img)
{
	Helper::SDL_RenderCopy(sdl_renderer, img, NULL, NULL);
}

void Renderer::Clear()
{
	SDL_SetRenderDrawColor(sdl_renderer, clear_r, clear_g, clear_b, clear_a);
	SDL_RenderClear(sdl_renderer);
}

void Renderer::DrawText(const std::string& text_content, std::string font_name, int font_size, SDL_Color font_color, int x, int y)
{
	if (text_content.empty()) {
		return;
	}
	//The staff solution works by storing a struct of the parameters into a vector. At end-of-frame, the renderer draws all requests in-order at once, then clears the vector. Must call every frame to keep text
	SDL_Surface* surface = TTF_RenderText_Solid(TextDB::GetFont(font_name, font_size), text_content.c_str(), font_color);
	std::string error = SDL_GetError();
	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
	SDL_FRect rect = { (float)x, (float)y, (float)surface->w, (float)surface->h };
	Helper::SDL_RenderCopy(sdl_renderer, texture, NULL, &rect);
	SDL_FreeSurface(surface);

}

void Renderer::DrawAllText()
{
	while (!textDrawCalls.empty()) {
		auto& call = textDrawCalls.front();
		SDL_Color color = { static_cast<Uint8>(call.r), static_cast<Uint8>(call.g), static_cast<Uint8>(call.b), static_cast<Uint8>(call.a) };
		DrawText(call.str_content, call.font_name, call.font_size, color, static_cast<int>(call.x), static_cast<int>(call.y));
		textDrawCalls.pop();
	}
}

void Renderer::DrawUI()
{
	std::stable_sort(uiDrawCalls.begin(), uiDrawCalls.end(), [](const ImageDrawRequest& a, const ImageDrawRequest& b) { return a.sort_order < b.sort_order; });
	for(auto& call : uiDrawCalls){
		DoUIDrawRequest(call);
	}
	uiDrawCalls.clear();
}

void Renderer::DrawActorImages()
{
	std::stable_sort(actorDrawCalls.begin(), actorDrawCalls.end(), [](const ImageDrawRequest& a, const ImageDrawRequest& b) { return a.sort_order < b.sort_order; });
	for (auto& call : actorDrawCalls) {
		DoActorDrawRequest(call);
	}
	actorDrawCalls.clear();
}

void Renderer::DrawAllPixels()
{
	for (auto& call : pixelDrawCalls) {
		SDL_SetRenderDrawColor(sdl_renderer, call.r, call.g, call.b, call.a);
		SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderDrawPoint(sdl_renderer, static_cast<int>(call.x), static_cast<int>(call.y));
		SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_NONE);
	}
	pixelDrawCalls.clear();
}

void Renderer::DoUIDrawRequest(ImageDrawRequest& request)
{
	SDL_Texture* img = ImageDB::LoadActorImage(request.image_name);
	float width;
	float height;
	Helper::SDL_QueryTexture(img, &width, &height);
	SDL_FRect rect = { request.x, request.y, width,  height };
	SDL_FPoint center = {};
	SDL_RendererFlip flip = static_cast<SDL_RendererFlip>(0);
	if (request.scale_x < 0)
	{
		flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
	}
	if (request.scale_y < 0)
	{
		flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
	}
	SDL_SetTextureColorMod(img, request.r, request.g, request.b);
	SDL_SetTextureAlphaMod(img, request.a);
	Helper::SDL_RenderCopyEx(1, "name", sdl_renderer, img, NULL, &rect, request.rotation_degrees, &center, flip);
	SDL_SetTextureColorMod(img, 255, 255, 255);
	SDL_SetTextureAlphaMod(img, 255);
}

void Renderer::DoActorDrawRequest(ImageDrawRequest& request)
{
	glm::vec2 cam_vec = cam_position + cam_offset;

	SDL_RendererFlip flip = static_cast<SDL_RendererFlip>(0);
	if (request.scale_x < 0)
	{
		flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
	}
	if (request.scale_y < 0)
	{
		flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
	}

	SDL_Texture* img = ImageDB::LoadActorImage(request.image_name);
	float width;
	float height;
	Helper::SDL_QueryTexture(img, &width, &height);

	float scaled_width = (float)glm::abs(width * request.scale_x);
	float scaled_height = (float)glm::abs(height * request.scale_y);

	//TEMPORARY BODGE FOR TILES:
	if (request.srcrect != NULL) {
		scaled_width = 32;
		scaled_height = 32;
	}

	float final_pivot_x = request.pivot_x * glm::abs(scaled_width);
	float final_pivot_y = request.pivot_y * glm::abs(scaled_height);

	float render_x = ((float)request.x * 100) + ((float)window_width * 0.5f) - (final_pivot_x) - (100.0f * cam_vec.x);
	render_x -= window_width * (zoom_factor - 1) / (2 * zoom_factor);

	float render_y = ((float)request.y * 100) + ((float)window_height * 0.5f) - (final_pivot_y) - (100.0f * cam_vec.y);
	render_y -= window_height * (zoom_factor - 1) / (2 * zoom_factor);

	SDL_FRect rect = { render_x, render_y, scaled_width,  scaled_height };
	SDL_FPoint center = { final_pivot_x, final_pivot_y };

	float left_bound = -1.0f;
	float right_bound = (((float)window_width) / zoom_factor) + 1.0f;
	float up_bound = -1.0f;
	float down_bound = (((float)window_height) / zoom_factor) + 1.0f;

	if (render_x + (scaled_width) < left_bound) {
		if (request.srcrect != NULL) delete request.srcrect;
		return;
	}
	if (render_x - (scaled_width) > right_bound) {
		if (request.srcrect != NULL) delete request.srcrect;
		return;
	}
	if (render_y + (scaled_height) < up_bound) {
		if (request.srcrect != NULL) delete request.srcrect;
		return;
	}
	if (render_y - (scaled_height) > down_bound) {
		if (request.srcrect != NULL) delete request.srcrect;
		return;
	}

	SDL_RenderSetScale(sdl_renderer, zoom_factor, zoom_factor);

	SDL_SetTextureColorMod(img, request.r, request.g, request.b);
	SDL_SetTextureAlphaMod(img, request.a);

	Helper::SDL_RenderCopyEx(1, "name", sdl_renderer, img, request.srcrect, &rect, request.rotation_degrees, &center, flip);

	SDL_SetTextureColorMod(img, 255, 255, 255);
	SDL_SetTextureAlphaMod(img, 255);

	SDL_RenderSetScale(sdl_renderer, 1.0f, 1.0f);
	if (request.srcrect != NULL) delete request.srcrect;
}


