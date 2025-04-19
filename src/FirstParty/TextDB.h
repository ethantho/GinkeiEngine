#pragma once

#include "SDL_ttf.h"
#include <string>
#include <unordered_map>
#include <vector>

class TextDB
{
public:
	static void Init();
	static void LoadFont(std::string font_name, int point_size);
	static TTF_Font* GetFont(std::string font_name, int point_size);

	static std::vector<std::string> loaded_text;
	static TTF_Font* loaded_font;
	static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;

};

