#pragma once

#include "SDL_image.h"
#include <string>
#include <unordered_map>

class ImageDB
{

public:
	static SDL_Texture* LoadActorImage(std::string image_name);
	static inline std::unordered_map<std::string, SDL_Texture*> actor_images_loaded;
	static SDL_Renderer* sdl_renderer;
	static void CreateDefaultParticleTextureWithName(const std::string& name);

};

