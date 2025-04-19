#include "ImageDB.h"
#include <filesystem>
#include <iostream>

SDL_Renderer* ImageDB::sdl_renderer;

SDL_Texture* ImageDB::LoadActorImage(std::string image_name)
{
	auto itr = actor_images_loaded.find(image_name);
	if (itr != actor_images_loaded.end()) {
		return itr->second;
	}

	const std::filesystem::path resources_dir = "resources/images/" + image_name + ".png";

	if (!std::filesystem::exists(resources_dir)) {
		std::cout << "error: missing image " << image_name;
		exit(0);
	}

	//return IMG_LoadTexture(sdl_renderer, ("resources/images/" + image_name + ".png").c_str());
	actor_images_loaded[image_name] = IMG_LoadTexture(sdl_renderer, ("resources/images/" + image_name + ".png").c_str());
	return actor_images_loaded[image_name];
}

void ImageDB::CreateDefaultParticleTextureWithName(const std::string& name)
{
	if (actor_images_loaded.find(name) != actor_images_loaded.end()) {
		return;
	}

	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);

	Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
	SDL_FillRect(surface, NULL, white_color);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);

	SDL_FreeSurface(surface);
	actor_images_loaded[name] = texture;

}

